// src\Serialization\Config\Hashing.h - hashing configuration
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @brief Hashing configuration for serialization integrity checks
 * @details Used to select and configure the hash algorithm for data integrity.
 */
namespace Serialization {
namespace detail_ {
    // Type for hash function return value
    using HashReturnType = uint8_t;
}
}
