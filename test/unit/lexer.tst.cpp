#include "lexer.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <vector>

using namespace ::testing;

void validate_lexer(std::string_view input, std::vector<Token> expected_tokens)
{
    Lexer lexer(input);
    std::vector<Token> tokens;
    for (auto token = lexer.next_token(); token.type != TokenType::EOFILE; token = lexer.next_token())
    {
        tokens.push_back(token);
    }
    tokens.push_back({TokenType::EOFILE, ""});
    EXPECT_THAT(tokens, ContainerEq(expected_tokens));
}

TEST(Lexer, Simple)
{
    validate_lexer("=+(){},;", {
                                   {TokenType::ASSIGN,    "="},
                                   {TokenType::PLUS,      "+"},
                                   {TokenType::LPAREN,    "("},
                                   {TokenType::RPAREN,    ")"},
                                   {TokenType::LBRACE,    "{"},
                                   {TokenType::RBRACE,    "}"},
                                   {TokenType::COMMA,     ","},
                                   {TokenType::SEMICOLON, ";"},
                                   {TokenType::EOFILE,    "" }
    });
}

TEST(Lexer, String)
{
    validate_lexer("\"foobar\"", {
                                     {TokenType::STRING, "foobar"},
                                     {TokenType::EOFILE, ""      }
    });
}

TEST(Lexer, While)
{
    validate_lexer("while(true){}", {
                                        {TokenType::WHILE,  "while"},
                                        {TokenType::LPAREN, "("    },
                                        {TokenType::TRUE,   "true" },
                                        {TokenType::RPAREN, ")"    },
                                        {TokenType::LBRACE, "{"    },
                                        {TokenType::RBRACE, "}"    },
                                        {TokenType::EOFILE, ""     }
    });
}

TEST(Lexer, Array)
{
    validate_lexer("[1, 2]", {
                                 {TokenType::LBRACKET, "["},
                                 {TokenType::INT,      "1"},
                                 {TokenType::COMMA,    ","},
                                 {TokenType::INT,      "2"},
                                 {TokenType::RBRACKET, "]"},
                                 {TokenType::EOFILE,   "" }
    });
}

TEST(Lexer, HashLiteral)
{
    validate_lexer(R"({"foo" : "bar"})", {
                                             {TokenType::LBRACE, "{"  },
                                             {TokenType::STRING, "foo"},
                                             {TokenType::COLON,  ":"  },
                                             {TokenType::STRING, "bar"},
                                             {TokenType::RBRACE, "}"  },
                                             {TokenType::EOFILE, ""   }
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
                       {TokenType::LET,       "let"    },
                       {TokenType::IDENT,     "five"   },
                       {TokenType::ASSIGN,    "="      },
                       {TokenType::INT,       "5"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::LET,       "let"    },
                       {TokenType::IDENT,     "ten"    },
                       {TokenType::ASSIGN,    "="      },
                       {TokenType::INT,       "10"     },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::LET,       "let"    },
                       {TokenType::IDENT,     "add"    },
                       {TokenType::ASSIGN,    "="      },
                       {TokenType::FUNCTION,  "fn"     },
                       {TokenType::LPAREN,    "("      },
                       {TokenType::IDENT,     "x"      },
                       {TokenType::COMMA,     ","      },
                       {TokenType::IDENT,     "y"      },
                       {TokenType::RPAREN,    ")"      },
                       {TokenType::LBRACE,    "{"      },
                       {TokenType::IDENT,     "x"      },
                       {TokenType::PLUS,      "+"      },
                       {TokenType::IDENT,     "y"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::RBRACE,    "}"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::LET,       "let"    },
                       {TokenType::IDENT,     "result" },
                       {TokenType::ASSIGN,    "="      },
                       {TokenType::IDENT,     "add"    },
                       {TokenType::LPAREN,    "("      },
                       {TokenType::IDENT,     "five"   },
                       {TokenType::COMMA,     ","      },
                       {TokenType::IDENT,     "ten"    },
                       {TokenType::RPAREN,    ")"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::BANG,      "!"      },
                       {TokenType::MINUS,     "-"      },
                       {TokenType::SLASH,     "/"      },
                       {TokenType::ASTERISK,  "*"      },
                       {TokenType::INT,       "5"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::INT,       "5"      },
                       {TokenType::LT,        "<"      },
                       {TokenType::INT,       "10"     },
                       {TokenType::GT,        ">"      },
                       {TokenType::INT,       "5"      },
                       {TokenType::SEMICOLON, ";"      },
                       {TokenType::RETURN,    "return" },
                       {TokenType::IF,        "if"     },
                       {TokenType::ELSE,      "else"   },
                       {TokenType::TRUE,      "true"   },
                       {TokenType::FALSE,     "false"  },
                       {TokenType::INT,       "10"     },
                       {TokenType::EQ,        "=="     },
                       {TokenType::INT,       "10"     },
                       {TokenType::INT,       "10"     },
                       {TokenType::NOT_EQ,    "!="     },
                       {TokenType::INT,       "9"      },
                       {TokenType::STRING,    "foobar" },
                       {TokenType::STRING,    "foo bar"},
                       {TokenType::EOFILE,    ""       }
    });
}