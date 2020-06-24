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

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../../main.h"

enum {CallTracer_MaxCallDepth = 128};

/* r2 or SR, the status register */
typedef struct Status_reg {
  uint16_t carry : 1;      // Carry flag; Set when result produces a carry
  uint16_t zero : 1;       // Zero flag
  uint16_t negative : 1;   // Negative flag
  uint16_t GIE : 1;    // General Inter enable; Enable maskable ints = 1; 0 = dont
  uint16_t CPUOFF : 1;     // CPU off; CPU OFF = 1; CPU ON = 0;
  uint16_t OSCOFF : 1;    // Oscillator Off. LFXT1CLK ON = 0; LFXT1CLK OFF = 1;
  uint16_t SCG0 : 1;  // System Clock Generator DCOCLK DCO ON = 0; DCO OFF = 1;
  uint16_t SCG1 : 1;  // System Clock Generator SMCLK; ON = 0; OFF = 1;
  uint16_t overflow : 1;   // Overflow flag
  uint16_t reserved : 7;   // Reserved bits
} __attribute__((packed)) Status_reg;

// Structure describing call information //
typedef struct CallEntry {
  uint16_t targetPc; // Target call PC
  uint16_t returnPc; // Return PC (one instruction after the call)
  uint16_t sp;       // SP value at the time of the call
} CallEntry;

// Structure containing data for call tracing //
typedef struct CallTracer {
  CallEntry calls[CallTracer_MaxCallDepth]; // Call stack
  uint32_t callDepth;                       // Current call stack depth
} CallTracer;

// Structure containing CPU statistics //
typedef struct CpuStats {
  uint16_t spLowWatermark; // The lowest recorded SP value
  uint16_t spLastValue;    // Last SP value
} CpuStats;

// Main CPU structure //
typedef struct Cpu {
  bool running;      /* CPU running or not */

  uint16_t pc, sp;   /* R0 and R1 respectively */
  uint16_t sr;     /* Status register fields */
  int16_t cg2;       /* R3 or Constant Generator #2 */

  int16_t r4, r5, r6, r7;   /* R4-R15 General Purpose Registers */
  int16_t r8, r9, r10, r11;
  int16_t r12, r13, r14, r15;

  Port_1 *p1;
  Usci *usci;
  Bcm *bcm;
  Timer_a *timer_a;

  CallTracer callTracer;
  CpuStats stats;
} Cpu;

Status_reg get_sr_fields (Emulator* const emu);
void set_sr_from_fields(Emulator* const emu, const Status_reg fields);
void initialize_msp_registers (Emulator* const emu);
void update_register_display (Emulator* const emu);
void update_cpu_stats(Emulator* const emu);
void display_cpu_stats(Emulator* const emu);
void reset_cpu_stats(Emulator* const emu);
void reset_call_tracer(Emulator* const emu);
void report_instruction_execution(Emulator* const emu, const uint16_t instruction);


#endif
