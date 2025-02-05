#pragma once
#include <cstdint>
#include <memory>
#include <mlang/token.hpp>
#include <string>
#include <vector>

namespace mlang
{
enum class NodeType : std::uint8_t
{
    Program,
    Identifier,
    LetStatement,
    ReturnStatement,
    BlockStatement,
    ExpressionStatement,
    IntegerLiteral,
    BooleanLiteral,
    IfExpression,
    PrefixExpression,
    InfixExpression,
    FnLiteral,
    CallExpression,
    StringLiteral,
    ArrayLiteral,
    IndexExpression,
    HashLiteral,
    WhileStatement,
};

class Node
{
public:
    virtual auto token_literal() -> std::string = 0;
    virtual auto to_string() -> std::string = 0;
    virtual auto get_type() -> NodeType = 0;
    virtual ~Node() = 0;
};

class Statement : public Node
{
public:
    virtual ~Statement() = 0;
};

class Expression : public Node
{
public:
    virtual ~Expression() = 0;
};

class Program : public Node
{
public:
    ~Program() = default;

    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    std::vector<std::unique_ptr<Statement>> m_statements;
};

class Identifier : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::string m_value;
};

class LetStatement : public Statement
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Identifier> m_name;
    std::unique_ptr<Expression> m_value;
};

class ReturnStatement : public Statement
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_return_value;
};

class BlockStatement : public Statement
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::vector<std::unique_ptr<Statement>> m_statements;
};

class ExpressionStatement : public Statement
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_expression;
};

class IntegerLiteral : public Expression
{
public:
    IntegerLiteral() = default;
    IntegerLiteral(std::int64_t val);
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::int64_t m_value;
};

class StringLiteral : public Expression
{
public:
    StringLiteral() = default;
    StringLiteral(std::string_view str);
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::string m_value;
};

class BooleanLiteral : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    bool m_value;
};

class IfExpression : public Expression
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<BlockStatement> m_consequence;
    std::unique_ptr<BlockStatement> m_alternative;
};

class WhileStatement : public Statement
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<BlockStatement> m_loop_body;
};

class PrefixExpression : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::string m_operator;
    std::unique_ptr<Expression> m_right;
};

class InfixExpression : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_left;
    std::string m_operator;
    std::unique_ptr<Expression> m_right;
};

class FnLiteral : public Expression
{
public:
    auto token_literal() -> std::string override;
    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::vector<std::shared_ptr<Identifier>> m_parameters;
    std::shared_ptr<BlockStatement> m_body;
};

class ArrayLiteral : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::vector<std::unique_ptr<Expression>> m_expressions;
};

class IndexExpression : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_index;
};

class CallExpression : public Expression
{
public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    std::unique_ptr<Expression> m_function;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

class HashLiteral : public Expression
{
public:
    using ExprHashMap = std::vector<std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>>>;

public:
    auto token_literal() -> std::string override;

    auto to_string() -> std::string override;
    auto get_type() -> NodeType override;

public:
    Token m_token;
    ExprHashMap m_pairs;
};
}  // namespace mlang
