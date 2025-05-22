// src\Tool\Serialization\Hash\CrcHardware.h - hardware CRC calculation
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <libopencm3/stm32/crc.h>
#include "Serialization/Config/DataFormat.h"
#include "Serialization/Hash/ABase.h"

namespace Serialization::detail_::Hash {
/**
 * @class CrcHardware
 * @brief Calculates CRC using hardware peripheral
 */
class CrcHardware final : public ABase {
public:
    /**
     * @brief Initialize CRC hardware
     */
    void begin() override {
        rcc_periph_clock_enable(RCC_CRC);
    }

    /**
     * @brief Calculate CRC for a data buffer
     * @param data Pointer to data
     * @param size Size in bytes
     * @return CRC value
     */
    uint32_t calculate(const void* data, size_t size) {
        const size_t blockSize = sizeof(uint32_t);
        const size_t fullBlocks = size / blockSize;
        const size_t remainingBytes = size % blockSize;

        crc_reset();
        // Process full 32-bit blocks
        uint32_t crc = crc_calculate_block(
            reinterpret_cast<const uint32_t*>(data),
            fullBlocks
        );

        // Process remaining bytes
        if (remainingBytes) {
            uint32_t lastWord = 0;
            memcpy(&lastWord, static_cast<const uint8_t*>(data) + fullBlocks * blockSize, remainingBytes);
            crc = crc_calculate(lastWord);
        }
        return crc;
    }
};
} // namespace Serialization::detail_::Hash
