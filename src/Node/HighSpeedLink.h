// src\Node\HighSpeedLink.h - high-speed link module, receives a packet via UART, sends it to another module via SPI, and receives data back
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include "Device/HardwareUART.h"
#include "Serialization/Serializer.h"
#include "Device/Blinker.h"
#include "Tool/Hexdumper.h"

namespace Node {
/**
 * @class HighSpeedLink
 * @brief High-speed data transfer module between interfaces (UART <-> SPI)
 * @details Receives a packet via UART, sends it via SPI to another module, and receives data back.
 */
class HighSpeedLink {
    /// Activity indicator, blinks when data is received
    Device::Blinker m_blinker;

    /**
     * @brief SPI transfer helper
     * @details Sends data via SPI and receives response
     * @param tx_buf Pointer to transmit buffer
     * @param rx_buf Pointer to receive buffer
     * @param len Number of bytes to transfer
     */
    void spi_transfer(uint8_t *tx_buf, uint8_t *rx_buf, size_t len) {
        // Enable SPI1 clock
        rcc_periph_clock_enable(RCC_SPI1);

        // Configure pins (PA4=SS, PA5=SCK, PA7=MOSI, PA6=MISO)
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 | GPIO5 | GPIO7);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6);

        // Initialize SPI (Master, 8bit, mode 0)
        spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_8, 
                        SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                        SPI_CR1_CPHA_CLK_TRANSITION_1,
                        SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        spi_enable(SPI1);

        // SS LOW
        gpio_clear(GPIOA, GPIO4);
        // Transfer and receive
        for (size_t i = 0; i < len; i++) {
            rx_buf[i] = spi_xfer(SPI1, tx_buf[i]);
        }
        // SS HIGH
        gpio_set(GPIOA, GPIO4);
        spi_disable(SPI1);
    }

public:
    /**
     * @brief Initialize the module
     * @details Sets up SPI interface and activity indicator
     */
    void begin() {
        m_blinker.begin();
    }

    /**
     * @brief Main processing loop
     * @details Reads data from UART, transfers via SPI, outputs result
     * @param uart Reference to UART interface
     * @param serializer Reference to serializer
     */
    void loop(Device::HardwareUART &uart, Serialization::Serializer &serializer) {
        if (!uart.available()) return;
        // Buffer for incoming data
        Device::HardwareUART::Buffer buffer = { };
        const size_t length = sizeof(buffer);
        if (!uart.readBytes(buffer, length)) return;
        Tool::Hex::dump(buffer, length, "UART");
        // Blink LED after receiving data, will turn off quickly due to SPI
        m_blinker.light();

        // Buffer for SPI response
        Device::HardwareUART::Buffer rx_buf = { };
        spi_transfer(buffer, rx_buf, length);
        // Tool::Hex::dump(rx_buf, length, " SPI");

        // Deserialize data
        Serialization::RawData rawDataRx;
        bool b = serializer.deserialize(rx_buf, length, &rawDataRx);
        LOG("deserialize: %s\r\n", (b ? "TRUE" : "FALSE"));
        if (!b) return;

        // Final result. Format and output time
        uint16_t *source = rawDataRx.data();
        char time[9] = {
            static_cast<char>(source[0] / 10 + '0'), static_cast<char>(source[0] % 10 + '0'),
            ':',
            static_cast<char>(source[1] / 10 + '0'), static_cast<char>(source[1] % 10 + '0'),
            ':',
            static_cast<char>(source[2] / 10 + '0'), static_cast<char>(source[2] % 10 + '0'),
            '\0'
        };
        ((void)time);
        LOG("%s\r\n", time);
    }
};
} // namespace Node
