// src\Node\TelemetryUnit.h - telemetry module, currently contains a hardcoded data array, sends data to another module via UART
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include "Device/HardwareUART.h"
#include "Serialization/Serializer.h"
#include "Device/Button/User.h"
#include "Tool/Periodical.h"

namespace Node {
/**
 * @class TelemetryUnit
 * @brief Telemetry module, sends data to another module via UART
 * @tparam Periodical Sending period policy (default is PeriodicalTpl)
 */
template<typename Periodical = Tool::PeriodicalTpl< Device::HardwareUART, Serialization::Serializer > >
class TelemetryUnit final : public Periodical {
    static constexpr auto& _1 = std::placeholders::_1;
    static constexpr auto& _2 = std::placeholders::_2;
    /// USER button for manual data modification
    Device::Button::User m_userButton;
    /// Source data for sending, time and timer
    Serialization::RawData m_source = { };
    
    /// Change source data on button press. No synchronization
    void loop_() override {
        using Action = Device::Button::User::Action;
        m_userButton.loop( [this] (Action action) {
                if ( Action::Pressed == action ) {
                    LOG( "$$$ Button USER pressed $$$\r\n" );
                    m_source[ 3 ] = static_cast<uint16_t>( millis( ) / 1000 % 65535 );
                }
            } );
    }

    /**
     * @brief Periodic data sending
     * @details Called by Periodical, handles button press and serializes data
     * @param uart Reference to UART interface
     * @param serializer Reference to serializer
     * @return true if data was sent, false if not time yet
     */
    bool periodical_(Device::HardwareUART &uart, Serialization::Serializer &serializer) {
        return serializer.serialize( m_source, &uart );
    }

public:
    /**
     * @brief Constructor
     * @details Initializes the base Periodical class with the periodical_ callback
     */
    TelemetryUnit(uint32_t period = 500) :
        Periodical( period, &TelemetryUnit::periodical_, this, _1, _2 )
    {}

    /// @brief Module initialization: sets up the button and fills initial data
    void begin() {
        m_userButton.begin( );
        LOG( "\r\n%s\r\n", __TIME__ );
        // Source data array, for example, compilation time and current timer byte
        constexpr uint8_t AsciiZero = '0';
        constexpr uint8_t DecimalBase = 10;
        m_source = {
            (__TIME__[0] - AsciiZero) * DecimalBase + (__TIME__[1] - AsciiZero),
            (__TIME__[3] - AsciiZero) * DecimalBase + (__TIME__[4] - AsciiZero),
            (__TIME__[6] - AsciiZero) * DecimalBase + (__TIME__[7] - AsciiZero),
            static_cast<uint16_t>( millis( ) / 1000 % 65535 )
        };
    }
};
} // namespace Node