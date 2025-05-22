// src\Tool\Hexdumper.h - dumper for visual debugging
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

namespace Tool {
class Hex {
public:
    enum class Mode {
        HexOnly,
        AsciiOnly,
        HexWithAscii,
        HexWithAddress,
        AsciiWithAddress,
        FullDump
    };

private:
    /// Default output mode
    static constexpr auto Default = Mode::HexOnly;

public:
    /**
     * @brief Outputs a formatted data dump
     * @param data Pointer to the data to dump
     * @param len Number of bytes to output
     * @param prefix Line prefix (optional)
     * @param mode Display mode (see Mode)
     * @param bytes_per_line Number of bytes per line (default 16)
     *
     * Modes:
     * - HexOnly: only hexadecimal values
     * - AsciiOnly: only ASCII representation
     * - HexWithAscii: hex and ASCII
     * - HexWithAddress: hex with addresses
     * - AsciiWithAddress: ASCII with addresses
     * - FullDump: full dump (address + hex + ASCII)
     */
    static void dump(const void* data, size_t len, const char* prefix = nullptr, 
                    Mode mode = Default, uint8_t bytes_per_line = 16) 
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        char ascii[17] = {0};
        bool newLineNeeded = false;

        const bool showAscii = (mode == Mode::AsciiOnly || mode == Mode::HexWithAscii || 
                                mode == Mode::AsciiWithAddress || mode == Mode::FullDump);
        const bool showHex = (mode == Mode::HexOnly || mode == Mode::HexWithAscii || 
                            mode == Mode::HexWithAddress || mode == Mode::FullDump);
        const bool showAddress = (mode == Mode::HexWithAddress || 
                                mode == Mode::AsciiWithAddress || 
                                mode == Mode::FullDump);

        for (size_t i = 0; i < len; ++i) {
            if (i % bytes_per_line == 0) {
                if (newLineNeeded && showAscii) {
                    LOG(" |");
                    LOG(ascii);
                    LOG("|"); LOG("\r\n");
                }
                newLineNeeded = true;

                if (prefix) {
                    LOG(prefix);
                    LOG(": ");
                }

                if (showAddress) {
                    LOG("0x%08x   ", reinterpret_cast<uint32_t>(bytes + i));
                }
                memset(ascii, 0, sizeof(ascii));
            }

            if (showHex) {
                LOG("%02x ", bytes[i]);
                if (i % 8 == 7) LOG(" ");
            }

            if (showAscii) {
                ascii[i % bytes_per_line] = (bytes[i] >= 32 && bytes[i] <= 126) ? bytes[i] : '.';
            }
        }

        if (newLineNeeded) {
            if (showAscii) {
                size_t remaining = len % bytes_per_line;
                if (remaining != 0) {
                    for (size_t i = remaining; i < bytes_per_line; ++i) {
                        if (showHex) {
                            LOG("   ");
                            if (i % 8 == 7) LOG(" ");
                        }
                    }
                    LOG(" |");
                    LOG(ascii);
                    LOG("|"); LOG("\r\n");
                }
            }
            else if (showHex) {
                LOG("\r\n");
            }
        }
    }

    template<typename T, size_t N>
    static void dump(std::array< T, N > const& array, const char* prefix = nullptr, 
        Mode mode = Default, uint8_t bytes_per_line = 16) 
    {
        dump( array.data( ), sizeof( array ), prefix, mode, bytes_per_line );
    }

    template<typename T, size_t N>
    static void dump(std::array< T, N > const* array, const char* prefix = nullptr, 
        Mode mode = Default, uint8_t bytes_per_line = 16) 
    {
        dump( array ->data( ), sizeof( *array ), prefix, mode, bytes_per_line );
    }
};
} // namespace Tool