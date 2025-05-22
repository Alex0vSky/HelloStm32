// test/hardware/test_Uart/test.cpp - 
#include <unity.h>
void setUp() {} void tearDown() {}

#include "Device/HardwareUART.h"

void test_uart_loopback(void) {
	Device::HardwareUART uart;
	uart.begin( 9600 );

    Device::HardwareUART::Buffer in = { };
    uart.write( in, sizeof( in ) );

    Device::HardwareUART::Buffer out = { };
    uart.readBytes( out, sizeof( out ) );

    TEST_ASSERT_EQUAL_HEX8_ARRAY( in, out, sizeof( out ) );
}
