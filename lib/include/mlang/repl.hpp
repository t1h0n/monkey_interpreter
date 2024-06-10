#pragma once
#include "mlang/lexer.hpp"
#include "mlang/object.hpp"

namespace mlang
{
void repl_interactive();

namespace detail
{
void exec(std::string_view input, const std::shared_ptr<Context>& env);
}
}  // namespace mlang