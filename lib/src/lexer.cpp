#include "lexer.hpp"

#include <array>

using namespace std::literals;

Lexer::Lexer(std::string_view input)
    : m_input{input}
    , m_pos{0}
    , m_read_pos{0}
    , m_ch{0}
{
    read_char();
}

Token Lexer::next_token()
{
    skip_whitespaces();
    Token tok;

    switch (m_ch)
    {
    case '=':
    {
        if (peek_char() == '=')
        {
            const auto prv = m_ch;
            read_char();
            tok = Token{TokenType::EQ, std::string(1, prv) + std::string(1, m_ch)};
        }
        else
        {
            tok = Token{TokenType::ASSIGN, std::string(1, m_ch)};
        }

        break;
    }
    case '+':
    {
        tok = Token{TokenType::PLUS, std::string(1, m_ch)};
        break;
    }
    case '-':
    {
        tok = Token{TokenType::MINUS, std::string(1, m_ch)};
        break;
    }
    case '!':
    {
        if (peek_char() == '=')
        {
            const auto prv = m_ch;
            read_char();
            tok = tok = Token{TokenType::NOT_EQ, std::string(1, prv) + std::string(1, m_ch)};
        }
        else
        {
            tok = Token{TokenType::BANG, std::string(1, m_ch)};
        }
        break;
    }
    case '/':
    {
        tok = Token{TokenType::SLASH, std::string(1, m_ch)};
        break;
    }
    case '*':
    {
        tok = Token{TokenType::ASTERISK, std::string(1, m_ch)};
        break;
    }
    case '<':
    {
        tok = Token{TokenType::LT, std::string(1, m_ch)};
        break;
    }
    case '>':
    {
        tok = Token{TokenType::GT, std::string(1, m_ch)};
        break;
    }
    case ';':
    {
        tok = Token{TokenType::SEMICOLON, std::string(1, m_ch)};
        break;
    }
    case '"':
    {
        read_char();
        tok = Token{TokenType::STRING, read_string()};
        break;
    }
    case ':':
    {
        tok = Token{TokenType::COLON, std::string(1, m_ch)};
        break;
    }
    case ',':
    {
        tok = Token{TokenType::COMMA, std::string(1, m_ch)};
        break;
    }
    case '(':
    {
        tok = Token{TokenType::LPAREN, std::string(1, m_ch)};
        break;
    }
    case ')':
    {
        tok = Token{TokenType::RPAREN, std::string(1, m_ch)};
        break;
    }
    case '{':
    {
        tok = Token{TokenType::LBRACE, std::string(1, m_ch)};
        break;
    }
    case '}':
    {
        tok = Token{TokenType::RBRACE, std::string(1, m_ch)};
        break;
    }
    case '[':
    {
        tok = Token{TokenType::LBRACKET, std::string(1, m_ch)};
        break;
    }
    case ']':
    {
        tok = Token{TokenType::RBRACKET, std::string(1, m_ch)};
        break;
    }
    case 0:
    {
        tok.type = TokenType::EOFILE;
        break;
    }
    default:
    {
        if (is_letter(m_ch))
        {
            tok.literal = read_identifier();
            tok.type = lookup_ident(tok.literal);
            return tok;
        }
        else if (isdigit(m_ch))
        {
            tok.type = TokenType::INT;
            tok.literal = read_number();
            return tok;
        }
        else
        {
            tok.literal = std::string(1, m_ch);
        }
    }
    };

    read_char();
    return tok;
}

std::string Lexer::read_identifier()
{
    const auto pos = m_pos;
    while (is_letter(m_ch))
    {
        read_char();
    }
    return std::string(m_input.substr(pos, m_pos - pos));
}

auto Lexer::read_number() -> std::string
{
    const auto pos = m_pos;
    while (isdigit(m_ch))
    {
        read_char();
    }
    return std::string(m_input.substr(pos, m_pos - pos));
}

auto Lexer::read_string() -> std::string
{
    const auto pos = m_pos;
    while (m_ch != '"' && m_ch != 0)
    {
        read_char();
    }
    return std::string(m_input.substr(pos, m_pos - pos));
}

void Lexer::skip_whitespaces()
{
    while (isspace(m_ch) && m_ch != 0)
    {
        read_char();
    }
}

void Lexer::read_char()
{
    if (m_read_pos >= m_input.size())
    {
        m_ch = 0;
    }
    else
    {
        m_ch = m_input[m_read_pos];
    }
    m_pos = m_read_pos;
    m_read_pos += 1;
}

char Lexer::peek_char()
{
    if (m_read_pos >= m_input.size())
    {
        return 0;
    }
    return m_input[m_read_pos];
}

bool Lexer::is_letter(char ch)
{
    return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

TokenType Lexer::lookup_ident(std::string_view ident) const
{
    static constexpr auto TOKENS = std::array{
        std::make_pair("fn"sv, TokenType::FUNCTION),
        std::make_pair("let"sv, TokenType::LET),
        std::make_pair("true"sv, TokenType::TRUE),
        std::make_pair("false"sv, TokenType::FALSE),
        std::make_pair("if"sv, TokenType::IF),
        std::make_pair("else"sv, TokenType::ELSE),
        std::make_pair("return"sv, TokenType::RETURN),
        std::make_pair("while"sv, TokenType::WHILE),
    };
    for (const auto [tok_ident, tok_type] : TOKENS)
    {
        if (tok_ident == ident)
        {
            return tok_type;
        }
    }
    return TokenType::IDENT;
}
