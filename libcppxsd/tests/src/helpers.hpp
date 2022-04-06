#pragma once
#include <concepts>
#include <exception>
#include <functional>
#include <string>
#include <string_view>
#include <fmt/format.h>
// clang-format off
template <typename T>
concept Named = requires(T)
{
    { T::kName } -> std::convertible_to<std::string_view>;
};
template <typename T>
concept NamedRefType = requires(T)
{
    { T::type::kName } -> std::convertible_to<std::string_view>;
};
// clang-format on

struct PrintTypeName
{
    static constexpr std::string_view kUnknownType = "unknownType";

    template <Named T>
    std::string_view getName() const
    {
        return T::kName;
    }

    template <NamedRefType T>
    std::string_view getName() const
    {
        return T::type::kName;
    }

    template <typename T>
    std::string_view getName() const
    {
        return kUnknownType;
    }
};

template <typename R>
struct require_type
{
    using CbFnc = std::function<void(const R &)>;
    CbFnc cb;

    void operator()(const R &x) const
    {
        if (cb)
            cb(x);
    }
    template <typename T>
    void operator()(const T &) const
    {
        throw std::runtime_error(
            fmt::format("[{}]: illegal: {}", PrintTypeName{}.getName<R>(), PrintTypeName{}.getName<T>()));
    }
};
