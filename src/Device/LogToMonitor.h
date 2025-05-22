// src\Device\LogToMonitor.h - logging to serial monitor
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <cstdarg>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>

enum LogToMonitorSpec { DEC = 10, HEX = 16 };

/**
 * @class LogToMonitor
 * @brief Provides logging functionality to the serial monitor
 */
namespace Device {
class LogToMonitor {
public:
    /**
     * @brief Initialize the serial monitor with a specific baud rate
     * @param baud Baud rate (default is 9600)
     */
    void begin(uint32_t baud = 9600) {
        rcc_periph_clock_enable(RCC_USART2);
        // Configure USART2 pins (TX=PA2, RX=PA3)
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2); // TX
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO3); // RX

        // Configure USART2, 8N1
        usart_set_baudrate(USART2, baud);
        usart_set_databits(USART2, 8);
        usart_set_stopbits(USART2, USART_STOPBITS_1);
        usart_set_parity(USART2, USART_PARITY_NONE);
        usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
        usart_set_mode(USART2, USART_MODE_TX_RX);
        usart_enable(USART2);
    }

    /**
     * @brief Print a string to the serial monitor
     * @param str String to print
     */
    void print(const char* str) {
        for (const char* p = str; *p; p++) {
            usart_send_blocking(USART2, *p);
        }
    }

    /**
     * @brief Print a newline to the serial monitor
     */
    void println() {
        usart_send_blocking(USART2, '\r');
        usart_send_blocking(USART2, '\n');
    }

    /**
     * @brief Print a string followed by a newline to the serial monitor
     * @param str String to print
     */
    void println(const char* str) {
        print(str);
        println();
    }

    /**
     * @brief Print an integer value in decimal or hexadecimal format
     * @param val Integer value
     * @param base Format base (DEC or HEX)
     */
    void println(int val, LogToMonitorSpec base) {
        char buf[16];
        snprintf(buf, sizeof(buf), base == HEX ? "%x" : "%d", val);
        println(buf);
    }

    /**
     * @brief Printf-style logging to the serial monitor
     * @param fmt Format string
     * @param ... Arguments
     */
    void printf(const char* fmt, ...) {
        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        print(buf);
    }
};
} // namespace Device
