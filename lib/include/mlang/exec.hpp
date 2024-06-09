#pragma once
#include <fs.hpp>

namespace mlang
{
void exec(const fs::path& file_path);

namespace detail
{
auto read_file(const fs::path& file_path) -> std::string;
}
}  // namespace mlang
