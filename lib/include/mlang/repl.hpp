#pragma once
#include "mlang/lexer.hpp"
#include "mlang/object.hpp"

namespace mlang
{
class Repl
{
public:
    void interactive();
    void exec(std::string_view input, const std::shared_ptr<Context>& env);
};
}  // namespace mlang