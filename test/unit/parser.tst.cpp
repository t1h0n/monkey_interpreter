
#include "parser.hpp"

#include "fmt/format.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lexer.hpp"
#include "node.hpp"
#include "range/v3/all.hpp"

#include <algorithm>
#include <vector>

using namespace ::testing;
using namespace ::std::literals;
namespace rv = ::ranges::views;

namespace
{
void test_let_statement(const std::string& input, const std::string& ident, const std::string& value)
{
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    std::vector<std::string> errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    auto& statement = *program->m_statements.front();
    ASSERT_EQ(statement.token_literal(), "let");
    auto& let_statement = dynamic_cast<LetStatement&>(statement);
    EXPECT_EQ(let_statement.m_name->token_literal(), ident);
    EXPECT_EQ(let_statement.m_value->token_literal(), value);
}
}  // namespace

TEST(Parser, LetStatement)
{
    using validation_t = std::initializer_list<std::tuple<std::string, std::string, std::string>>;
    const auto expected = validation_t{
        {"let x = 5;",      "x",      "5"   },
        {"let y = true;",   "y",      "true"},
        {"let foobar = y;", "foobar", "y"   },
    };
    for (auto&& [statement, exp_ident, exp_value] : expected)
    {
        test_let_statement(statement, exp_ident, exp_value);
    }
}

TEST(Parser, ReturnStatement)
{
    const std::string input = R"(
    return 5;
    return 10;
    return 585;)";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    EXPECT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 3);
    for (auto&& statement : program->m_statements)
    {
        ASSERT_EQ(statement->token_literal(), "return");
        auto& return_statement = dynamic_cast<ReturnStatement&>(*statement);
    }
}

TEST(Parser, Identifier)
{
    const std::string input = "parse_me_daddy;";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<Identifier&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(node_val.m_value, "parse_me_daddy");
    EXPECT_EQ("parse_me_daddy", program->to_string());
}

TEST(Parser, Int)
{
    const std::string input = "5;";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<IntegerLiteral&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(node_val.m_value, 5);
    EXPECT_EQ("5", program->to_string());
}

namespace
{
void test_prefix_expr(const std::string& input, const std::string& op, std::int64_t val)
{
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<PrefixExpression&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(node_val.m_operator, op);
    EXPECT_EQ(node_val.m_token.literal, op);
    EXPECT_EQ("("s + input + ")"s, program->to_string());
    EXPECT_EQ(dynamic_cast<IntegerLiteral&>(*node_val.m_right).m_value, val);
}
}  // namespace

TEST(Parser, Prefix)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::string, std::int64_t>>;
    for (const auto& [input, op, val] : arg_list_t{
             {"!3", "!", 3},
             {"-3", "-", 3}
    })
    {
        test_prefix_expr(input, op, val);
    }
}

namespace
{
void test_infix_expr(const std::string& input, std::int64_t l, const std::string& op, std::int64_t r)
{
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<InfixExpression&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(node_val.m_operator, op);
    EXPECT_EQ(node_val.m_token.literal, op);
    EXPECT_EQ("("s + input.substr(0, std::size(input) - 1) + ")"s, program->to_string());
    EXPECT_EQ(dynamic_cast<IntegerLiteral&>(*node_val.m_right).m_value, l);
    EXPECT_EQ(dynamic_cast<IntegerLiteral&>(*node_val.m_left).m_value, r);
}
}  // namespace

TEST(Parser, Infix)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t, std::string, std::int64_t>>;
    for (const auto& [input, l, op, r] : arg_list_t{
             {"5 + 5;",  5, "+",  5},
             {"5 - 5;",  5, "-",  5},
             {"5 * 5;",  5, "*",  5},
             {"5 / 5;",  5, "/",  5},
             {"5 > 5;",  5, ">",  5},
             {"5 < 5;",  5, "<",  5},
             {"5 == 5;", 5, "==", 5},
             {"5 != 5;", 5, "!=", 5},
    })
    {
        test_infix_expr(input, l, op, r);
    }
}

