// src\Serialization\Hash\ABase.h - base class for hash algorithms
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @brief Abstract base class for hash algorithms
 * @details Provides interface for hash calculation used in serialization.
 */
namespace Serialization {
namespace Hash {
/**
 * @class ABase
 * @brief Abstract base for hash calculation
 */
class ABase {
public:
    /**
     * @brief Calculate hash for given data
     * @param data Pointer to data
     * @param size Size of data in bytes
     * @return Hash value
     */
    virtual uint8_t calculate(const void* data, size_t size) const = 0;
    virtual ~ABase() = default;
};
}
}
