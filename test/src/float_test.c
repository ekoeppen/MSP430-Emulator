#include <stdio.h>
#include <string.h>
#include "bsp.h"

#define BASE_10 10

static float doCalculationFloat(const float a, const float b, const float c)
{
    // 17 * (17 + 111) - 2040 == (17 * 128) - 2040 == 2176 - 2040 == 136
    const float result = a * (a + b) - 2040.0f;
    // 136 / 3 == 45,(3)
    const float result2 = result / c;
    // 45,(3) - 1 == 44,(3)
    return result2 - 1.0f;
}

static void printFloat(const float a)
{
    char buffer[24];
    const int32_t integer = (int32_t)(a * 10.0f);
    itoa(integer, buffer, BASE_10);
    const size_t length = strlen(buffer);
    buffer[length] = buffer[length-1];
    buffer[length - 1] = '.';
    buffer[length + 1] = 0;
    uart_write_string(buffer);
}

void main()
{
    uart_init();
    char* ftoaBuffer;
    int ftoaStatus;
    const float a = 17.0f;
    const float b = 111.0f;
    const float c = 3.0f;
    const float resultFloat = doCalculationFloat(a, b, c);
    uart_write_string("Test calculation from ");
    printFloat(a);
    uart_write_string(" and ");
    printFloat(b);
    uart_write_string(" yields ");
    printFloat(resultFloat);
    while (true)
        asm volatile ("nop");
}
