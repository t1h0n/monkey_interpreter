#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <mlang/fmt_enum.hpp>
#include <stdexcept>
#include <string>

namespace mlang
{
enum class TokenType : std::uint8_t
{
    ILLEGAL,
    EOFILE,
    // Identifiers + literals
    IDENT,
    INT,
    STRING,
    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,
    LT,
    GT,
    EQ,
    NOT_EQ,
    // Delimiters
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COLON,
    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
    WHILE,
};

struct Token
{
    TokenType type = TokenType::ILLEGAL;
    std::string literal;
};

inline bool operator==(const Token& lhs, const Token& rhs)
{
    return lhs.type == rhs.type && lhs.literal == rhs.literal;
}

inline bool operator!=(const Token& lhs, const Token& rhs)
{
    return !(lhs == rhs);
}

}  // namespace mlang

template <>
struct fmt::formatter<::mlang::Token>
{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator
    {
        auto it = ctx.begin(), end_it = ctx.end();
        if (it != end_it && *it != '}')
        {
            throw std::runtime_error("invalid format");
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const mlang::Token& tok, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "Token{{{}, '{}'}}", tok.type, tok.literal);
    }
};
