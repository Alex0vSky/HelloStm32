// src\Tool\CompileTimeConfigure.h - compile-time configurator
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <limits>
#include "Config.h"

namespace Tool {
namespace detail_ {
// Count the number of leading zero bits in the value of the given type
// @insp https://gist.github.com/pps83/3210a2f980fd02bb2ba2e5a1fc4a2ef0
template <typename T>
static constexpr unsigned int countLeadingZeros(T x)
{
    for (size_t i = 0; i < sizeof(x) * __CHAR_BIT__; ++i)
    {
        if (x >> (sizeof(x) * __CHAR_BIT__ - 1 - i))
            return i;
    }
    return sizeof(x) * __CHAR_BIT__;
}
} // namespace detail_

// Checks user configuration, ensures correct packing
class CompileTimeConfigure {
public:
    // Capacity type used to calculate bit capacity
    using CapacityCompiler = size_t;
    // Input type
    static_assert(std::is_unsigned<Config::type>::value, "Invalid type");
    using InputType = Config::type;

private:
    // Maximum number of bits in CapacityCompiler type
    static constexpr auto maxPossibleBits = std::numeric_limits<CapacityCompiler>::digits;

    // Output type is always fixed
    using OutputType = uint8_t;

    // Check that minimal and maximum range are not swapped, etc.
    static_assert(Config::maximum >= Config::minimal, "Invalid range: maximum < minimal");
    static_assert(Config::maximum >= 0, "Maximum cannot be negative");
    static_assert(Config::minimal >= 0, "Negative values are not supported");
    static_assert(Config::amount > 0, "Amount must be positive");

    // Check that the maximum value fits in CapacityCompiler type
    static_assert(std::numeric_limits< decltype( Config::maximum ) >::digits <= maxPossibleBits, "Value exceeds capacity compiler");
    // Convert maximum range to CapacityCompiler type
    static constexpr auto rangeSize = static_cast< CapacityCompiler >( Config::maximum - Config::minimal );
    // Number of leading zeros in the maximum value in CapacityCompiler
    static constexpr auto leadingZerosBitsInMaximum = detail_::countLeadingZeros( rangeSize );
    // Check for overflow after placing maximum value in CapacityCompiler
    static_assert(leadingZerosBitsInMaximum <= maxPossibleBits, "Value exceeds capacity compiler");
    // Number of bits to store the value
    static constexpr OutputType bitCount = maxPossibleBits - leadingZerosBitsInMaximum;

    // Number of bits in the input type
    static constexpr auto maxBits = std::numeric_limits< InputType >::digits;
    // Check that there is enough space to store the value
    static_assert(bitCount <= maxBits, "Input type too small");

    // Clamp if exceeds maximum value
    static constexpr InputType clamp(InputType value) {
        const bool below_min = (value < Config::minimal);
        const bool above_max = (value > Config::maximum);
        if (below_min) return Config::minimal;
        if (above_max) return Config::maximum;
        return value;
    }
    // Clear bits outside the stored value
    static constexpr InputType applyMask(CapacityCompiler value) {
        // The mask ensures only bitCount bits are processed
        const InputType bitMask  = ( ( 1u << bitCount ) - 1 );
        return value & bitMask;
    }

public:
    // Number of bytes in the array after packing
    static constexpr size_t packedSize() {
        // Number of bits in the output data
        constexpr size_t total_bits = Config::amount * bitCount;
        // Round up to the nearest byte
        return ( total_bits + 7 ) / 8;
    }
    // Return the number of input data, just a getter
    static constexpr size_t getInputAmount() {
        return Config::amount;
    }
    // Return the number of bits in the output data, just a getter
    static constexpr size_t getOutputBitCount() {
        return bitCount;
    }
    // Take into account the minimum value of the range
    static constexpr InputType normalize(InputType value) {
        // Normalize to [0, max-min]
        return clamp( value ) - Config::minimal;
    }
    static constexpr InputType denormalize(CapacityCompiler value) {
        return applyMask( value ) + Config::minimal;
    }
};
} // namespace Tool