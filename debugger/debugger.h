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

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "../main.h"
#include "../devices/cpu/registers.h"
#include "../devices/memory/memspace.h"

typedef enum { BYTE_STRIDE, WORD_STRIDE, DWORD_STRIDE } Stride;

enum { MAX_BREAKPOINTS = 100 };

enum { ERROR_ILLEGAL_INSTRUCTION = 1 };

typedef struct Debugger
{
  bool disassemble_mode;
  bool debug_mode;
  bool quit;
  int32_t error;

  char mnemonic[50];

  uint16_t bp_addresses[MAX_BREAKPOINTS];
  uint16_t memory_bp_addresses[MAX_BREAKPOINTS];
  uint16_t num_memory_bps;
  uint32_t num_bps;

} Debugger;

void register_signal(int sig);

void setup_debugger(Emulator *emu);

void dump_memory(Emulator *emu, uint8_t *MEM, uint32_t size,
		 uint32_t start_addr, uint8_t stride);

void handle_sigint(int signal);

bool exec_cmd (Emulator *emu, char *buf, int len);

bool handle_breakpoints (Emulator *emu);

#endif
