/*
  MSP430 Emulator
  Copyright (C) 2020 Rudolf Geosits (rgeosits@live.esu.edu)

  "MSP430 Emulator" is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  "MSP430 Emulator" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/



#include "main.h"
#include <stdio.h>
#include <fcntl.h>
#include "debugger/io.h"

static void printVersion()
{
    printf(PROGRAM_NAME " version " VERSION_STRING "\n");
    printf("Compiled @ " __DATE__ " : " __TIME__ "\n");
}

static void printHelp()
{
    printVersion();
    printf("The following options are supported:\n");
    printf("-m OFFSET offset in hex for the next binary to load\n");
    printf("-b NAME Load binary file\n");
    printf("-v Print program version\n");
    printf("-h Print this help\n");
    printf("-r Run after loading\n");
    printf("-i NAME Set USCI input pipe/file\n");
    printf("-o NAME Set USCI output pipe/file\n");
    printf("-d NAME Set DIGITAL I/O PORT 1 output pipe/file\n");
}

static bool setEmulatorConfig(Emulator* const emu, int argc, char *argv[])
{
    int option;
    int offset = 0xC000;
    emu->do_trace = false;
    emu->port = -1;
    emu->binary = NULL;
    emu->usci_input_pipe_fd = 0;
    emu->usci_output_pipe_fd = 0;
    emu->usci_input_pipe_name = NULL;
    emu->usci_output_pipe_name = NULL;
    initialize_msp_memspace();
    while ((option = getopt(argc, argv, "hvrm:b:i:o:d:")) != -1)
    {
        switch (option)
        {
            case 'h':
                printHelp();
                return false;
            case 'v':
                printVersion();
                return false;
            case 'm':
                offset = strtol(optarg, (char **)NULL, 16);
                break;
            case 'b':
                load_firmware(emu, optarg, offset);
                break;
            case 'i':
                emu->usci_input_pipe_name = optarg;
                break;
            case 'o':
                emu->usci_output_pipe_name = optarg;
                break;
            case 'd':
                emu->port1_output_pipe_name = optarg;
                break;
            case 'r':
                emu->start_running = true;
                break;
            default:
                printf("Unknown option\n");
                return false;
        }
    }
    return true;
}

static void initializeMsp430(Emulator* const emu)
{
    emu->cpu       = (Cpu *) calloc(1, sizeof(Cpu));
    emu->cpu->bcm  = (Bcm *) calloc(1, sizeof(Bcm));
    emu->cpu->timer_a  = (Timer_a *) calloc(1, sizeof(Timer_a));
    emu->cpu->p1   = (Port_1 *) calloc(1, sizeof(Port_1));
    emu->cpu->usci = (Usci *) calloc(1, sizeof(Usci));

    initialize_msp_registers(emu);

    setup_bcm(emu);
    setup_timer_a(emu);
    setup_port_1(emu);
    setup_usci(emu);
}

static void deinitializeMsp430(Emulator* const emu)
{
    uninitialize_msp_memspace();
    Cpu* const cpu = emu->cpu;
    free(cpu->timer_a);
    free(cpu->bcm);
    free(cpu->p1);
    free(cpu->usci);
    free(cpu);
}

static bool openUsciPipes(Emulator* const emu)
{
    if (emu->usci_input_pipe_name != NULL)
    {
        emu->usci_input_pipe = fopen(emu->usci_input_pipe_name, "rb");
        if (emu->usci_input_pipe == NULL)
        {
            print_console(emu, "Cannot open USCI input pipe\n");
            return false;
        }
        emu->usci_input_pipe_fd = fileno(emu->usci_input_pipe);
        int flags = fcntl(emu->usci_input_pipe_fd, F_GETFL, 0);
        fcntl(emu->usci_input_pipe_fd, F_SETFL, flags | O_NONBLOCK);
    }
    if (emu->usci_output_pipe_name != NULL)
    {
        emu->usci_output_pipe = fopen(emu->usci_output_pipe_name, "wb");
        if (emu->usci_output_pipe == NULL)
        {
            print_console(emu, "Cannot open USCI output pipe\n");
            return false;
        }
        emu->usci_output_pipe_fd = fileno(emu->usci_output_pipe);
    }
    return true;
}

static bool closeUsciPipes(Emulator* const emu)
{
    if (emu->usci_input_pipe != NULL)
    {
        fclose(emu->usci_input_pipe);
        emu->usci_input_pipe = NULL;
        emu->usci_input_pipe_fd = -1;
    }
    if (emu->usci_output_pipe != NULL)
    {
        fclose(emu->usci_output_pipe);
        emu->usci_output_pipe = NULL;
        emu->usci_output_pipe_fd = -1;
    }
    return true;
}

static bool openDioPipes(Emulator* const emu)
{
    if (emu->port1_output_pipe_name != NULL)
    {
        emu->port1_output_pipe = fopen(emu->port1_output_pipe_name, "wb");
        if (emu->port1_output_pipe == NULL)
        {
            print_console(emu, "Cannot open DIGITAL I.O PORT 1 output pipe\n");
            return false;
        }
        emu->port1_output_pipe_fd = fileno(emu->port1_output_pipe);
    }
    return true;
}

static bool closeDioPipes(Emulator* const emu)
{
    if (emu->port1_output_pipe != NULL)
    {
        fclose(emu->port1_output_pipe);
        emu->port1_output_pipe = NULL;
        emu->port1_output_pipe_fd = -1;
    }
    return true;
}

static void handleCommanding(Emulator* const emu)
{
    Cpu* const cpu = emu->cpu;
    // Handle debugger when CPU is not running
    if (!cpu->running)
    {
        char* buffer = readline(NULL);
        const int bufferLength = strlen(buffer);
        exec_cmd(emu, buffer, bufferLength);
        free(buffer);
    }
}

static void handleProcessingStep(Emulator* const emu)
{
    Cpu* const cpu = emu->cpu;
    if (!cpu->running)
        return;
    // Handle Breakpoints
    if (handle_breakpoints(emu))
        return;
    // Instruction Decoder
    decode(emu, fetch(emu, true), EXECUTE);
    // Handle Peripherals
    handle_bcm(emu);
    handle_timer_a(emu);
    handle_port_1(emu);
    handle_usci(emu);
    // Average of 4 cycles per instruction
    mclk_wait_cycles(emu, 4);
}

int mainInernal(int argc, char *argv[], Emulator* const emu)
{
    Debugger* const deb = emu->debugger;
    if (!setEmulatorConfig(emu, argc, argv))
        return 0;

    initializeMsp430(emu);
    Cpu* const cpu = emu->cpu;
    setup_debugger(emu);

    register_signal(SIGINT); // Register Callback for CONTROL-c

    if (!openUsciPipes(emu))
    {
        closeUsciPipes(emu);
        return -1;
    }

    if (!openDioPipes(emu))
    {
        closeDioPipes(emu);
        return -1;
    }

    cpu->running = emu->start_running;
    if (!cpu->running) {
        // display first round of registers
        display_registers(emu);
        disassemble(emu, cpu->pc, 1);
        update_register_display(emu);
    }

    // Fetch-Decode-Execute Cycle (run machine)
    while (!deb->quit)
    {
        handleCommanding(emu);
        handleProcessingStep(emu);
    }

    closeUsciPipes(emu);
    closeDioPipes(emu);
    deinitializeMsp430(emu);
    return 0;
}

int main(int argc, char *argv[])
{
    Emulator *emu = (Emulator *) calloc( 1, sizeof(Emulator) );
    emu->debugger  = (Debugger *) calloc(1, sizeof(Debugger));

    const int result = mainInernal(argc, argv, emu);

    free(emu->debugger);
    free(emu);
    return result;
}
