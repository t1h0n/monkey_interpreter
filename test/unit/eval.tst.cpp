

#include "mlang/eval.hpp"

#include "mlang/object.hpp"
#include "mlang/parser.hpp"
#include "mlang/token.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utility>

using namespace ::testing;

namespace
{
template <typename OutObj, typename Out, typename = std::enable_if_t<std::is_convertible_v<Out, decltype(OutObj::m_value)>>>
void test_generic_expr(const std::string& input, const Out& expected, mlang::ObjectType out_type)
{
    mlang::Parser p(std::make_unique<mlang::Lexer>(input));
    auto program = p.parse_program();
    const auto& errors = p.get_errors();
    EXPECT_THAT(errors, IsEmpty()) << input;
    ASSERT_THAT(program, NotNull()) << input;

    auto env = std::make_shared<mlang::Context>();
    auto res = eval(program.get(), env);
    ASSERT_THAT(res, NotNull()) << input;
    ASSERT_EQ(res->get_type(), out_type) << res->inspect();
    auto* obj = static_cast<OutObj*>(res.get());
    EXPECT_EQ(obj->m_value, expected) << input;
}

void test_generic_expr_with_nil(const std::string& input)
{
    mlang::Parser p(std::make_unique<mlang::Lexer>(input));
    auto program = p.parse_program();
    const auto& errors = p.get_errors();
    EXPECT_THAT(errors, IsEmpty()) << input;
    ASSERT_THAT(program, NotNull()) << input;

    auto env = std::make_shared<mlang::Context>();
    auto res = eval(program.get(), env);
    ASSERT_THAT(res, NotNull()) << input;
    EXPECT_EQ(mlang::detail::NIL, res) << input;
}

void test_error(const std::string& input, const std::string& expected_err)
{
    mlang::Parser p(std::make_unique<mlang::Lexer>(input));
    auto program = p.parse_program();
    const auto& errors = p.get_errors();
    EXPECT_THAT(errors, IsEmpty()) << input << "\n"
                                   << expected_err;
    ASSERT_THAT(program, NotNull()) << input << "\n"
                                    << expected_err;

    auto env = std::make_shared<mlang::Context>();
    auto res = eval(program.get(), env);
    ASSERT_THAT(res, NotNull()) << input << "\n"
                                << expected_err;
    ASSERT_EQ(res->get_type(), mlang::ObjectType::ERROR) << input << "\n"
                                                         << expected_err;
    auto* obj = static_cast<mlang::ErrorObj*>(res.get());
    EXPECT_EQ(obj->m_what, expected_err) << input;
}
}  // namespace

TEST(eval, IntegerObj)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"5",  5 },
             {"10", 10}
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, BooleanObj)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, bool>>;
    for (const auto& [input, expected] : arg_list_t{
             {"true",  true },
             {"false", false},
    })
    {
        test_generic_expr<mlang::BooleanObj>(input, expected, mlang::ObjectType::BOOLEAN);
    }
}

TEST(eval, PrefixOperatorBool)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, bool>>;
    for (const auto& [input, expected] : arg_list_t{
             {"!true",   false},
             {"!false",  true },
             {"!5",      false},
             {"!!true",  true },
             {"!!false", false},
             {"!!5",     true },
    })
    {
        test_generic_expr<mlang::BooleanObj>(input, expected, mlang::ObjectType::BOOLEAN);
    }
}

