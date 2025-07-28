# ucoro

A lightweight, header‑only C++20 coroutine framework for embedded systems.  
It provides:
- Manual scheduling (`Task` + `.resume()`)  
- Event‑based `co_await` awaitables  
- `yield()` / `yield_timeout()` macros for compact suspension loops  
- Timeout support, policies, and protothread‑style helpers  

---

## 📂 Repository layout

ucoro/
├── InstantCoroutine.h — one‐shot coroutine helper
├── Instantthread.h — protothread wrapper for synchronous callbacks
├── Protothread.h — “classic” protothreads API
├── coro_event.h — event subscription awaitable
├── coro_macro.h — yield(), yield_until(), yield_timeout(), co_pause_forever() macros
├── coro_policy.h — policy definitions (e.g. NoBlockPolicy, TimeoutPolicy)
├── coro_promise.h — promise_type implementations (templated on TaskT, Policy)
├── coro_task.h — Task<T,Policy> + TaskBase<> boilerplate
└── u_coro.h — user‑facing includes and forward declarations


---

## 📝 File summaries

### `InstantCoroutine.h`  
Helper to run a coroutine exactly once, without frame allocations. Useful for fire‑and‑forget “quick” coroutines.

### `Instantthread.h`  
Wraps a callback into a “protothread” that can be resumed in response to events, but without full coroutine machinery.

### `Protothread.h`  
A minimal “protothreads” implementation (resembling Adam Dunkels’s Protothreads), for comparison or fallback.

### `coro_event.h`  
Defines `EventAwaitable<E>` which lets you `co_await make_event_awaiter<E>()`.  
Subscribes to a global `event_controller`, calls `promise.block()/unblock()` under the hood.

### `coro_macro.h`  
Your macro library:

- `co_yield_now()`  
- `co_yield_until(cond[, expr])`  
- `co_yield_until_timeout(cond, timeout_ms, timed_out_var[, expr])`  
- `co_pause_forever()`  
- `yield(...)` — unified zero‑to‑two‑argument macro  
- `yield_timeout(...)` — alias for `co_yield_until_timeout`

### `coro_policy.h`  
Policy classes (e.g. `NoBlockPolicy`, `TimeoutPolicy`) that govern how `Promise` handles blocking, timeouts, atomic flags, etc.

### `coro_promise.h`  
`Promise<T,TaskT,Policy>` specializations:

- Stores return value or `void`.  
- Implements `yield_value()`, `return_value()`, `block()/unblock()` from `Policy`.

### `coro_task.h`  
`Task<T,Policy>` + `TaskBase<>`:

- `resume()`, `done()`, `has_error()`  
- `value()` accessors for non‐void tasks.

### `u_coro.h`  
Single header that includes all the above in the correct order.

---

## 🚀 Quickstart & Examples

### 1. Basic Task

```cpp
#include "u_coro.h"

ucoro::Task<void> blink_led() {
    while (true) {
        led.toggle();
        yield();
    }
}

int main() {
    auto t = blink_led();
    while (!t.done()) {
        t.resume();
        delay_ms(500);
    }
}
```

### 2. yield / yield_timeout macros
```cpp
#include "coro_macro.h"

// Wait until flag == true, suspending once per iteration
void ucoro::Task<void> wait_flag() {
    bool timed_out = false;

    // no args  → co_yield_now()
    yield();

    // one   arg → co_yield_until(flag)
    yield(some_flag);

    // two   args → co_yield_until(flag, std::suspend_always{})
    yield(some_flag, std::suspend_always{});

    // timeout: wait up to 1000ms, set timed_out
    yield_timeout(some_flag, 1000, timed_out);
    if (timed_out) {
        // handle timeout
    }
}

```
