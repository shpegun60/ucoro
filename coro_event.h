#ifndef CORO_EVENT_H
#define CORO_EVENT_H

#include "u_coro.h"

#if UCORO_ENABLED /* **********************UCORO_ENABLED*************************** */
#include <functional>

// Перелік можливих подій
enum class EventType {
    TIMER1,
    UART_RX,
    GPIO_PIN0,
    // ... інші типи подій ...
    COUNT // Для визначення кількості подій
};


template <EventType E>
struct EventAwaitable;

// Глобальний контролер подій
class EventController {
public:

    static constexpr std::size_t EVENT_COUNT = static_cast<std::size_t>(EventType::COUNT);

    // Реєструє обробник для певної події
    template <EventType E>
    void subscribe(std::function<void()> callback) noexcept {
        callbacks[static_cast<std::size_t>(E)] = std::move(callback);
    }

    // Встановлює подію як таку, що відбулася, і викликає її обробник
    template <EventType E>
    void pend() noexcept {
        constexpr auto index = static_cast<std::size_t>(E);
        if (callbacks[index]) {
            callbacks[index]();
            clear_callback<E>();
        }
    }

    // Перевіряє, чи подія в стані очікування
    template <EventType E>
    bool is_pending() const noexcept {
        return pending_flags[static_cast<std::size_t>(E)];
    }

    // Очищає обробник для певної події
    template <EventType E>
    void clear_callback() noexcept {
        callbacks[static_cast<std::size_t>(E)] = nullptr;
    }

private:
    // Масив для зберігання обробників подій
    std::array<std::function<void()>, EVENT_COUNT> callbacks{};
    // Масив для позначення стану подій (чи відбулися)
    std::array<bool, EVENT_COUNT> pending_flags{};
};

// Глобальний екземпляр контролера подій
inline EventController event_controller;

// Awaiter для асинхронного очікування подій
template <EventType E>
struct EventAwaitable {
    bool await_ready() const noexcept {
        return event_controller.is_pending<E>();
    }

    template<class Promise>
    void await_suspend(std::coroutine_handle<Promise> h) noexcept {
        static_assert(Promise::policy_t::use_blocking, "You must use blocking type Task");

        auto& promise = h.promise();
        promise.block();

        Promise::task_t task;

        event_controller.subscribe<E>([&]() mutable {
            promise.unblock();
        });
    }

    void await_resume() const noexcept {}
};

template<EventType I>
EventAwaitable<I> make_interrupt_awaiter() noexcept {
    return {};
}

#endif /* **********************UCORO_ENABLED*************************** */
#endif // CORO_EVENT_H
