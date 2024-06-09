#include "mlang/lexer.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using namespace ::testing;

namespace
{
void validate_lexer(std::string_view input, std::vector<mlang::Token> expected_tokens)
{
    mlang::Lexer lexer(input);
    std::vector<mlang::Token> tokens;
    for (auto token = lexer.next_token(); token.type != mlang::TokenType::EOFILE; token = lexer.next_token())
    {
        tokens.push_back(token);
    }
    tokens.push_back({mlang::TokenType::EOFILE, ""});
    EXPECT_THAT(tokens, ContainerEq(expected_tokens));
}
}  // namespace

TEST(Lexer, Simple)
{
    validate_lexer("=+(){},;", {
                                   {mlang::TokenType::ASSIGN,    "="},
                                   {mlang::TokenType::PLUS,      "+"},
                                   {mlang::TokenType::LPAREN,    "("},
                                   {mlang::TokenType::RPAREN,    ")"},
                                   {mlang::TokenType::LBRACE,    "{"},
                                   {mlang::TokenType::RBRACE,    "}"},
                                   {mlang::TokenType::COMMA,     ","},
                                   {mlang::TokenType::SEMICOLON, ";"},
                                   {mlang::TokenType::EOFILE,    "" }
    });
}

TEST(Lexer, String)
{
    validate_lexer("\"foobar\"", {
                                     {mlang::TokenType::STRING, "foobar"},
                                     {mlang::TokenType::EOFILE, ""      }
    });
}

TEST(Lexer, While)
{
    validate_lexer("while(true){}", {
                                        {mlang::TokenType::WHILE,  "while"},
                                        {mlang::TokenType::LPAREN, "("    },
                                        {mlang::TokenType::TRUE,   "true" },
                                        {mlang::TokenType::RPAREN, ")"    },
                                        {mlang::TokenType::LBRACE, "{"    },
                                        {mlang::TokenType::RBRACE, "}"    },
                                        {mlang::TokenType::EOFILE, ""     }
    });
}

TEST(Lexer, Array)
{
    validate_lexer("[1, 2]", {
                                 {mlang::TokenType::LBRACKET, "["},
                                 {mlang::TokenType::INT,      "1"},
                                 {mlang::TokenType::COMMA,    ","},
                                 {mlang::TokenType::INT,      "2"},
                                 {mlang::TokenType::RBRACKET, "]"},
                                 {mlang::TokenType::EOFILE,   "" }
    });
}

TEST(Lexer, HashLiteral)
{
    validate_lexer(R"({"foo" : "bar"})", {
                                             {mlang::TokenType::LBRACE, "{"  },
                                             {mlang::TokenType::STRING, "foo"},
                                             {mlang::TokenType::COLON,  ":"  },
                                             {mlang::TokenType::STRING, "bar"},
                                             {mlang::TokenType::RBRACE, "}"  },
                                             {mlang::TokenType::EOFILE, ""   }
    });
}

TEST(Lexer, Complex)
{
    validate_lexer(R"(let five = 5;
let ten = 10;
let add = fn(x, y) {
x + y;
};
let result = add(five, ten);
!-/*5;
5 < 10 > 5;
return if else true false
10 == 10
10 != 9
"foobar"
"foo bar"
)",
                   {
                       {mlang::TokenType::LET,       "let"    },
                       {mlang::TokenType::IDENT,     "five"   },
                       {mlang::TokenType::ASSIGN,    "="      },
                       {mlang::TokenType::INT,       "5"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::LET,       "let"    },
                       {mlang::TokenType::IDENT,     "ten"    },
                       {mlang::TokenType::ASSIGN,    "="      },
                       {mlang::TokenType::INT,       "10"     },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::LET,       "let"    },
                       {mlang::TokenType::IDENT,     "add"    },
                       {mlang::TokenType::ASSIGN,    "="      },
                       {mlang::TokenType::FUNCTION,  "fn"     },
                       {mlang::TokenType::LPAREN,    "("      },
                       {mlang::TokenType::IDENT,     "x"      },
                       {mlang::TokenType::COMMA,     ","      },
                       {mlang::TokenType::IDENT,     "y"      },
                       {mlang::TokenType::RPAREN,    ")"      },
                       {mlang::TokenType::LBRACE,    "{"      },
                       {mlang::TokenType::IDENT,     "x"      },
                       {mlang::TokenType::PLUS,      "+"      },
                       {mlang::TokenType::IDENT,     "y"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::RBRACE,    "}"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::LET,       "let"    },
                       {mlang::TokenType::IDENT,     "result" },
                       {mlang::TokenType::ASSIGN,    "="      },
                       {mlang::TokenType::IDENT,     "add"    },
                       {mlang::TokenType::LPAREN,    "("      },
                       {mlang::TokenType::IDENT,     "five"   },
                       {mlang::TokenType::COMMA,     ","      },
                       {mlang::TokenType::IDENT,     "ten"    },
                       {mlang::TokenType::RPAREN,    ")"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::BANG,      "!"      },
                       {mlang::TokenType::MINUS,     "-"      },
                       {mlang::TokenType::SLASH,     "/"      },
                       {mlang::TokenType::ASTERISK,  "*"      },
                       {mlang::TokenType::INT,       "5"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::INT,       "5"      },
                       {mlang::TokenType::LT,        "<"      },
                       {mlang::TokenType::INT,       "10"     },
                       {mlang::TokenType::GT,        ">"      },
                       {mlang::TokenType::INT,       "5"      },
                       {mlang::TokenType::SEMICOLON, ";"      },
                       {mlang::TokenType::RETURN,    "return" },
                       {mlang::TokenType::IF,        "if"     },
                       {mlang::TokenType::ELSE,      "else"   },
                       {mlang::TokenType::TRUE,      "true"   },
                       {mlang::TokenType::FALSE,     "false"  },
                       {mlang::TokenType::INT,       "10"     },
                       {mlang::TokenType::EQ,        "=="     },
                       {mlang::TokenType::INT,       "10"     },
                       {mlang::TokenType::INT,       "10"     },
                       {mlang::TokenType::NOT_EQ,    "!="     },
                       {mlang::TokenType::INT,       "9"      },
                       {mlang::TokenType::STRING,    "foobar" },
                       {mlang::TokenType::STRING,    "foo bar"},
                       {mlang::TokenType::EOFILE,    ""       }
    });
}