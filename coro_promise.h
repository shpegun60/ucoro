#ifndef CORO_PROMISE_H
#define CORO_PROMISE_H

#include "coro_policy.h"

#if UCORO_ENABLED /* **********************UCORO_ENABLED*************************** */

#include <exception>

namespace ucoro {

/*
 * *******************************************************************
 *  Blocking mixin:
 *  if use_blocking==true — contains flag, block/unblock/is_blocked
 *  if use_blocking==false — empty
 * *******************************************************************
*/
template<class Policy, bool = Policy::use_blocking>
struct BlockingMixin { };

template<class Policy>
struct BlockingMixin<Policy, false> { };

template<class Policy>
struct BlockingMixin<Policy, true> {
    typename Policy::template block_t<bool> waiting_for_event{false};

    void block() noexcept {
        if constexpr (Policy::is_atomic) {
            waiting_for_event.store(true, Policy::order);
        } else {
            waiting_for_event = true;
        }
    }

    void unblock() noexcept {
        if constexpr (Policy::is_atomic) {
            waiting_for_event.store(false, Policy::order);
        } else {
            waiting_for_event = false;
        }
    }

    bool is_blocked() const noexcept {
        if constexpr (Policy::is_atomic) {
            return waiting_for_event.load(Policy::order);
        } else {
            return waiting_for_event;
        }
    }
};

/*
 * *******************************************************************
 *  PromiseBase with mixin
 * *******************************************************************
*/
template<class TaskT, class Policy = default_policy>
struct PromiseBase
    : BlockingMixin<Policy>
{
    using policy_t = Policy;
    using task_t = Policy;
    std::exception_ptr error{};

    constexpr std::suspend_always initial_suspend() noexcept { return {}; }
    constexpr std::suspend_always final_suspend()   noexcept { return {}; }

    void unhandled_exception() noexcept {
        error = std::current_exception();
    }

    TaskT get_return_object() noexcept {
        return TaskT{
            TaskT::coro_t::from_promise(static_cast<typename TaskT::promise_type&>(*this))
        };
    }

//    [[noreturn]] static TaskT get_return_object_on_allocation_failure() {
//        throw std::bad_alloc();
//    }
};

}

#endif /* **********************UCORO_ENABLED*************************** */
#endif // CORO_PROMISE_H
