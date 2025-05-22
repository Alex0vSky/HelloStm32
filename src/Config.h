// src\Config.h -- user configuration
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @brief Data configuration for bit packing or unpacking
 * @details Settings are used by the CompileTimeConfigure class for:
 *   - Determining the bit size of elements
 *   - Validating input data
 *   - Calculating the packed size
 * 
 * @warning All values must be constexpr and satisfy the following conditions:
 *   - minimal is less than or equal to maximum
 *   - amount is greater than 0
 *   - type is an unsigned integer type
 * 
 * @note For efficient packing of small ranges:
 *   - Use bit packing for ranges up to 256 values.
 *     After 256 values, bit packing loses its meaning, as the benefit from saving space is offset by the overhead of unpacking.
 *     Switch to regular storage in uint8_t/uint16_t or use compression such as RLE for sequences with small differences.
 *   - Packing efficiency increases for narrow value ranges, as they require fewer bits, while expanding the range increases the required number of bits logarithmically.
 *     The smaller the spread of values, the more compact the packing: halving the range reduces the required number of bits by 1, and increasing by 1 bit allows storing twice as many unique values.
 *   - For values 0-127, Varint encoding is preferable.
 */
namespace Config {
    /**
     * @brief Array element type, only unsigned integers
     * @details Must satisfy std::is_unsigned<type>::value
     */
    using type = unsigned short;

    /**
     * @brief Number of elements in the input array
     */
    constexpr unsigned int amount = 4;

    /**
     * @brief Minimum value of an element (inclusive)
     */
    constexpr unsigned int minimal = 0;

    /**
     * @brief Maximum value of an element (inclusive)
     */
    constexpr unsigned int maximum = 1000;
} // namespace Config