// src\Device\HardwareUART.h - could be split into several classes and enable DMA mode separately, but there was not enough time
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <stddef.h>
#include <type_traits>
#include "Serialization/Config/DataFormat.h"
#include "Serialization/Config/Hashing.h"

namespace Device {
/**
 * @brief Class for working with UART (USART3) on pins PC10 (RX) and PC11 (TX)
 * @note Implementation via registers, without using HAL/LL
 * @note With DMA and circular buffer
 */
class HardwareUART {
    /// Pointer to USART3 registers
    const uint32_t k_usart = USART3;

    /// DMA buffer size
    static constexpr uint32_t k_DmaBufferSize = 0 
        + sizeof( Serialization::detail_::HashReturnType ) 
        + sizeof( Serialization::detail_::PackedData );

    /// Hardware DMA buffer, filled automatically
    inline static volatile uint8_t dma_buf[k_DmaBufferSize] = { };

    /**
     * @brief Data ready flag
     * @warning Requires atomic access. Only write in interrupt, only read in main loop.
     * @details Set to true when DMA buffer is full (TCIF interrupt).
     *          Reset to false after reading data.
     */
    inline static volatile bool data_ready = false;

    /**
     * @brief DMA interrupt handler, circular mode
     * @note Assume CNDTR is always equal to k_DmaBufferSize on entry
     */
    __attribute__((__used__)) static void DMA1_Channel3_IRQHandler() {
        // Ignore errors
        if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TEIF)) {
            dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TEIF);
            return;
        }
        if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TCIF)) {
            dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TCIF);
            /*
             * In circular mode:
             * - Interrupt occurs when buffer is completely filled
             * - CNDTR is ALREADY reloaded to k_DmaBufferSize at this point
             * - Do not use CNDTR to calculate position!
             */
            data_ready = true;
        }
    }

public:
    /// Buffer type for convenience
    using Buffer = std::remove_volatile_t< decltype( dma_buf ) >;

    /**
     * @brief Read data from DMA buffer
     * @param[out] buffer Pointer to destination buffer
     * @param[in] length Required number of bytes (must == k_DmaBufferSize)
     * @return Actual number of bytes read (0 on error)
     * @warning Not thread-safe! Requires external synchronization if called from multiple threads.
     * @note Blocking operation (waits for data_ready == true)
     */
    size_t readBytes(uint8_t *buffer, size_t length) {
        if ( length != k_DmaBufferSize ) return 0;
        
        // Wait for data indefinitely
        while ( !data_ready );
        // Interrupt protection
        cm_disable_interrupts( );
        for (uint32_t i = 0; i < k_DmaBufferSize; ++i)
            buffer[i] = dma_buf[i];
        // Check for new data
        data_ready = !!(dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TCIF));
        cm_enable_interrupts( );

        return k_DmaBufferSize;
    }

    /**
     * @brief Check if data is available for reading
     * @return Number of available bytes
     */
    int available() {
        return data_ready ?k_DmaBufferSize :0;
    }

    /**
     * @brief Initialize UART (USART3 on PC10, PC11 and DMA with interrupts)
     * @param baud Baud rate
     */
    void begin(uint32_t baud) {
        // Disable USART
        usart_disable(k_usart);
        // Disable DMA
        dma_disable_channel(DMA1, DMA_CHANNEL3);

        rcc_periph_clock_enable(RCC_AFIO);
        rcc_periph_clock_enable(RCC_GPIOC);
        rcc_periph_clock_enable(RCC_USART3);
        // Enable DMA1 clock
        rcc_periph_clock_enable(RCC_DMA1);

        // Configure DMA1 Channel3 for RX
        dma_channel_reset(DMA1, DMA_CHANNEL3);
        dma_set_peripheral_address(DMA1, DMA_CHANNEL3, (uint32_t)&USART_DR(k_usart));
        dma_set_memory_address(DMA1, DMA_CHANNEL3, (uint32_t)dma_buf);
        dma_set_number_of_data(DMA1, DMA_CHANNEL3, k_DmaBufferSize);
        dma_enable_circular_mode(DMA1, DMA_CHANNEL3);
        dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);
        dma_enable_transfer_error_interrupt(DMA1, DMA_CHANNEL3);
        dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL3);

        gpio_primary_remap(AFIO_MAPR_SWJ_CFG_FULL_SWJ, AFIO_MAPR_USART3_REMAP_PARTIAL_REMAP);
        // TX
        gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);
        // RX
        gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO11);

        // 8E2 mode
        usart_set_baudrate(k_usart, baud);
        // 8 data bits + 1 parity bit
        usart_set_databits(k_usart, 9);
        // 2 stop bits
        usart_set_stopbits(k_usart, USART_STOPBITS_2);
        // Even parity
        usart_set_parity(k_usart, USART_PARITY_EVEN);
        usart_set_mode(k_usart, USART_MODE_TX_RX);
        usart_set_flow_control(k_usart, USART_FLOWCONTROL_NONE);
        usart_enable_rx_dma(k_usart);
        usart_enable(k_usart);

        dma_enable_channel(DMA1, DMA_CHANNEL3);
        nvic_set_priority(NVIC_DMA1_CHANNEL3_IRQ, 0);
        nvic_enable_irq(NVIC_DMA1_CHANNEL3_IRQ);
    }

    /**
     * @brief Send a single byte
     * @param c Character to send
     * @return Actual number of bytes sent
     */
    size_t write(char c) {
        usart_send_blocking(USART3, c);
        return sizeof( c );
    }

    /**
     * @brief Send an array of data via UART
     * @param buffer Pointer to buffer with data to send
     * @param size Number of bytes to send
     * @return Actual number of bytes sent
     */
    size_t write(const uint8_t *buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            write(buffer[i]);
        }
        return size;
    }
};
// Alias so the linker sees the handler, as an alternative to mangled name you can wrap the whole class in `extern "C"`
extern "C" void dma1_channel3_isr() __attribute__((alias("_ZN6Device12HardwareUART24DMA1_Channel3_IRQHandlerEv")));
} // namespace Device