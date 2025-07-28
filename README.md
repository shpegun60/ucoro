# uCoro: Lightweight C++20 Coroutine Framework for Embedded Systems

A lightweight header-only C++20 coroutine framework optimized for embedded systems with minimal overhead and manual scheduling control.
It provides:
- Manual scheduling (`Task` + `.resume()`)  
- Event‑based `co_await` awaitables  
- `yield()` / `yield_timeout()` macros for compact suspension loops  
- Timeout support, policies, and protothread‑style helpers  
Features
- 🚀 Manual scheduling with explicit .resume() control
- ⏱️ Timeout support for operations with configurable policies
- 🔄 Protothread-style helpers for synchronous workflows
- 📊 Event-based awaitables for interrupt-driven architectures
- 📦 Zero dynamic allocations in core operations
- 🧩 Policy-based design for customizable behavior
- 🧠 Compact macros for common suspension patterns
---

## 📂 Repository structure

The `ucoro/` folder contains all the core components of the library:

### File Descriptions

- **`InstantCoroutine.h`** — Launches a coroutine once without heap allocation. Fire-and-forget usage.
- **`Instantthread.h`** — Lightweight wrapper to treat a callback-like function as a resumable "thread".
- **`Protothread.h`** — Minimal protothread system using macros, inspired by Adam Dunkels' protothreads.
- **`coro_event.h`** — Awaitable event system: provides `make_event_awaiter<T>()` to suspend on events.
- **`coro_macro.h`** — A collection of coroutine macros like `yield()`, `yield_timeout()`, and infinite suspension helpers.
- **`coro_policy.h`** — Policy classes for blocking, timeouts, and atomic flag handling.
- **`coro_promise.h`** — Core coroutine `promise_type` implementations, wired with policy and task logic.
- **`coro_task.h`** — Defines the `Task<T, Policy>` interface with resume, state tracking, and value access.
- **`u_coro.h`** — Master include header that pulls in everything in correct order.

Include only `u_coro.h` for full access to the library:
```cpp
#include "u_coro.h"

```
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

### 3. Event-Based Waiting
```cpp
#include "coro_event.h"

ucoro::Task<void> wait_for_button() {
    co_await make_event_awaiter<ButtonPressedEvent>();
    led.turn_on();
}
```
### 4. Protothread Integration
```cpp
#include "Protothread.h"

void sensor_handler() {
    PT_BEGIN();
    while (!sensor.ready()) {
        PT_WAIT_UNTIL(sensor.event_triggered());
    }
    process_data();
    PT_END();
}
```
