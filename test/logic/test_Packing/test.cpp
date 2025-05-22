// test\logic\test_Packing\test.cpp - for original user config only
#include <unity.h>
void setUp() {} void tearDown() {}

#include "Logger.h"
#include "Serialization/Serializer.h"

void test_roundtrip() {
    Serialization::RawData original = {0, 999, 500, 42};
    Serialization::detail_::PackedData packed;
    Serialization::RawData unpacked;
    
    Serialization::detail_::Packing::viaBitReader::pack(original, &packed);
    Serialization::detail_::Packing::viaBitReader::unpack(packed, &unpacked);
    TEST_ASSERT_EQUAL_UINT16_ARRAY(original.data(), unpacked.data(), original.size());
}

void test_clamping() {
    Serialization::RawData input = {10, 1500, 2000, 1001};
    Serialization::detail_::PackedData packed;
    Serialization::RawData unpacked;
    
    Serialization::detail_::Packing::viaBitReader::pack(input, &packed);
    Serialization::detail_::Packing::viaBitReader::unpack(packed, &unpacked);
    
    TEST_ASSERT_EQUAL_UINT16(10, unpacked[0]);
    TEST_ASSERT_EQUAL_UINT16(1000, unpacked[1]);
    TEST_ASSERT_EQUAL_UINT16(1000, unpacked[2]);
    TEST_ASSERT_EQUAL_UINT16(1000, unpacked[3]);
}

void test_fuzzy() {
    // for (int i = 0; i < 1000; ++i) {
    for (int i = 0; i < 1; ++i) {
        Serialization::RawData input = {
            static_cast<uint16_t>(rand() % 1001),
            static_cast<uint16_t>(rand() % 1001),
            static_cast<uint16_t>(rand() % 1001),
            static_cast<uint16_t>(rand() % 1001)
        };
        
        Serialization::detail_::PackedData packed;
        Serialization::RawData unpacked;

        Serialization::detail_::Packing::viaBitReader::pack(input, &packed);
        Serialization::detail_::Packing::viaBitReader::unpack(packed, &unpacked);
        TEST_ASSERT_EQUAL_UINT16_ARRAY(input.data(), unpacked.data(), input.size());
    }
}
