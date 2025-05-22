// src\main.cpp -- entry point for the application
#include "Device/SysTick.h"
#include "Device/LogToMonitor.h"
#include "Logger.h"
#include "Device/HardwareUART.h"
#include "Device/Blinker.h"
#include "Node/TelemetryUnit.h"

/**
 * @brief Main application entry point
 */
int main() {
    // Initialize hardware and modules
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    rcc_periph_clock_enable(RCC_GPIOA);
    Device::SysTick::init_millis();

#ifdef DEBUG
    Serial.begin();
#endif // DEBUG

    Device::Blinker led(GPIO13);
    led.begin();

    Device::HardwareUART uart;
    uart.begin(115200);

    Node::TelemetryUnit telemetry;
    telemetry.begin();

    // Main loop
    while (true) {
        telemetry.loop(uart);
        led.toggle();
        // Add delay or other processing as needed
    }
}
