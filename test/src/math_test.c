#include <stdlib.h>
#include "bsp.h"

#define BASE_10 10

static uint16_t doCalculationU16(const uint32_t a, const uint32_t b, const uint32_t c)
{
    // 17 * (17 + 111) % 255 == (17 * 128) % 255 == 2176 % 255 == 136
    const uint32_t result = a * (a + b) % 255;
    // 136 / 3 == 45
    const uint32_t result2 = result / c;
    // 45 - 1 == 44
    return (uint16_t)result2 - 1;
}

static int16_t doCalculationI16(const int32_t a, const int32_t b, const int32_t c)
{
    // 17 * (17 + 111) % 255 == (17 * 128) % 255 == 2176 % 255 == 136
    const int32_t result = a * (a + b) % 255;
    // 136 / 3 == 45
    const int32_t result2 = result / c;
    // 45 - 1 == 44
    return (int16_t)result2 - 1;
}

static int doCalculationI(const int a, const int b, const int c)
{
    // 17 * (17 + 111) % 255 == (17 * 128) % 255 == 2176 % 255 == 136
    const int result = a * (a + b) % 255;
    // 136 / 3 == 45
    const int result2 = result / c;
    // 45 - 1 == 44
    return result2 - 1;
}

void main()
{
    uart_init();
    char buffer[16];
    const uint32_t a = 17;
    const uint32_t b = 111;
    const uint32_t c = 3;
    const uint16_t resultU16 = doCalculationU16(a, b, c);
    const int16_t resultI16 = doCalculationI16(a, b, c);
    const int resultI = doCalculationI(a, b, c);
    uart_write_string("Test calculation from ");
    itoa(a, buffer, BASE_10);
    uart_write_string(buffer);
    uart_write_string(" and ");
    itoa(b, buffer, BASE_10);
    uart_write_string(buffer);
    uart_write_string(" yields ");
    itoa((int)resultU16, buffer, BASE_10);
    uart_write_string(buffer);
    uart_write_string(" == ");
    itoa((int)resultI16, buffer, BASE_10);
    uart_write_string(buffer);
    uart_write_string(" == ");
    itoa((int)resultI, buffer, BASE_10);
    uart_write_string(buffer);
    while (true)
        asm volatile ("nop");
}