CC=gcc
LDLIBS=-lreadline
EMULATOR=msp430-emu
PREFIX=/usr/local

.PHONY: all test clean

all: ${EMULATOR} ${SERVER}

# Main emulator program

${EMULATOR} : main.o utilities.o registers.o memspace.o debugger.o disassembler.o \
	register_display.o decoder.o flag_handler.o formatI.o formatII.o formatIII.o io.o
	${CC} ${CCFLAGS} -o $@ $^ ${LDLIBS}

main.o : main.c main.h
	${CC} ${CCFLAGS} -c $<

utilities.o : devices/utilities.c devices/utilities.h
	${CC} ${CCFLAGS} -c $<

registers.o : devices/cpu/registers.c devices/cpu/registers.h
	${CC} ${CCFLAGS} -c $<

memspace.o : devices/memory/memspace.c devices/memory/memspace.h
	${CC} ${CCFLAGS} -c $<

debugger.o : debugger/debugger.c debugger/debugger.h
	${CC} ${CCFLAGS} -c $<

disassembler.o : debugger/disassembler.c debugger/disassembler.h
	${CC} ${CCFLAGS} -c $<

register_display.o : debugger/register_display.c debugger/register_display.h
	${CC} ${CCFLAGS} -c $<

decoder.o : devices/cpu/decoder.c devices/cpu/decoder.h
	${CC} ${CCFLAGS} -c $<

flag_handler.o : devices/cpu/flag_handler.c devices/cpu/flag_handler.h
	${CC} ${CCFLAGS} -c $<

formatI.o : devices/cpu/formatI.c devices/cpu/formatI.h
	${CC} ${CCFLAGS} -c $<

formatII.o : devices/cpu/formatII.c devices/cpu/formatII.h
	${CC} ${CCFLAGS} -c $<

formatIII.o : devices/cpu/formatIII.c devices/cpu/formatIII.h
	${CC} ${CCFLAGS} -c $<

io.o: debugger/io.c debugger/io.h
	${CC} ${CCFLAGS} -c $<

clean :
	rm -f main.o utilities.o emu_server.o registers.o \
		memspace.o debugger.o disassembler.o \
		register_display.o decoder.o flag_handler.o formatI.o \
		formatII.o formatIII.o io.o \
		${EMULATOR}

install : ${EMULATOR}
	install -d ${PREFIX}/bin
	install $< ${PREFIX}/bin
