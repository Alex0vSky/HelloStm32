// src\Device\SysTick.h - millis() implementation
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
// for pragmas -- diagnostic ignored "-Wredundant-decls"
#include <libopencm3/cm3/nvic.h>

namespace Device::SysTick {
void init_millis();
} // namespace Device::SysTick

uint32_t millis();
