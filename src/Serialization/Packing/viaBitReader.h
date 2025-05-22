// src\Tool\Serialization\Packing\viaBitReader.h - packing functions using a bit manipulation library
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include "Serialization/Config/DataFormat.h"
#include "Tool/CompileTimeConfigure.h"

namespace Serialization::detail_::Packing {
namespace detail_ {
#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif
#define BIT_CLEAR(value, bitIdx) ((value) &= ~(1<<(bitIdx)))
#define BIT_CHECK2(value, bitIdx) ((value >> bitIdx) & (1))
#define BIT_UPDATE2(value, bitIdx, bitBoolValue) BIT_CLEAR((value), (bitIdx)); (value) |= ((bitBoolValue)<<bitIdx);
#define setSingleBit(byte, bitIdx, bitValue) BIT_UPDATE2((byte), (bitIdx), (bitValue))
#define getSingleBit(byte, bitIdx) BIT_CHECK2((byte), (bitIdx))
#define TO_UINT8_PTR(expr) ((uint8_t *)expr)

/**
 * @struct BitAddress
 * @brief Lightweight bit reader/writer for arbitrary bit-level access
 * @details Based on Antoine Beauchamp's BitReader (http://github.com/end2endzone/BitReader)
 */
struct BitAddress
{
    uint32_t byte;      ///< Current byte index in the buffer
    uint8_t bit;        ///< Current bit index within the byte
    const uint8_t* mBuffer; ///< Pointer to the buffer

    BitAddress() : byte(0), bit(0), mBuffer(nullptr) {}
    
    /// Reset position to the start of the buffer
    void reset() { byte = 0; bit = 0; }
    
    /// Set the buffer pointer and reset position
    void setBuffer(const void* iBuffer) {
        mBuffer = TO_UINT8_PTR(iBuffer);
        reset();
    }
    
    /// Move forward by a given number of bits
    void add(uint32_t iBits) {
        byte += (iBits / BITS_PER_BYTE);
        bit += (iBits % BITS_PER_BYTE);
        if (bit >= BITS_PER_BYTE) {
            byte++;
            bit -= BITS_PER_BYTE;
        }
    }
    
    /// Move to the next bit
    void next() {
        bit++;
        if (bit == BITS_PER_BYTE) {
            byte++;
            bit = 0;
        }
    }
    
    /**
     * @brief Write bits from a source buffer to the current buffer position
     * @param iSourceBits Pointer to the source bits
     * @param iNumBits Number of bits to write
     */
    void write(const void* iSourceBits, uint32_t iNumBits) {
        BitAddress input;
        for(uint32_t i=0; i<iNumBits; i++) {
            setSingleBit(TO_UINT8_PTR(mBuffer)[byte], bit, 
                        getSingleBit(TO_UINT8_PTR(iSourceBits)[input.byte], input.bit));
            next();
            input.next();
        }
    }
    
    /**
     * @brief Read bits from the buffer into a target buffer
     * @param iNumBits Number of bits to read
     * @param iTargetBits Pointer to the target buffer
     */
    void read(uint32_t iNumBits, void* iTargetBits) {
        BitAddress output;
        for(uint32_t i=0; i<iNumBits; i++) {
            setSingleBit(TO_UINT8_PTR(iTargetBits)[output.byte], output.bit,
                        getSingleBit(mBuffer[byte], bit));
            next();
            output.next();
        }
    }
};

#undef BITS_PER_BYTE
#undef BIT_CLEAR
#undef BIT_CHECK2
#undef BIT_UPDATE2
#undef setSingleBit
#undef getSingleBit
#undef TO_UINT8_PTR
} // namespace detail_

/**
 * @class viaBitReader
 * @brief Bit-level packing and unpacking using BitAddress helper
 * @details Packs and unpacks RawData and PackedData using arbitrary bit widths.
 */
class viaBitReader {
    using PackingPolicy = Tool::CompileTimeConfigure;
public:
    /**
     * @brief Packs input RawData into PackedData buffer using bit-level access
     * @param input Input data array
     * @param buffer Pointer to output packed buffer
     */
    static void pack(RawData const& input, PackedData *buffer) {
        buffer ->fill( 0 );
        detail_::BitAddress bitwriter;
        bitwriter.setBuffer( buffer ->data( ) );
        for ( auto val : input ) {
            PackingPolicy::InputType clampedValue = PackingPolicy::normalize( val );
            bitwriter.write( &clampedValue, PackingPolicy::getOutputBitCount( ) );
        }
    }

    /**
     * @brief Unpacks PackedData buffer into RawData array using bit-level access
     * @param buffer Input packed buffer
     * @param output Pointer to output data array
     */
    static void unpack(PackedData const& buffer, RawData *output) {
        detail_::BitAddress bitreader;
        bitreader.setBuffer( buffer.data( ) );
        for ( auto& val : *output ) {
            PackingPolicy::CapacityCompiler read;
            bitreader.read( PackingPolicy::getOutputBitCount( ), &read );
            val = PackingPolicy::denormalize( read );
        }
    }
};
} // namespace Serialization::detail_::Packing
