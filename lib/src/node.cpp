#include <fmt/ranges.h>
#include <mlang/node.hpp>
#include <range/v3/view.hpp>

namespace rv = ranges::views;

namespace mlang
{
Node::~Node() = default;
Statement::~Statement() = default;
Expression::~Expression() = default;

auto Program::token_literal() -> std::string
{
    if (m_statements.size() > 0)
    {
        return m_statements[0]->token_literal();
    }
    return {};
}

auto Program::to_string() -> std::string
{
    std::string out;
    for (const auto& s : m_statements)
    {
        out += s->to_string();
    }
    return out;
}

auto Program::get_type() -> NodeType
{
    return NodeType::Program;
}

auto LetStatement::token_literal() -> std::string
{
    return m_token.literal;
}

auto LetStatement::to_string() -> std::string
{
    return fmt::format("{} {} = {};", token_literal(), m_name->to_string(), m_value ? m_value->to_string() : "");
}

auto LetStatement::get_type() -> NodeType
{
    return NodeType::LetStatement;
}

auto Identifier::token_literal() -> std::string
{
    return m_token.literal;
}

auto Identifier::to_string() -> std::string
{
    return m_value;
}

auto Identifier::get_type() -> NodeType
{
    return NodeType::Identifier;
}

auto ReturnStatement::token_literal() -> std::string
{
    return m_token.literal;
}

auto ReturnStatement::to_string() -> std::string
{
    return fmt::format("{} = {};", token_literal(), m_return_value ? m_return_value->to_string() : "");
}

auto ReturnStatement::get_type() -> NodeType
{
    return NodeType::ReturnStatement;
}

auto BlockStatement::token_literal() -> std::string
{
    return m_token.literal;
}

auto BlockStatement::to_string() -> std::string
{
    using value_t = typename decltype(m_statements)::value_type;
    return fmt::format("{}", fmt::join(m_statements | rv::transform([](const value_t& ptr)
                                                                    { return ptr->to_string(); }),
                                       ""));
}

auto BlockStatement::get_type() -> NodeType
{
    return NodeType::BlockStatement;
}

auto ExpressionStatement::token_literal() -> std::string
{
    return m_token.literal;
}

auto ExpressionStatement::to_string() -> std::string
{
    if (!m_expression)
    {
        return {};
    }
    return m_expression->to_string();
}

auto ExpressionStatement::get_type() -> NodeType
{
    return NodeType::ExpressionStatement;
}

IntegerLiteral::IntegerLiteral(std::int64_t val)
    : m_value{val}
{
}

auto IntegerLiteral::token_literal() -> std::string
{
    return m_token.literal;
}

auto IntegerLiteral::to_string() -> std::string
{
    return m_token.literal;
}

auto IntegerLiteral::get_type() -> NodeType
{
    return NodeType::IntegerLiteral;
}

StringLiteral::StringLiteral(std::string_view str)
    : m_value(str)
{
}

auto StringLiteral::token_literal() -> std::string
{
    return m_token.literal;
}

auto StringLiteral::to_string() -> std::string
{
    return fmt::format("\"{}\"", m_value);
}

auto StringLiteral::get_type() -> NodeType
{
    return NodeType::StringLiteral;
}

auto BooleanLiteral::token_literal() -> std::string
{
    return m_token.literal;
}

auto BooleanLiteral::to_string() -> std::string
{
    return m_token.literal;
}

auto BooleanLiteral::get_type() -> NodeType
{
    return NodeType::BooleanLiteral;
}

auto IfExpression::token_literal() -> std::string
{
    return m_token.literal;
}

auto IfExpression::to_string() -> std::string
{
    auto if_str = fmt::format("if {} {{{}}}", m_condition->to_string(), m_consequence->to_string());
    if (m_alternative)
    {
        if_str += fmt::format(" else {{{}}}", m_alternative->to_string());
    }
    return if_str;
}

auto IfExpression::get_type() -> NodeType
{
    return NodeType::IfExpression;
}

auto PrefixExpression::token_literal() -> std::string
{
    return m_token.literal;
}

auto PrefixExpression::to_string() -> std::string
{
    return fmt::format("({}{})", m_operator, m_right->to_string());
}

auto PrefixExpression::get_type() -> NodeType
{
    return NodeType::PrefixExpression;
}

auto InfixExpression::token_literal() -> std::string
{
    return m_token.literal;
}

auto InfixExpression::to_string() -> std::string
{
    return fmt::format("({} {} {})", m_left->to_string(), m_operator, m_right->to_string());
}

auto InfixExpression::get_type() -> NodeType
{
    return NodeType::InfixExpression;
}

auto FnLiteral::token_literal() -> std::string
{
    return m_token.literal;
}

auto FnLiteral::to_string() -> std::string
{
    using value_t = typename decltype(m_parameters)::value_type;
    return fmt::format("fn({}){{{}}}", fmt::join(m_parameters | rv::transform([](const value_t& ptr)
                                                                              { return ptr->to_string(); }),
                                                 ", "),
                       m_body->to_string());
}

auto FnLiteral::get_type() -> NodeType
{
    return NodeType::FnLiteral;
}

auto CallExpression::token_literal() -> std::string
{
    return m_token.literal;
}

auto CallExpression::to_string() -> std::string
{
    using value_t = typename decltype(m_arguments)::value_type;
    return fmt::format("{}({})", m_function->to_string(), fmt::join(m_arguments | rv::transform([](const value_t& ptr)
                                                                                                { return ptr->to_string(); }),
                                                                    ", "));
}

auto CallExpression::get_type() -> NodeType
{
    return NodeType::CallExpression;
}

auto ArrayLiteral::token_literal() -> std::string
{
    return m_token.literal;
}

auto ArrayLiteral::to_string() -> std::string
{
    using value_t = typename decltype(m_expressions)::value_type;
    return fmt::format("[{}]", fmt::join(m_expressions | rv::transform([](const value_t& ptr)
                                                                       { return ptr->to_string(); }),
                                         ", "));
}

auto ArrayLiteral::get_type() -> NodeType
{
    return NodeType::ArrayLiteral;
}

auto IndexExpression::token_literal() -> std::string
{
    return m_token.literal;
}

auto IndexExpression::to_string() -> std::string
{
    return fmt::format("({}[{}])", m_left->to_string(), m_index->to_string());
}

auto IndexExpression::get_type() -> NodeType
{
    return NodeType::IndexExpression;
}

auto WhileStatement::token_literal() -> std::string
{
    return m_token.literal;
}

auto WhileStatement::to_string() -> std::string
{
    return fmt::format("while({}){{{}}}", m_condition->to_string(), m_loop_body->to_string());
}

auto WhileStatement::get_type() -> NodeType
{
    return NodeType::WhileStatement;
}

auto HashLiteral::to_string() -> std::string
{
    using value_t = typename decltype(m_pairs)::value_type;
    return fmt::format("{{{}}}", fmt::join(m_pairs | rv::transform([](const value_t& pr)
                                                                   { return pr.first->to_string() + ":" + pr.second->to_string(); }),
                                           ", "));
}

auto HashLiteral::get_type() -> NodeType
{
    return NodeType::HashLiteral;
}

auto HashLiteral::token_literal() -> std::string
{
    return m_token.literal;
}
}  // namespace mlang
