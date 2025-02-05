#pragma once

#include <mlang/token.hpp>

namespace mlang
{
class ILexer
{
public:
    virtual Token next_token() = 0;
    virtual ~ILexer();
};
}  // namespace mlang
