// src\Device\Button\User.h - USER button
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <functional>

namespace Device::Button {
/**
 * @brief Class for working with the USER button (blue)
 * @details Implements debounce and press handling
 */
class User {
    /// USER button pin number
    static constexpr uint32_t k_pin = GPIO13;

    // Time of last state change
    uint32_t m_last_time = 0;

    /**
     * @brief Current processing state
     * @details 0 - not pressed, 1 - initial detection, 2 - confirmed press
     */
    uint8_t m_state = 0;

public:
    enum class Action {
        // Button pressed
        Pressed,
        // Button held
        Held,
        // Button released
        Released
    };

    /**
     * @brief Button initialization
     * @details Configures the pin and waits for button release at startup
     */
    void begin() {
        // Configure button pin via libopencm3
        gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, k_pin);
        // Pull-up
        gpio_set(GPIOC, k_pin);
        // Wait for stable state if button is pressed at startup
        while (!gpio_get(GPIOC, k_pin));		
    }

    /**
     * @brief Main button processing loop
     * @param func Function called on confirmed press
     */
    void loop(std::function<void(Action)> func) {
        // Current button state (LOW - pressed)
        const bool isPressed = !gpio_get(GPIOC, k_pin);

        // State processing
        if ( isPressed ) {
            // First stage - press detection
            if ( m_state == 0 && millis( ) - m_last_time > 20) {
                m_state = 1;
                m_last_time = millis( );
            }
            // Second stage - press confirmation
            else if ( m_state == 1 && millis( ) - m_last_time > 50 ) {
                m_state = 2;
                func( Action::Pressed );
            }
            else if ( m_state == 2 && millis( ) - m_last_time > 500 ) {
                m_state = 3;
                func( Action::Held );
            }
        } else {
            // Reset state on release
            if ( m_state != 0 && millis( ) - m_last_time > 20 ) {
                if ( 1 != m_state )
                    func( Action::Released );
                m_state = 0;
                m_last_time = millis( );
            }
        }
    }
};
} // namespace Device::Button