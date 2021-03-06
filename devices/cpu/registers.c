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

#include "registers.h"
#include "../../debugger/io.h"

#define OPCODE_MASK 0xFFC0u
#define OPCODE_CALL_INSTRUCTION 0x1280u

//##########+++ MSP430 Register initialization +++##########
void initialize_msp_registers(Emulator* const emu)
{
  extern uint8_t* MEMSPACE;
  Cpu *cpu = emu->cpu;
  Debugger *debugger = emu->debugger;

  /* Initialize Program Counter to *0xFFFE at boot or reset (WARM)*/
  uint16_t resetIntHandlerAddress = ((uint16_t*)MEMSPACE)[0xfffe / 2];
  cpu->pc = resetIntHandlerAddress;

  /* Stack pointer - set to the end of the address space, should be set by
     the program */
  cpu->sp = 0xFFFF;

  /* Initialize the status register */
  memset(&cpu->sr, 0, sizeof(Status_reg));

  cpu->running = false;
  cpu->cg2 = 0;

  cpu->r4 = cpu->r5 = cpu->r6 = cpu->r7 = cpu->r8 =
    cpu->r9 = cpu->r10 = cpu->r11 = cpu->r12 = cpu->r13 =
    cpu->r14 = cpu->r15 = 0;

  reset_cpu_stats(emu);
  reset_call_tracer(emu);
}

void update_register_display (Emulator* const emu)
{
  Cpu *cpu = emu->cpu;
  char thing[50] = "....";

  if (emu->cpu->running) {

    return;
  }

  sprintf(thing, "%04X", cpu->pc);

  sprintf(thing, "%04X", cpu->sp);

  sprintf(thing, "%04X", cpu->sr);

  sprintf(thing, "%04X",(uint16_t) cpu->cg2);

  sprintf(thing, "%04X",(uint16_t) cpu->r4);

  sprintf(thing, "%04X",(uint16_t) cpu->r5);

  sprintf(thing, "%04X",(uint16_t) cpu->r6);

  sprintf(thing, "%04X",(uint16_t) cpu->r7);

  sprintf(thing, "%04X",(uint16_t) cpu->r8);

  sprintf(thing, "%04X",(uint16_t) cpu->r9);

  sprintf(thing, "%04X", (uint16_t)cpu->r10);

  sprintf(thing, "%04X", (uint16_t)cpu->r11);

  sprintf(thing, "%04X", (uint16_t)cpu->r12);

  sprintf(thing, "%04X", (uint16_t)cpu->r13);

  sprintf(thing, "%04X", (uint16_t)cpu->r14);

  sprintf(thing, "%04X", (uint16_t)cpu->r15);
}

Status_reg get_sr_fields (Emulator* const emu)
{
  Cpu *cpu = emu->cpu;
  uint16_t value = cpu->sr;
  Status_reg fields;

  // reset SR to set it properly...
  memset(&fields, 0, sizeof(Status_reg));

  if (value & 0x8000) fields.reserved |= 0x8000;
  if (value & 0x4000) fields.reserved |= 0x4000;
  if (value & 0x2000) fields.reserved |= 0x2000;
  if (value & 0x1000) fields.reserved |= 0x1000;
  if (value & 0x0800) fields.reserved |= 0x0800;
  if (value & 0x0400) fields.reserved |= 0x0400;
  if (value & 0x0200) fields.reserved |= 0x0200;

  fields.overflow = (value & 0x0100) ? 1 : 0;
  fields.SCG1 =     (value & 0x0080) ? 1 : 0;
  fields.SCG0 =     (value & 0x0040) ? 1 : 0;
  fields.OSCOFF =   (value & 0x0020) ? 1 : 0;
  fields.CPUOFF =   (value & 0x0010) ? 1 : 0;
  fields.GIE =      (value & 0x0008) ? 1 : 0;
  fields.negative = (value & 0x0004) ? 1 : 0;
  fields.zero =     (value & 0x0002) ? 1 : 0;
  fields.carry =    (value & 0x0001) ? 1 : 0;
  return fields;
}

void set_sr_from_fields(Emulator* const emu, const Status_reg fields)
{
  Cpu *cpu = emu->cpu;
  uint16_t r2 = 0;

  // reserved bits not working quite right yet
  if (fields.reserved & 0b1000000) {
    r2 |= 0x8000;
  }
  if (fields.reserved & 0b0100000) {
    r2 |= 0x4000;
  }
  if (fields.reserved & 0b0010000) {
    r2 |= 0x2000;
  }
  if (fields.reserved & 0b0001000) {
    r2 |= 0x1000;
  }
  if (fields.reserved & 0b0000100) {
    r2 |= 0x0800;
  }
  if (fields.reserved & 0b0000010) {
    r2 |= 0x0400;
  }
  if (fields.reserved & 0b0000001) {
    r2 |= 0x0200;
  }

  if (fields.overflow) {
    r2 |= 0x0100;
  }

  if (fields.SCG1) {
    r2 |= 0x0080;
  }

  if (fields.SCG0) {
    r2 |= 0x0040;
  }

  if (fields.OSCOFF) {
    r2 |= 0x0020;
  }

  if (fields.CPUOFF) {
    r2 |= 0x0010;
  }

  if (fields.GIE) {
    r2 |= 0x0008;
  }

  if (fields.negative) {
    r2 |= 0x0004;
  }

  if (fields.zero) {
    r2 |= 0x0002;
  }

  if (fields.carry) {
    r2 |= 0x0001;
  }

  cpu->sr = r2;
}

void update_cpu_stats(Emulator* const emu)
{
  char buffer[STRING_BUFFER_SIZE];
  if (emu->cpu->stats.spLowWatermark > emu->cpu->sp)
  {
    if (emu->do_trace)
    {
      sprintf(buffer, "New SP low watermark - %04X\n", emu->cpu->sp);
      print_console(emu, buffer);
    }
    emu->cpu->stats.spLowWatermark = emu->cpu->sp;
  }

  emu->cpu->stats.spLastValue = emu->cpu->sp;
}

void display_cpu_stats(Emulator* const emu)
{
  char stats[STRING_BUFFER_SIZE];
  sprintf(stats, "CPU stats:\n \tSP low watermark - %04X\n",
    emu->cpu->stats.spLowWatermark);
  print_console(emu, stats);
}

void reset_cpu_stats(Emulator* const emu)
{
  emu->cpu->stats.spLowWatermark = 0xFFFF;
  emu->cpu->stats.spLastValue = 0xFFFF;
}

void reset_call_tracer(Emulator* const emu)
{
  emu->cpu->callTracer.callDepth = 0;
}

static void print_spaces(Emulator* const emu, const uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
    print_console(emu, " ");
}
