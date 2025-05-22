// src\Tool\Serialization\Hash\CrcSoftware.h - software CRC calculation
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <cstdint>
#include <array>
#include "Serialization/Config/DataFormat.h"
#include "Serialization/Hash/ABase.h"

namespace Serialization::detail_::Hash {
/**
 * @class CrcSoftware
 * @brief Calculates CRC in software
 */
class CrcSoftware final : public ABase {
    // Polynomial CRC-8: x^8 + x^2 + x + 1 (0x07)
    static constexpr uint8_t CRC8_POLY = 0x07;
    
    uint8_t m_crc = 0;

    void restart() {
        m_crc = 0;
    }

    void add(uint8_t data) {
        m_crc ^= data;
        for (uint8_t i = 0; i < 8; ++i) {
            if (m_crc & 0x80) {
                m_crc = (m_crc << 1) ^ CRC8_POLY;
            } else {
                m_crc <<= 1;
            }
        }
    }

public:
    /**
     * @brief Calculate CRC for a data buffer
     * @param data Pointer to data
     * @param size Size in bytes
     * @return CRC value
     */
	uint8_t calculate(PackedData const& data) {
        restart();
        for (size_t i = 0; i < data.size(); ++i) {
            add(data[i]);
        }
        return m_crc;
	}
};
} // namespace Serialization::detail_::Hash
