// test/hardware/test_Spi/test.cpp - 
#include <unity.h>
void setUp() {} void tearDown() {}

#include "Device/SysTick.h"
#include "Logger.h"
#include "Node/HighSpeedLink.h"

// First its signature
using Unprivate = void (Node::HighSpeedLink::*)(uint8_t *tx_buf, uint8_t *rx_buf, size_t len);
Unprivate g_unprivate;
template <Unprivate ptmf> struct Exploit { 
  static inline struct Unused_ { Unused_() { ::g_unprivate = ptmf; } } unused_;
};
// Seconds its name
template struct Exploit<&Node::HighSpeedLink::spi_transfer>;

void test_spi_loopback(void) {
	Node::HighSpeedLink highSpeedLink;
    highSpeedLink.begin( );

    uint8_t tx_buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t rx_buf[4] = {0};
    (highSpeedLink.*g_unprivate)( tx_buf, rx_buf, 4 );
	TEST_ASSERT_EQUAL_HEX8_ARRAY( tx_buf, rx_buf, 4 );
}
