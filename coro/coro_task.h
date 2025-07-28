#ifndef CORO_TASK_H
#define CORO_TASK_H


#include "coro_policy.h"
#if UCORO_ENABLED /* **********************UCORO_ENABLED*************************** */

#include "coro_promise.h"
#include <utility>

namespace ucoro {

template<class Promise>
class TaskBase {
public:
    using coro_t = std::coroutine_handle<Promise>;

    TaskBase() noexcept = default;
    explicit TaskBase(coro_t h) noexcept : coro(h) {}
    TaskBase(const TaskBase&) = delete;
    TaskBase(TaskBase&& o) noexcept : coro(std::exchange(o.coro, nullptr)) {}
    TaskBase& operator=(const TaskBase&) = delete;
    TaskBase& operator=(TaskBase&& o) noexcept {
        if (this != &o) {
            if (coro) {
                coro.destroy();
            }
            coro = std::exchange(o.coro, nullptr);
        }
        return *this;
    }

    ~TaskBase() {
        if (coro) {
            coro.destroy();
        }
    }

    bool is_valid() const noexcept { return coro != nullptr; }
    bool done() const noexcept { return is_valid() ? coro.done() : true; }
    bool has_error() const noexcept { return is_valid() && coro.promise().error; }
    std::exception_ptr get_error() const noexcept { return is_valid() ? coro.promise().error : nullptr; }

    bool resume() noexcept {
        // if the handle is empty or already at the end — do nothing
        if (!coro || coro.done()) {
            return false;
        }

        if constexpr (Promise::policy_t::use_blocking) {
            if (coro.promise().is_blocked()) {
                // still waiting for the event — remain unfulfilled
                return true;
            }
        }

        // otherwise wake up
        coro.resume();

        // return whether it is not finished yet
        return !coro.done();
    }


protected:
    coro_t coro = nullptr;
};


} /* namespace coro */

#endif /* **********************UCORO_ENABLED*************************** */
#endif // CORO_TASK_H
