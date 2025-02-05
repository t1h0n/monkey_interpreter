#pragma once
#include <fmt/core.h>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <type_traits>

template <typename T>
struct ::fmt::formatter<T, char, std::enable_if_t<std::is_enum_v<T>>>
{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator
    {
        auto it = ctx.begin(), end_it = ctx.end();
        if (it != end_it && *it != '}')
        {
            throw std::runtime_error("invalid format");
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const T& enum_val, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", magic_enum::enum_name(enum_val));
    }
};
