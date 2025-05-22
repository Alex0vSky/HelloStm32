// src\Device\Blinker.h - LED blinker control
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @class Blinker
 * @brief Controls an LED for blinking indication
 */
namespace Device {
class Blinker {
    uint32_t m_pin;
public:
    /**
     * @brief Constructor
     * @param pin GPIO pin number for the LED
     */
    explicit Blinker(uint32_t pin);

    /**
     * @brief Initialize the blinker (configure GPIO)
     */
    void begin();

    /**
     * @brief Toggle the LED state
     */
    void toggle();

    /**
     * @brief Set the LED state
     * @param on true to turn on, false to turn off
     */
    void set(bool on);
};
}
