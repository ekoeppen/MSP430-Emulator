TOP_DIR=$(realpath ../)
EMU=${TOP_DIR}/msp430-emu
OUT_DIR=${TOP_DIR}/test/build
SRC_DIR=${TOP_DIR}/test/src

DEVICE=msp430g2553

TI_GCC=msp430-elf-gcc

ifeq (, $(shell which ${TI_GCC}))
# If TI GCC is not present, the try to use the default Linux one.
	CC=msp430-gcc
	OBJCOPY=msp430-objcopy
	CFLAGS=-mmcu=$(DEVICE) -Os -g
	LFLAGS=
else
# If TI GCC is present, use it
	CC=msp430-elf-gcc
	OBJCOPY=msp430-elf-objcopy
	CFLAGS=-mmcu=$(DEVICE) -msmall -Wall -g
	MSP430_GCC_PATH=${shell which ${CC}}
	MSP430_PATH=${shell dirname ${MSP430_GCC_PATH}}
	MSP430_SUPPORT_DIR?=${realpath ${MSP430_PATH}/..}/include
	LFLAGS=-L ${SRC_DIR} -L $(MSP430_SUPPORT_DIR) -T $(DEVICE).ld -Wl,--gc-sections
endif


USCI_UART_TEST = usci_uart_test
MATH_TEST = math_test
DIO_TEST = dio_test
FLOAT_TEST = float_test

.PHONY: all clean test ${USCI_UART_TEST} ${MATH_TEST} ${DIO_TEST} ${FLOAT_TEST}

all: ${OUT_DIR}/${USCI_UART_TEST}.bin \
	${OUT_DIR}/${MATH_TEST}.bin \
	${OUT_DIR}/${DIO_TEST}.bin \
	${OUT_DIR}/${FLOAT_TEST}.bin

${OUT_DIR}:
	mkdir -p $@

${OUT_DIR}/${USCI_UART_TEST}.bin: ${SRC_DIR}/${USCI_UART_TEST}.c ${OUT_DIR}
	${CC} ${CFLAGS} ${LFLAGS} -o ${OUT_DIR}/${USCI_UART_TEST}.elf $<
	${OBJCOPY} -O binary ${OUT_DIR}/${USCI_UART_TEST}.elf $@

${OUT_DIR}/${MATH_TEST}.bin: ${SRC_DIR}/${MATH_TEST}.c ${OUT_DIR}
	${CC} ${CFLAGS} ${LFLAGS} -o ${OUT_DIR}/${MATH_TEST}.elf $<
	${OBJCOPY} -O binary ${OUT_DIR}/${MATH_TEST}.elf $@

${OUT_DIR}/${DIO_TEST}.bin: ${SRC_DIR}/${DIO_TEST}.c ${OUT_DIR}
	${CC} ${CFLAGS} ${LFLAGS} -o ${OUT_DIR}/${DIO_TEST}.elf $<
	${OBJCOPY} -O binary ${OUT_DIR}/${DIO_TEST}.elf $@

${OUT_DIR}/${FLOAT_TEST}.bin: ${SRC_DIR}/${FLOAT_TEST}.c ${OUT_DIR}
	${CC} ${CFLAGS} ${LFLAGS} -o ${OUT_DIR}/${FLOAT_TEST}.elf $<
	${OBJCOPY} -O binary ${OUT_DIR}/${FLOAT_TEST}.elf $@

clean:
	rm -r -f ${OUT_DIR}

test: ${USCI_UART_TEST} ${MATH_TEST} ${DIO_TEST} ${FLOAT_TEST}

${USCI_UART_TEST}: ${OUT_DIR}/${USCI_UART_TEST}.bin
	# Setup input/output
	rm -r -f ${OUT_DIR}/${USCI_UART_TEST}_pipe.in
	rm -r -f ${OUT_DIR}/${USCI_UART_TEST}_pipe.out
	printf "Adventurer;" > ${OUT_DIR}/${USCI_UART_TEST}_pipe.in
	touch ${OUT_DIR}/${USCI_UART_TEST}_pipe.out
	# Run the simulator with the test binary
	printf "reset\ns 10000\nquit\n" |\
		${EMU} -m cli -b ${OUT_DIR}/${USCI_UART_TEST}.bin \
		-i ${OUT_DIR}/${USCI_UART_TEST}_pipe.in \
		-o ${OUT_DIR}/${USCI_UART_TEST}_pipe.out \
		> /dev/null
	# Create reference file
	printf "Hello there!Echo - Adventurer" > ${OUT_DIR}/${USCI_UART_TEST}_pipe.out.ref
	diff ${OUT_DIR}/${USCI_UART_TEST}_pipe.out ${OUT_DIR}/${USCI_UART_TEST}_pipe.out.ref

${MATH_TEST}: ${OUT_DIR}/${MATH_TEST}.bin
	# Setup output
	rm -r -f ${OUT_DIR}/${MATH_TEST}_pipe.out
	touch ${OUT_DIR}/${MATH_TEST}_pipe.out
	# Run the simulator with the test binary
	printf "reset\ns 100000\nquit\n" |\
		${EMU} -m cli -b ${OUT_DIR}/${MATH_TEST}.bin \
		-o ${OUT_DIR}/${MATH_TEST}_pipe.out \
		> /dev/null
	# Create reference file
	printf "Test calculation from 17 and 111 yields 44 == 44 == 44" > ${OUT_DIR}/${MATH_TEST}_pipe.out.ref
	diff ${OUT_DIR}/${MATH_TEST}_pipe.out ${OUT_DIR}/${MATH_TEST}_pipe.out.ref

${DIO_TEST}: ${OUT_DIR}/${DIO_TEST}.bin
	# Setup output
	rm -r -f ${OUT_DIR}/${DIO_TEST}_pipe.out
	touch ${OUT_DIR}/${DIO_TEST}_pipe.out
	# Run the simulator with the test binary
	printf "reset\ns 10000\nquit\n" |\
		${EMU} -m cli -b ${OUT_DIR}/${DIO_TEST}.bin \
		-d ${OUT_DIR}/${DIO_TEST}_pipe.out \
		> /dev/null
	# Create reference file
	printf "0XXXXXXX\n1XXXXXXX\n0XXXXXXX\n" > ${OUT_DIR}/${DIO_TEST}_pipe.out.ref
	diff ${OUT_DIR}/${DIO_TEST}_pipe.out ${OUT_DIR}/${DIO_TEST}_pipe.out.ref

${FLOAT_TEST}: ${OUT_DIR}/${FLOAT_TEST}.bin
	# Setup output
	rm -r -f ${OUT_DIR}/${FLOAT_TEST}_pipe.out
	touch ${OUT_DIR}/${FLOAT_TEST}_pipe.out
	# Run the simulator with the test binary
	printf "reset\ns 1000000\nquit\n" |\
		${EMU} -m cli -b ${OUT_DIR}/${FLOAT_TEST}.bin \
		-o ${OUT_DIR}/${FLOAT_TEST}_pipe.out \
		> /dev/null
	# Create reference file
	printf "Test calculation from 17.0 and 111.0 yields 44.3" > ${OUT_DIR}/${FLOAT_TEST}_pipe.out.ref
	diff ${OUT_DIR}/${FLOAT_TEST}_pipe.out ${OUT_DIR}/${FLOAT_TEST}_pipe.out.ref