TEST(Parser, OperatorPrecedence)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::string>>;
    for (const auto& [input, output] : arg_list_t{
             {"-a * b",                                    "((-a) * b)"                                     },
             {"!-a",                                       "(!(-a))"                                        },
             {"a + b + c",                                 "((a + b) + c)"                                  },
             {"a + b - c",                                 "((a + b) - c)"                                  },
             {"a * b * c",                                 "((a * b) * c)"                                  },
             {"a * b / c",                                 "((a * b) / c)"                                  },
             {"a + b / c",                                 "(a + (b / c))"                                  },
             {"a + b * c + d / e - f",                     "(((a + (b * c)) + (d / e)) - f)"                },
             {"3 + 4; -5 * 5",                             "(3 + 4)((-5) * 5)"                              },
             {"5 > 4 == 3 < 4",                            "((5 > 4) == (3 < 4))"                           },
             {"5 < 4 != 3 > 4",                            "((5 < 4) != (3 > 4))"                           },
             {"3 + 4 * 5 == 3 * 1 + 4 * 5",                "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"         },
             {"3 + 4 * 5 == 3 * 1 + 4 * 5",                "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"         },
             {"(5 + 5) * 2",                               "((5 + 5) * 2)"                                  },
             {"2 / (5 + 5)",                               "(2 / (5 + 5))"                                  },
             {"-(5 + 5)",                                  "(-(5 + 5))"                                     },
             {"!(true == true)",                           "(!(true == true))"                              },
             {"a + add(b * c) + d",                        "((a + add((b * c))) + d)"                       },
             {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
             {"add(a + b + c * d / f + g)",                "add((((a + b) + ((c * d) / f)) + g))"           },
             {"a * [1, 2, 3, 4][b * c] * d",               "((a * ([1, 2, 3, 4][(b * c)])) * d)"            },
             {"add(a * b[2], b[1], 2 * [1, 2][1])",        "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"   },
    })
    {
        Parser parser(std::make_unique<Lexer>(input));
        const auto program = parser.parse_program();
        EXPECT_EQ(program->to_string(), output);
    }
}

TEST(Parser, BooleanLiteral)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::string>>;
    for (const auto& [input, output] : arg_list_t{
             {"false", "false"},
             {"true",  "true" },
    })
    {
        Parser parser(std::make_unique<Lexer>(input));
        const auto program = parser.parse_program();
        const auto& errors = parser.get_errors();
        EXPECT_THAT(errors, IsEmpty());
        ASSERT_THAT(program, NotNull());
        EXPECT_EQ(program->to_string(), output);
        EXPECT_EQ(program->m_statements.size(), 1);
        const auto& node_val = dynamic_cast<BooleanLiteral&>(
            *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
        EXPECT_EQ(node_val.m_token.literal, output);
    }
}

TEST(Parser, IfStatement)
{
    const std::string input = "if (x < y) {x} else {y}";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<IfExpression&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(input, program->to_string());
}

TEST(Parser, FunctionLiteral)
{
    const std::string input = "fn(x, y) { x + y; }";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<FnLiteral&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ("fn(x, y){(x + y)}", program->to_string());
}

TEST(Parser, CallExpression)
{
    const std::string input = "add(1, 2 * 3, 4 + 5);";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<CallExpression&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ("add(1, (2 * 3), (4 + 5))", program->to_string());
}

TEST(Parser, StringLiteral)
{
    const std::string input = "\"parse me daddy\"";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<StringLiteral&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ(input, program->to_string());
}

TEST(Parser, ArrayLiteral)
{
    const std::string input = "[parse, me, daddy]";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    const auto& node_val = dynamic_cast<ArrayLiteral&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    EXPECT_EQ("[parse, me, daddy]", program->to_string());
}

TEST(Parser, HashLiteral)
{
    const std::string input = R"({"one":1})";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    EXPECT_EQ(input, program->to_string());
    const auto& node_val = dynamic_cast<HashLiteral&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);
    const auto& hash1 = node_val.m_pairs.at(0);
    const auto hash1_key = dynamic_cast<StringLiteral&>(*hash1.first);
    const auto hash1_val = dynamic_cast<IntegerLiteral&>(*hash1.second);
    EXPECT_EQ(hash1_key.m_value, "one");
    EXPECT_EQ(hash1_val.m_value, 1);
}

TEST(Parser, IndexExpression)
{
    const std::string input = "arr[2 + 1]";
    Parser parser(std::make_unique<Lexer>(input));
    const auto program = parser.parse_program();
    const auto& errors = parser.get_errors();
    EXPECT_THAT(errors, IsEmpty());
    ASSERT_THAT(program, NotNull());
    EXPECT_EQ(program->m_statements.size(), 1);
    ASSERT_EQ("(arr[(2 + 1)])", program->to_string()) << program->to_string();

    const auto& node_val = dynamic_cast<IndexExpression&>(
        *dynamic_cast<ExpressionStatement&>(*program->m_statements[0]).m_expression);

    const auto& left = dynamic_cast<Identifier&>(*node_val.m_left);
    const auto& right = dynamic_cast<InfixExpression&>(*node_val.m_index);
    EXPECT_EQ(left.m_value, "arr");
    EXPECT_EQ(right.m_operator, "+");
    const auto& left_int = dynamic_cast<IntegerLiteral&>(*right.m_left);
    EXPECT_EQ(left_int.m_value, 2);
    const auto& right_int = dynamic_cast<IntegerLiteral&>(*right.m_right);
    EXPECT_EQ(right_int.m_value, 1);
}