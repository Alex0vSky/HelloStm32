// src\Tool\Serialization\Packing\Ordinary.h - ordinary packing functions
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include "Serialization/Config/DataFormat.h"
#include "Tool/CompileTimeConfigure.h"

namespace Serialization::detail_::Packing {
// The class is designed only for the basic config and was not tested with other configs
static_assert( std::is_same_v< Tool::CompileTimeConfigure::InputType, unsigned short >, "Allowed for only basic config" );
static_assert( Tool::CompileTimeConfigure::getInputAmount( ) == 4, "Allowed for only basic config" );
static_assert( Tool::CompileTimeConfigure::getOutputBitCount( ) == 10, "Allowed for only basic config" );
static_assert( Tool::CompileTimeConfigure::denormalize( 0 ) == 0, "Allowed for only basic config" );

/**
 * @struct Ordinary
 * @brief Provides ordinary (non-bitwise) packing and unpacking for RawData and PackedData
 * @details Packs 4 values of 10 bits each into a byte array, and unpacks them back.
 */
struct Ordinary {
    /**
     * @brief Packs input RawData into PackedData buffer
     * @param input Input data array
     * @param buffer_ Pointer to output packed buffer
     */
    static void pack(RawData const& input, PackedData *buffer_) {
        PackedData &buffer = *buffer_;
        buffer.fill( 0 );

        for ( size_t i = 0; i < input.size( ); i++ ) {

            // Limit value to 1000
            uint16_t val = (input[i] > 1000) ? 1000 : input[i];

            size_t byte_pos = (i * 10) / 8;
            size_t bit_shift = (i * 10) % 8;

            // Place lower bits into the current byte
            buffer[byte_pos] |= (val & 0x3FF) << bit_shift;

            // If value crosses byte boundary, place upper bits into next byte
            if (bit_shift > 2 && byte_pos + 1 < buffer.size()) {
                buffer[byte_pos + 1] |= (val & 0x3FF) >> (8 - bit_shift);
            }
        }
        // Explicitly clear unused bits in the last byte. Leave only the lowest 4 bits.
        buffer.back() &= 0x0F;
    }

    /**
     * @brief Unpacks PackedData buffer into RawData array
     * @param buffer Input packed buffer
     * @param output_ Pointer to output data array
     */
    static void unpack(PackedData const& buffer, RawData *output_) {
        RawData &output = *output_;
        for ( size_t i = 0; i < output.size( ); i++ ) {
            size_t byte_pos = (i * 10) / 8;
            uint16_t low_byte = buffer[byte_pos];
            uint16_t high_byte = (byte_pos + 1 < buffer.size()) ? buffer[byte_pos + 1] : 0;

            size_t bit_offset = (i * 10) % 8;
            // Extract 10 bits for each value
            output[i] = (low_byte >> bit_offset) | ((high_byte << (8 - bit_offset)) & 0x3FF);
        }
    }
};
} // namespace Serialization::detail_::Packing
