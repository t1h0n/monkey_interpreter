#pragma once
#include "lexer.hpp"
#include "object.hpp"

class Repl
{
public:
    void interactive();
    void exec(std::string_view input, const std::shared_ptr<Context>& env);
};