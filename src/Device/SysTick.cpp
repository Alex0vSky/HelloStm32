// src\Device\SysTick.cpp - system tick timer implementation
// Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <libopencm3/stm32/systick.h>

static volatile uint32_t g_millis;

/**
 * @brief Initializes the system tick timer
 */
void systick_init() {
    // Configure SysTick for 1ms interrupts
    systick_set_reload(72000 - 1);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
}

/**
 * @brief SysTick interrupt handler
 * @details Increments system millisecond counter
 */
extern "C" void sys_tick_handler(void) {
    // Increment global millisecond counter
    ++g_millis;
}

/**
 * @brief Returns the system millisecond counter
 * @return Current value of the millisecond counter
 */
uint32_t millis() { 
    return g_millis; 
}
