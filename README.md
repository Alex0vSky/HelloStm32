# STM32 Data Serialization & Telemetry Project

[![STM32](https://img.shields.io/badge/STM32-Nucleo_F103RB-blue)](https://www.st.com/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Enabled-green)](https://platformio.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)

An embedded project for STM32 implementing bit-packed data serialization, telemetry transmission, and high-speed UART-SPI communication.

## Key Features

- **Efficient bit packing** (4x 10-bit values into 5 bytes)
- **Hardware acceleration**: DMA for UART, STM32 hardware CRC
- **Flexible configuration** via `Config.h`
- **Two operational modes**:
  - Telemetry: periodic UART data transmission
  - HighSpeedLink: UART reception → SPI transmission → response processing
- **Debug support**:
  - Hex data dumps
  - USART2 logging (debug builds only)

## Technology Stack

- **Language**: C++17
- **Hardware**: STM32F103RB (Nucleo board)
- **Framework**: libopencm3
- **Tools**: PlatformIO
- **Algorithms**:
  - Bit packing (custom implementation + Antoine Beauchamp's BitReader)
  - CRC-8 (hardware and software implementations)

## Project Structure

```
src/
├── Config.h             # User configuration
├── Device/              # Peripheral drivers
│   ├── Blinker.h        # LED control
│   ├── HardwareUART.h   # DMA-enabled UART
│   └── ...              
├── Node/
│   ├── TelemetryUnit.h  # Telemetry module
│   └── HighSpeedLink.h  # UART↔SPI bridge
├── Serialization/       # Bit packers and hashers
└── Tool/                # Utility classes
```

## Getting Started

1. Install [PlatformIO](https://platformio.org/)
2. Clone the repository:
   ```bash
   git clone https://github.com/your-repo.git
   cd your-repo
   ```
3. Build and upload:
   ```bash
   pio run -t upload
   ```
4. For debugging:
   ```bash
   pio run -e debug -t upload
   ```

## Workflow Example

1. TelemetryUnit sends data (compile time + timer):
   ```
   [UART] 34 12 05 00 00 A1 (packed data + CRC)
   ```
2. HighSpeedLink receives it, transmits via SPI and outputs:
   ```
   [SPI] 34 12 05 00 00 A1
   Deserialized: 10:20:30
   ```

## Contributing

PRs and questions are welcome! No strict requirements for contributors.

## Acknowledgments

- **PlatformIO** for the build system
- **STMicroelectronics** for STM32 and libopencm3
- **Antoine Beauchamp** for [BitReader](http://github.com/end2endzone/BitReader)
- Open-source community for inspiration

## License

Project is MIT licensed. See [LICENSE](https://github.com/Alex0vSky/HelloStm32/blob/main/LICENSE) for details.
