// src\Serialization\Config\DataFormat.h - data format configuration
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @brief Data format configuration for serialization
 * @details Defines the structure and types used for packing and unpacking data.
 */
namespace Serialization {
namespace detail_ {
    // Example: packed data structure for serialization
    struct PackedData {
        uint8_t data[8]; ///< Packed data buffer
        // Add more fields as needed
    };
}
}
