#pragma once
#include <fmt/core.h>
#include <utility>

namespace mlang
{
template <typename Callable>
class RaiiWrapper
{
public:
    RaiiWrapper(const Callable& on_scope_exit)
        : m_on_scope_exit{on_scope_exit}
    {
    }

    RaiiWrapper(Callable&& on_scope_exit)
        : m_on_scope_exit{std::move(on_scope_exit)}
    {
    }

    void disarm() noexcept
    {
        m_disarmed = true;
    }

    ~RaiiWrapper()
    {
        if (!m_disarmed)
        {
            m_on_scope_exit();
        }
    }

private:
    Callable m_on_scope_exit;
    bool m_disarmed = false;
};
}  // namespace mlang