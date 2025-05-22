// src\Tool\Info\Building.h - build information
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @brief Provides build information (date, time, etc.)
 */
namespace Tool {
namespace Info {
    /**
     * @brief Returns build date as a string
     */
    inline const char* buildDate() { return __DATE__; }

    /**
     * @brief Returns build time as a string
     */
    inline const char* buildTime() { return __TIME__; }

    // @insp SO/output-compiler-version-in-a-c-program
    #define A0S_STRINGIZE_expander(x) #x 
    #define A0S_STRINGIZE(x) A0S_STRINGIZE_expander( x )
    #define A0S_SEMVER(major, minor, patch) A0S_STRINGIZE( major ) "." A0S_STRINGIZE( minor ) "." A0S_STRINGIZE( patch )

    void Building() {
    #ifdef __clang__
        LOG( "clang verion: %s\n", A0S_SEMVER( __clang_major__, __clang_minor__, __clang_patchlevel__) );
    #else
        LOG( "g++ verion: %s\n", A0S_SEMVER( __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ ) );
    #endif
        LOG( "__cplusplus: %ld\n", __cplusplus );
        LOG( "C++ humanReadable: " );
        if (__cplusplus >= 202101L) 		LOG( "23\n" );
        else if (__cplusplus >= 202002L)	LOG( "20\n" );
        else if (__cplusplus >= 201703L)	LOG( "17\n" );
        else if (__cplusplus >= 201402L)	LOG( "14\n" );
        else if (__cplusplus >= 201103L)	LOG( "11\n" );
        else if (__cplusplus >= 199711L)	LOG( "98\n" );
        else LOG( "pre-standard C++\n" );

        // LOG( "STM32_CORE_VERSION: %d.%d.%d extra %d\n", STM32_CORE_VERSION_MAJOR, STM32_CORE_VERSION_MINOR, STM32_CORE_VERSION_PATCH, STM32_CORE_VERSION_EXTRA );
        // uint32_t uid[3];
        // uid[0] = *(uint32_t *)UID_BASE;
        // uid[1] = *(uint32_t *)(UID_BASE + 4);
        // uid[2] = *(uint32_t *)(UID_BASE + 8);
        // LOG( "Chip ID: %d/%d/%d\n", uid[0], uid[1], uid[2] );

    #ifdef PLATFORMIO
        LOG( "PLATFORMIO: %d\n", PLATFORMIO );
    #endif // PLATFORMIO
    }
    #undef A0S_STRINGIZE_expander
    #undef A0S_STRINGIZE
    #undef A0S_SEMVER
} // namespace Info
} // namespace Tool
