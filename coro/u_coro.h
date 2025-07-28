#ifndef COROTASK_H
#define COROTASK_H

#include "coro_policy.h"
#if UCORO_ENABLED /* **********************UCORO_ENABLED*************************** */

#include "coro_task.h"
#include "coro_macro.h"
#include <type_traits>


namespace ucoro {

// Forward declarations

// TaskTraits helps decouple Task and Promise type dependency
template<class T, class Policy>
struct Task;

// Promise primary template
template<class T, class TaskT, class Policy = default_policy>
struct Promise;

/*
 * *******************************************************************
 *  TaskTraits
 * *******************************************************************
*/

template<class T, class Policy>
struct TaskTraits {
    using TaskType = Task<T, Policy>;
    using PromiseType = Promise<T, TaskType, Policy>;
};

/*
 * *******************************************************************
 *  Promise
 * *******************************************************************
*/
// Specialization for non-void T
template<class T, class TaskT, class Policy>
struct Promise : PromiseBase<TaskT, Policy> {
    static_assert(std::is_trivially_copyable_v<T>,
                  "[UCORO]: Type must be trivially copyable");

    T val{};

    void return_value(T v) noexcept { val = std::move(v); }
    auto yield_value(T v) noexcept {
        val = std::move(v);
        return std::suspend_always{};
    }
    constexpr std::suspend_always yield_value(std::suspend_always) noexcept { return {}; }
};

// Specialization for void
template<class TaskT, class Policy>
struct Promise<void, TaskT, Policy> : PromiseBase<TaskT, Policy> {
    constexpr void return_void() noexcept {}
    constexpr std::suspend_always yield_value(std::suspend_always) noexcept { return {}; }
};

/*
 * *******************************************************************
 *  Task
 * *******************************************************************
*/
// Task primary template
template<class T = void, class Policy = default_policy>
struct Task : TaskBase<typename TaskTraits<T, Policy>::PromiseType> {
	using value_type = T;
    using promise_type = typename TaskTraits<T, Policy>::PromiseType;
    using Base = TaskBase<promise_type>;
    using Base::Base;

    const T& value() const & noexcept { return this->coro.promise().val; }
    T&       value() & noexcept { return this->coro.promise().val; }
    T&&      value() && noexcept { return std::move(this->coro.promise().val); }
};

// Task specialization for void
template<class Policy>
struct Task<void, Policy> : TaskBase<typename TaskTraits<void, Policy>::PromiseType> {
	using value_type = void;
	using promise_type = typename TaskTraits<void, Policy>::PromiseType;
    using Base = TaskBase<promise_type>;
    using Base::Base;
};

} // namespace ucoro

#endif /* **********************UCORO_ENABLED*************************** */
#endif // COROTASK_H
