#pragma once
#include <fmt/core.h>
#include <functional>

#if defined(ENABLE_PARSE_TRACING)
#define TRACE() const auto MACRO_trace_var_tmp_ = trace(__func__)

class RaiiWrapper
{
public:
    RaiiWrapper(const std::function<void()>& on_scope_exit)
        : m_on_scope_exit{on_scope_exit}
    {
    }

    ~RaiiWrapper()
    {
        if (m_on_scope_exit)
        {
            m_on_scope_exit();
        }
    }

private:
    std::function<void()> m_on_scope_exit;
};

inline auto trace(std::string_view s)
{
    static int counter = 0;
    fmt::println("{}BEGIN {}", std::string(counter * 4, ' '), s);
    ++counter;
    return RaiiWrapper([s]()
                       {
                        --counter;
                        fmt::println("{}END {}", std::string(counter * 4, ' '), s); });
}
#else
#define TRACE() void(0)
#endif