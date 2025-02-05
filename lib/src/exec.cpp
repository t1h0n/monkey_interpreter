#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <fstream>
#include <mlang/eval.hpp>
#include <mlang/exec.hpp>
#include <mlang/parser.hpp>
#include <string>

namespace mlang
{
namespace detail
{
auto read_file(const fs::path& file_path) -> std::string
{
    if (!fs::exists(file_path) || fs::is_empty(file_path))
    {
        fmt::println("file {} not found or is invalid", fs::absolute(file_path));
        return {};
    }
    const auto f_size = fs::file_size(file_path);
    std::string out_buf(static_cast<std::string::size_type>(f_size), 0);
    std::ifstream file(file_path.c_str(), std::ios::binary);
    file.read(out_buf.data(), f_size);
    return out_buf;
}
}  // namespace detail

void exec(const fs::path& file_path)
{
    auto input = detail::read_file(file_path);

    auto env = std::make_shared<Context>();
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    if (!errors.empty() || !program)
    {
        fmt::println("  parser errors:\n      {}", fmt::join(errors, "\n      "));
        return;
    }
    const auto evaluated = eval(program.get(), env);
}
}  // namespace mlang