TEST(eval, PrefixOperatorInt)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"5",   5  },
             {"10",  10 },
             {"-5",  -5 },
             {"-10", -10},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, EvalIntegerExpression)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{

             {"5",                               5  },
             {"10",                              10 },
             {"-5",                              -5 },
             {"-10",                             -10},
             {"5 + 5 + 5 + 5 - 10",              10 },
             {"2 * 2 * 2 * 2 * 2",               32 },
             {"-50 + 100 + -50",                 0  },
             {"5 * 2 + 10",                      20 },
             {"5 + 2 * 10",                      25 },
             {"20 + 2 * -10",                    0  },
             {"50 / 2 * 2 + 10",                 60 },
             {"2 * (5 + 10)",                    30 },
             {"3 * 3 * 3 + 10",                  37 },
             {"3 * (3 * 3) + 10",                37 },
             {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50 },
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, EvalBooleanExpression)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, bool>>;
    for (const auto& [input, expected] : arg_list_t{

             {"true == true",     true },
             {"false == false",   true },
             {"true == false",    false},
             {"true != false",    true },
             {"false != true",    true },
             {"(1 < 2) == true",  true },
             {"(1 < 2) == false", false},
             {"(1 > 2) == true",  false},
             {"(1 > 2) == false", true },
             {"true",             true },
             {"false",            false},
             {"1 < 2",            true },
             {"1 > 2",            false},
             {"1 < 1",            false},
             {"1 > 1",            false},
             {"1 == 1",           true },
             {"1 != 1",           false},
             {"1 == 2",           false},
             {"1 != 2",           true }
    })
    {
        test_generic_expr<mlang::BooleanObj>(input, expected, mlang::ObjectType::BOOLEAN);
    }
}

TEST(eval, IfElseExpressions)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"if (true) { 10 }",                                10},
             {"if (1) { 10 }",                                   10},
             {"if (1 < 2) { 10 }",                               10},
             {"if (1 > 2) { 10 } else { 20 }",                   20},
             {"if (1 < 2) { 10 } else { 20 }",                   10},
             {"if (10 > 1) {if (10 > 1) {return 10;}return 1;}", 10},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
    for (const auto& input : {"if (1 > 2) { 10 }", "if (false) { 10 }"})
    {
        test_generic_expr_with_nil(input);
    }
}

TEST(eval, ReturnStatements)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"return 10;",          10},
             {"return 10; 9;",       10},
             {"return 2 * 5; 9;",    10},
             {"9; return 2 * 5; 9;", 10},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, ErrorMessage)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::string>>;
    for (const auto& [input, err] : arg_list_t{
             {"5 + true;",                                                      "type mismatch: INTEGER + BOOLEAN"   },
             {"5 + true; 5;",                                                   "type mismatch: INTEGER + BOOLEAN"   },
             {"-true",                                                          "unknown operator: -BOOLEAN"         },
             {"true + false;",                                                  "unknown operator: BOOLEAN + BOOLEAN"},
             {"5; true + false; 5",                                             "unknown operator: BOOLEAN + BOOLEAN"},
             {"if (10 > 1) { true + false; }",                                  "unknown operator: BOOLEAN + BOOLEAN"},
             {"if (10 > 1) { if (10 > 1) { return true + false; } return 1; }", "unknown operator: BOOLEAN + BOOLEAN"},
             {"foobar",                                                         "identifier not found: foobar"       },
    })
    {
        test_error(input, err);
    }
}

TEST(eval, LetStatement)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, val] : arg_list_t{
             {"let a = 5; a;",                               5 },
             {"let a = 5 * 5; a;",                           25},
             {"let a = 5; let b = a; b;",                    5 },
             {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, val, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, FunctionObj)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, val] : arg_list_t{
             {"let a = 5; a;",                               5 },
             {"let a = 5 * 5; a;",                           25},
             {"let a = 5; let b = a; b;",                    5 },
             {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, val, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, FnCall)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"let identity = fn(x) { x; }; identity(5);",             5 },
             {"let identity = fn(x) { return x; }; identity(5);",      5 },
             {"let double = fn(x) { x * 2; }; double(5);",             10},
             {"let add = fn(x, y) { x + y; }; add(5, 5);",             10},
             {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
             {"fn(x) { x; }(5)",                                       5 },
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, FnCallClosure)
{
    const std::string input = R"(
        let newAdder = fn(x) {
            fn(y) { x + y };
        };
        let addTwo = newAdder(2);
        addTwo(2);
    )";
    test_generic_expr<mlang::IntegerObj>(input, 4, mlang::ObjectType::INTEGER);
}

