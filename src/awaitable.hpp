#include <concepts>
#include <coroutine>
#include <type_traits>
#include <utility>

// clang-format off
template<typename type>
concept awaiter = requires(type t, std::coroutine_handle<> c)
{
    { t.await_ready() } -> std::same_as<bool>;
    requires std::same_as<decltype(t.await_suspend(c)), void> ||
        std::same_as<decltype(t.await_suspend(c)), bool> ||
        std::same_as<decltype(t.await_suspend(c)), std::coroutine_handle<>>;
    { t.await_resume() };
};

/**
 * This concept declares a type that can be operator co_await()'ed and returns an awaiter_type.
 */
template<typename type>
concept awaitable = requires(type t)
{
    // operator co_await()
    { t.operator co_await() } -> awaiter;
};

template<typename type>
concept awaiter_void = requires(type t, std::coroutine_handle<> c)
{
    { t.await_ready() } -> std::same_as<bool>;
    requires std::same_as<decltype(t.await_suspend(c)), void> ||
        std::same_as<decltype(t.await_suspend(c)), bool> ||
        std::same_as<decltype(t.await_suspend(c)), std::coroutine_handle<>>;
    {t.await_resume()} -> std::same_as<void>;
};

template<typename type>
concept awaitable_void = requires(type t)
{
    // operator co_await()
    { t.operator co_await() } -> awaiter_void;
};

template<awaitable awaitable, typename = void>
struct awaitable_traits
{
};

template<awaitable awaitable>
static auto get_awaiter(awaitable&& value)
{
    return std::forward<awaitable>(value).operator co_await();
}

template<awaitable awaitable>
struct awaitable_traits<awaitable>
{
    using awaiter_type        = decltype(get_awaiter(std::declval<awaitable>()));
    static_assert(!std::is_const_v<awaitable>, "");
    static_assert(!std::is_const_v<awaiter_type>, "");
    using awaiter_return_type = decltype(std::declval<awaiter_type>().await_resume());
};
// clang-format on