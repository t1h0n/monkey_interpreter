#include "repl.hpp"

#include "eval.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "parser.hpp"

#include <iostream>

namespace
{
const std::string MONKEY_FACE = R"(            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ '~' /   /
        '._ '-=-' _.'
           '-----'
)";
}  // namespace

void Repl::interactive()
{
    fmt::println("{}", MONKEY_FACE);
    fmt::print(">> ");
    auto env = std::make_shared<Context>();
    std::string input;
    while (std::getline(std::cin, input))
    {
        exec(input, env);
        fmt::print(">> ");
    }
}

void Repl::exec(std::string_view input, const std::shared_ptr<Context>& env)
{
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    if (!errors.empty() || !program)
    {
        fmt::println("Woops! We ran into some monkey business here!");
        fmt::println("  parser errors:\n      {}", fmt::join(errors, "\n      "));
        return;
    }
    const auto evaluated = eval(program.get(), env);
    if (evaluated)
    {
        fmt::println("{}", evaluated->inspect());
    }
}
