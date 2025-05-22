// test\logic\test_Hashing\test.cpp - 
#include <unity.h>
void setUp() {} void tearDown() {}

#include "Serialization/Hash/CrcHardware.h"
#include "Serialization/Hash/CrcSoftware.h"

template<typename THasher>
void ordinary() {
    THasher crc;
    crc.begin();

    Serialization::detail_::PackedData data;
    for (auto &value : data) 
        value = rand();
    TEST_ASSERT_EQUAL(crc.calculate(data), crc.calculate(data));
}

void test_ordinary_CrcHardware() {
    ordinary<Serialization::detail_::Hash::CrcHardware>( );
}

void test_ordinary_CrcSoftware() {
    ordinary<Serialization::detail_::Hash::CrcSoftware>( );
}

template<typename THasher>
void damage_first_byte() {
    THasher crc;
    crc.begin();

    Serialization::detail_::PackedData data;
    for (auto &value : data) 
        value = rand();

    const auto hash = crc.calculate(data);
    // damage
    data[0] ^= 'A';
    TEST_ASSERT_NOT_EQUAL(hash, crc.calculate(data));
}

void test_first_CrcHardware() {
    damage_first_byte<Serialization::detail_::Hash::CrcHardware>( );
}

void test_first_CrcSoftware() {
    damage_first_byte<Serialization::detail_::Hash::CrcSoftware>( );
}

template<typename THasher>
void damage_all_bytes() {
    THasher crc;
    crc.begin();

    Serialization::detail_::PackedData data;
    for (auto &value : data) 
        value = rand();

    const auto hash = crc.calculate(data);
    // damage
    for (auto &value : data) {
        value = rand( );
    }
    TEST_ASSERT_NOT_EQUAL(hash, crc.calculate(data));
}

void test_lucky_CrcHardware() {
    damage_all_bytes<Serialization::detail_::Hash::CrcHardware>( );
}

void test_lucky_CrcSoftware() {
    damage_all_bytes<Serialization::detail_::Hash::CrcSoftware>( );
}

void test_forced_crc8_collision() {
    Serialization::detail_::Hash::CrcHardware crc;
    crc.begin();

    using Type = Serialization::detail_::PackedData;
    Type data1 = {0x31, 0x32, 0x33, 0x34, 0x35};
    uint8_t crc1 = crc.calculate(data1);

    // Known CRC32 low byte collision for polinom 0x00000065
    Type data2 = {0x72, 0xDF, 0x80, 0x1A, 0x3B};
    uint8_t crc2 = crc.calculate(data2);

    TEST_ASSERT_NOT_EQUAL_HEX32(crc1, crc2);
    TEST_ASSERT_TRUE(__builtin_memcmp( data1.data(), data2.data(), sizeof( Type ) ) != 0);
}
