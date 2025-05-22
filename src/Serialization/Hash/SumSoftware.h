// src\Serialization\Hash\SumSoftware.h - simple software hash (sum)
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include "Serialization/Config/DataFormat.h"
#include "Serialization/Hash/ABase.h"

/**
 * @class SumSoftware
 * @brief Simple hash calculation by summing bytes
 */
namespace Serialization {
namespace Hash {
class SumSoftware {
public:
    /**
     * @brief Calculate sum hash for a data buffer
     * @param data Pointer to data
     * @param size Size in bytes
     * @return Hash value (sum of bytes)
     */
    uint8_t calculate(const void* data, size_t size) const {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        uint8_t sum = 0;
        for (size_t i = 0; i < size; ++i)
            sum += bytes[i];
        return sum;
    }
};
}
}
