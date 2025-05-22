// src\Tool\Serialization\Serializer.h - serializer
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include "Serialization/Config/DataFormat.h"
#include "Serialization/Config/Hashing.h"
// #include "Serialization/Packing/Ordinary.h"
#include "Serialization/Packing/viaBitReader.h"
#include "Tool/Hexdumper.h"

namespace Serialization {
/**
 * @class Serializer
 * @brief Converter between raw data and stream format with integrity check
 * @details:
 *          - Serialization: packs 4x10 bits + hash -> writes to stream
 *          - Deserialization: reads from buffer -> checks hash -> unpacks
 * @note:
 *       - Only writes to stream during serialization
 *       - Reads from buffer (not stream) during deserialization 
 */
class Serializer {
    using HashReturnType = detail_::HashReturnType;
    using Packing = detail_::Packing::viaBitReader;

    /// Hasher for calculating data checksum
    detail_::Hasher m_hasher;

public:
    /// Hasher initialization
    void begin() {
        m_hasher.begin( );
    }

    /**
     * @brief Serializes data and sends to stream
     * @details Packs data, calculates hash, and sends to stream
     * @tparam T Output stream type
     * @param input Input data for serialization
     * @param stream Pointer to output stream
     * @return true if data was sent successfully, false otherwise
     */
    template<typename T>
    bool serialize(RawData const& input, T *stream) {
        // Buffer for packed data
        detail_::PackedData buffer;
        // Packing
        Packing::pack( input, &buffer );
        // Size of packed data
        const auto size = sizeof( buffer );
        // Calculate hash for integrity check
        const auto hash = m_hasher.calculate( buffer );
//		Tool::Hex::dump( input, "original" );
        Tool::Hex::dump( buffer, "packed" );
        LOG( "hash: %x\r\n", hash );
        // Send to stream: packed data + hash
        return true
                && ( stream ->write( buffer.data( ), size ) == size )
                && ( stream ->write( hash ) == sizeof( hash ) )
            ;
    }

    /**
     * @brief Deserializes data from buffer
     * @details Checks hash and unpacks data
     * @param input Pointer to input buffer
     * @param size Size of input buffer
     * @param output Pointer to output data array
     * @return true if data was deserialized successfully, false otherwise
     */
    bool deserialize(const void *input, size_t size, RawData *output) {
        // Buffer for receiving packed data
        detail_::PackedData buffer;
        // Check minimum packet size
        if ( size < sizeof( buffer ) + sizeof( HashReturnType ) )
            return false;
        // Copy data from input buffer
        memcpy( buffer.data( ), input, sizeof( buffer ) );
//		Tool::Hex::dump( buffer, "packed" );
        const auto bytes = reinterpret_cast< const uint8_t *>( input );
        // Extract hash from input data
        const HashReturnType hashFromInput = bytes[ sizeof( buffer ) ];
//		Serial.print( "hashFromInput: " ); Serial.println( hashFromInput, HEX );
        // Calculate hash for verification
        const HashReturnType hashCalculated = m_hasher.calculate( buffer );
        LOG( "hashCalculated: %x\r\n", hashCalculated );
        // Compare hashes
        if ( hashCalculated != hashFromInput )
            return false;
        // Unpack data into output array
        Packing::unpack( buffer, output );
//		Tool::Hex::dump( output, "unpacked" );
        return true;
    }
};
} //  namespace Serialization