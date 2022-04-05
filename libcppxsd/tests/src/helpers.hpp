#pragma once
#include <concepts>
#include <exception>
#include <functional>
#include <string>
#include <string_view>
// clang-format off
template <typename T>
concept Named = requires(T)
{
    { T::kName } -> std::convertible_to<std::string_view>;
};
// clang-format on

template <Named R>
struct require_type
{
    using CbFnc = std::function<void(const R &)>;
    CbFnc cb;

    void operator()(const R &x) const
    {
        if (cb)
            cb(x);
    }
    template <Named T>
    void operator()(const T &) const
    {
        throw std::runtime_error(std::string{"illegal: "} + std::string{T::kName});
    }
};