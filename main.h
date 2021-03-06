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

#pragma once

#define CLI_BUFFER_SIZE 1024
#define VERSION_STRING "1.0.0"
#define PROGRAM_NAME "MSP430 Emulator"

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <getopt.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct Emulator Emulator;

typedef struct Cpu Cpu;
typedef struct Port_1 Port_1;
typedef struct Usci Usci;
typedef struct Bcm Bcm;
typedef struct Timer_a Timer_a;
typedef struct Status_reg Status_reg;

typedef struct Debugger Debugger;
typedef struct Packet Packet;

#include "devices/cpu/registers.h"
#include "devices/utilities.h"
#include "devices/memory/memspace.h"
#include "devices/cpu/decoder.h"
#include "debugger/debugger.h"
#include "debugger/register_display.h"
#include "debugger/disassembler.h"

struct Emulator
{
    Cpu *cpu;
    Debugger *debugger;
    char* binary;
    int port;
    bool do_trace;
    bool start_running;
};
