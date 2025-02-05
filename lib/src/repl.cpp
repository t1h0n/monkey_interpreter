#include <fmt/ranges.h>
#include <iostream>
#include <mlang/eval.hpp>
#include <mlang/parser.hpp>
#include <mlang/repl.hpp>

namespace mlang
{
void repl_interactive()
{
    fmt::print(">> ");
    auto env = std::make_shared<Context>();
    std::string input;
    while (std::getline(std::cin, input))
    {
        detail::exec(input, env);
        fmt::print(">> ");
    }
}

namespace detail
{
void exec(std::string_view input, const std::shared_ptr<Context>& env)
{
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    if (!errors.empty() || !program)
    {
        fmt::println("Errors:");
        fmt::println("  parser errors:\n      {}", fmt::join(errors, "\n      "));
        return;
    }
    const auto evaluated = eval(program.get(), env);
    if (evaluated)
    {
        fmt::println("{}", evaluated->inspect());
    }
}
}  // namespace detail
}  // namespace mlang
