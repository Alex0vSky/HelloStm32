// test/unity_config.h - prepare output for stm32
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

// initMonitorLogging for Nucleo-F103RB, delay in loop for prevent message: 
// "If you don't see any output for the first 10 secs, please reset board (press reset button)""
#define UNITY_OUTPUT_START() \
    rcc_periph_clock_enable(RCC_GPIOA); \
    rcc_periph_clock_enable(RCC_USART2); \
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2); \
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO3); \
    usart_set_baudrate(USART2, 115200); \
    usart_set_databits(USART2, 8); \
    usart_set_stopbits(USART2, USART_STOPBITS_1); \
    usart_set_parity(USART2, USART_PARITY_NONE); \
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE); \
    usart_set_mode(USART2, USART_MODE_TX_RX); \
    usart_enable(USART2); \
    for (volatile uint32_t i = 0; i < 990000; i++); \
    (void)0;

// Outputs
#define UNITY_OUTPUT_CHAR(c)    usart_send_blocking( USART2, c )
#define UNITY_OUTPUT_FLUSH()    UNITY_OUTPUT_CHAR( '\r' )
#define UNITY_PRINT_EOL()       UNITY_OUTPUT_CHAR( '\n' )

// Ommit useless warnings
#pragma GCC diagnostic ignored "-Wredundant-decls"
