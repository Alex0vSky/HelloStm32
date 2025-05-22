// src\Tool\Periodical.h - support for periodic actions in loop(), based on millis()
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)
#include <functional>

namespace Tool {
/**
 *	@class		PeriodicalTpl
 *	@brief		Template class for periodic execution of actions in loop, based on millis.
 *	@tparam		Args	Set of arguments for the callback function.
 */
template<typename ...Args>
class PeriodicalTpl {
    using Return = bool;

    ///	Period between executions in milliseconds, default is 500ms
    const uint32_t k_period;

    ///	Time for the next execution, updated after each call
    uint32_t m_next = 0;

    ///	Callback function, returns bool and takes Args&...
    using Function = std::function<Return(Args&...)>;
    const Function m_f;

protected:
    /**
     *	@brief		Constructor
     *	@param		args...	Arguments for std::bind
     */
    template<typename ...ArgsCtor>
    PeriodicalTpl(uint32_t period, ArgsCtor... args) :
        k_period( period )
        , m_f( std::bind( args... ) )
    {}

    // If the derived class needs actions outside the period, it can override this
    virtual void loop_() 
    {}

public:
    /// Virtual destructor
    virtual ~PeriodicalTpl() = default;

    /**
     * @brief 	Executes the periodic task with timer overflow protection
     * @details Uses safe calculation of millis() time difference,
     *          ensuring operation even on overflow (~49.7 days). Provides:
     *          - No missed calls on overflow
     *          - Accurate intervals
     * @tparam	Args Template arguments for the callback function
     * @param	args Arguments to pass to the function
     * @return	Result of the callback or false if not time yet
     */
    Return loop(Args&... args) {
        loop_( );
        // Get current system timer value
        const uint32_t milliseconds = millis( );
        // Calculate time since last execution with overflow protection
        const uint32_t elapsed = static_cast<uint32_t>( milliseconds - m_next );
        // Skip if not enough time has passed
        if ( elapsed < k_period ) 
            return false;
        // Schedule next execution, works automatically on overflow
        m_next = milliseconds + k_period;
        // Call the target function
        return m_f( args... );
    }
};
} // namespace Tool