TEST(eval, StringObj)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::string>>;
    for (const auto& [input, expected] : arg_list_t{
             {"\"eval\"",                         "eval"          },
             {"\"dis\"",                          "dis"           },
             {"\"nuts\"",                         "nuts"          },
             {"\"parse\" + \" me\" + \" daddy\"", "parse me daddy"},
    })
    {
        test_generic_expr<mlang::StringObj>(input, expected, mlang::ObjectType::STRING);
    }
}

TEST(eval, BuiltInFns)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"len(\"dis\")",       3},
             {"len(\"\")",          0},
             {"len([1,2,3,4,5])",   5},
             {"last([1,2,3,4,5])",  5},
             {"first([1,2,3,4,5])", 1},
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}

TEST(eval, ArrayLiteral)
{
    const std::string input = "[1, 2 * 2, 3 + 3]";
    mlang::Parser p(std::make_unique<mlang::Lexer>(input));
    auto program = p.parse_program();
    const auto& errors = p.get_errors();
    EXPECT_THAT(errors, IsEmpty()) << input;
    ASSERT_THAT(program, NotNull()) << input;

    auto env = std::make_shared<mlang::Context>();
    auto res = eval(program.get(), env);
    ASSERT_THAT(res, NotNull()) << input;
    ASSERT_EQ(res->get_type(), mlang::ObjectType::ARRAY) << res->inspect();
    auto* obj = static_cast<mlang::ArrayObj*>(res.get());
    ASSERT_EQ(obj->m_values.size(), 3);
    EXPECT_EQ(dynamic_cast<mlang::IntegerObj&>(*obj->m_values[0]).m_value, 1) << input;
    EXPECT_EQ(dynamic_cast<mlang::IntegerObj&>(*obj->m_values[1]).m_value, 4) << input;
    EXPECT_EQ(dynamic_cast<mlang::IntegerObj&>(*obj->m_values[2]).m_value, 6) << input;
}

TEST(eval, HashLiteral)
{
    const std::string input = R"(
    let two = "two";
    {
        "one": 10 - 9,
        two: 1 + 1,
        "thr" + "ee": 6 / 2,
        4: 4,
        true: 5,
        false: 6
    };
    )";
    mlang::Parser p(std::make_unique<mlang::Lexer>(input));
    auto program = p.parse_program();
    const auto& errors = p.get_errors();
    EXPECT_THAT(errors, IsEmpty()) << input;
    ASSERT_THAT(program, NotNull()) << input;

    auto env = std::make_shared<mlang::Context>();
    auto res = eval(program.get(), env);
    ASSERT_THAT(res, NotNull()) << input;
    ASSERT_EQ(res->get_type(), mlang::ObjectType::HASH) << res->inspect();
    auto* obj = static_cast<mlang::HashObj*>(res.get());
    ASSERT_EQ(obj->m_pairs.size(), 6);
}

TEST(eval, IndexExpression)
{
    using arg_list_t = std::initializer_list<std::tuple<std::string, std::int64_t>>;
    for (const auto& [input, expected] : arg_list_t{
             {"[1, 2, 3][0]",                                                   1},
             {"[1, 2, 3][1]",                                                   2},
             {"[1, 2, 3][2]",                                                   3},
             {"let i = 0; [1][i];",                                             1},
             {"[1, 2, 3][1 + 1];",                                              3},
             {"let myArray = [1, 2, 3]; myArray[2];",                           3},
             {"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6},
             {"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]",        2},
             {"{true: 5}[true]",                                                5},
             {"{false: 5}[false]",                                              5},
             {"{\"foo\": 5}[\"foo\"]",                                          5},
             {"let key = \"foo\"; {\"foo\": 5}[key]",                           5},
             {"{5: 5}[5]",                                                      5}
    })
    {
        test_generic_expr<mlang::IntegerObj>(input, expected, mlang::ObjectType::INTEGER);
    }
}
