#pragma once
#include <mlang/ilexer.hpp>
#include <mlang/token.hpp>

namespace mlang
{
class Lexer : public ILexer
{
public:
    Lexer(std::string_view input);
    Token next_token() override;

private:
    auto read_identifier() -> std::string;
    auto read_number() -> std::string;
    auto read_string() -> std::string;
    void skip_whitespaces();
    void read_char();
    auto peek_char() -> char;
    auto is_letter(char ch) -> bool;
    auto lookup_ident(std::string_view ident) const -> TokenType;

private:
    std::string_view m_input;
    size_t m_pos;
    size_t m_read_pos;
    char m_ch;
};
}  // namespace mlang
