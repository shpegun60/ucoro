#ifndef CORO_POLICY_H
#define CORO_POLICY_H

// Check C++ version (C++20 or later)
#if __cplusplus < 202002L
#   define UCORO_ENABLED 0
#else
#// Checking the compiler for coroutines support
#if defined(__cpp_impl_coroutine) || defined(__cpp_coroutines) || __has_include(<coroutine>)
#   define UCORO_ENABLED 1
#   include <atomic>
#   include <coroutine>
#else
#   define UCORO_ENABLED 0
#endif
#endif

#if !UCORO_ENABLED
#   warning "[UCORO]: C++20 coroutines are not supported by this compiler or standard version. Protothread.h or Instantthread.h"
#endif /* !UCORO_ENABLED */

#if UCORO_ENABLED /* **********************UCORO_ENABLED*************************** */

namespace ucoro {

struct PlainPolicy {
    static constexpr bool use_blocking          = true;
    static constexpr bool is_atomic             = false;
    static constexpr std::memory_order order    = std::memory_order_relaxed;// it doesn't matter
    template<typename T> using block_t = T;
};

struct VolatilePolicy {
    static constexpr bool use_blocking          = true;
    static constexpr bool is_atomic             = false;
    static constexpr std::memory_order order    = std::memory_order_relaxed;// it doesn't matter
    template<typename T> using block_t = volatile T;
};

struct AtomicPolicy {
    static constexpr bool use_blocking          = true;
    static constexpr bool is_atomic             = true;
    static constexpr std::memory_order order    = std::memory_order_relaxed;
    template<typename T> using block_t = std::atomic<T>;
};

struct NoBlockPolicy {
    // never use locking
    static constexpr bool use_blocking = false;
    static constexpr bool is_atomic    = false; // it doesn't matter
    static constexpr std::memory_order order = std::memory_order_relaxed;// it doesn't matter
    template<typename T> using block_t = T;// it doesn't matter
};

using default_policy = NoBlockPolicy;
}

#endif /* **********************UCORO_ENABLED*************************** */
#endif // CORO_POLICY_H
