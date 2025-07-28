/*
 * coro_macro.h
 *
 *  Created on: Jul 28, 2025
 *      Author: admin
 */

#ifndef CORO_CORO_MACRO_H_
#define CORO_CORO_MACRO_H_

/*
 * *******************************************************************
 * Argument selectors by number of arguments
 * *******************************************************************
 */

#define _UCORO_GET_ARG_0_3(_0, _1, _2, NAME, ...) NAME
#define _UCORO_GET_ARG_2(_1, _2, NAME, ...) NAME
#define _UCORO_GET_ARG_3(_1, _2, _3, NAME, ...) NAME
#define _UCORO_GET_ARG_4(_1, _2, _3, _4, NAME, ...) NAME

/*
 * *******************************************************************
 * co_yield_now — single unconditional suspension
 * *******************************************************************
 */

#define co_yield_now() co_yield std::suspend_always{}

/*
 * *******************************************************************
 * co_yield_until — wait until condition becomes true
 * Supports 1 or 2 arguments (cond [, expr])
 * *******************************************************************
 */

#define _co_yield_until_impl(cond, expr)        \
    do {                                        \
        co_yield expr;                          \
    } while (!(cond))

#define co_yield_until1(cond)           _co_yield_until_impl(cond, std::suspend_always{})
#define co_yield_until2(cond, expr)     _co_yield_until_impl(cond, expr)

#define co_yield_until(...) \
    _UCORO_GET_ARG_2(__VA_ARGS__, co_yield_until2, co_yield_until1)(__VA_ARGS__)

/*
 * *******************************************************************
 * co_yield_until_timeout — wait until condition or timeout
 * Supports 3 or 4 arguments (cond, timeout, timed_out_var [, expr])
 * *******************************************************************
 */

#define _co_yield_until_timeout_impl(cond, timeout, timed_out_var, expr) 	\
    do {                                                                  	\
        const auto __start = Time::now();                                 	\
        (timed_out_var) = false;                                          	\
        do {                                                              	\
            co_yield expr;                                                	\
            if (Time::now() - __start >= (timeout)) {                     	\
                (timed_out_var) = true;                                   	\
                break;                                                    	\
            }                                                             	\
        } while (!(cond));                                                	\
    } while (false)

#define co_yield_until_timeout3(cond, timeout, timed_out_var) \
    _co_yield_until_timeout_impl(cond, timeout, timed_out_var, std::suspend_always{})

#define co_yield_until_timeout4(cond, timeout, timed_out_var, expr) \
    _co_yield_until_timeout_impl(cond, timeout, timed_out_var, expr)

#define co_yield_until_timeout(...) \
    _UCORO_GET_ARG_4(__VA_ARGS__, co_yield_until_timeout4, co_yield_until_timeout3)(__VA_ARGS__)

/*
 * *******************************************************************
 * co_pause_forever — infinite suspension loop
 * *******************************************************************
 */

#define co_pause_forever() do { co_yield std::suspend_always{}; } while (true)

/*
 * *******************************************************************
 * yield(...) — unified macro for 0–2 arguments
 * - yield();                      → co_yield_now();
 * - yield(cond);                  → co_yield_until(cond);
 * - yield(cond, expr);            → co_yield_until(cond, expr);
 * *******************************************************************
 */

#define _yield0()           co_yield_now()
#define _yield1(cond)       co_yield_until1(cond)
#define _yield2(cond, expr) co_yield_until2(cond, expr)

#if defined(_MSC_VER)
// If MSVC - we can do without GNU hacks
#define _YIELD_EXPAND(...) __VA_ARGS__
#define yield(...) _YIELD_EXPAND( \
    _UCORO_GET_ARG_0_3(__VA_ARGS__, _yield2, _yield1, _yield0)(__VA_ARGS__) \
)
#else
// For GCC/Clang: throw a comma if __VA_ARGS__ is empty
#define yield(...) \
    _UCORO_GET_ARG_0_3(, ##__VA_ARGS__, _yield2, _yield1, _yield0)(__VA_ARGS__)
#endif /* _MSC_VER */

/*
 * *******************************************************************
 * yield_timeout(...) — simplified wrapper for timeout
 * Usage:
 *     bool timed_out;
 *     yield_timeout(cond, timeout_ms, timed_out);
 *     yield_timeout(cond, timeout_ms, timed_out, expr);
 * *******************************************************************
 */

#define yield_timeout(...) \
    co_yield_until_timeout(__VA_ARGS__)

#endif /* CORO_CORO_MACRO_H_ */
