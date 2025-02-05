#pragma once

#include <functional>
#include <memory>
#include <mlang/lexer.hpp>
#include <mlang/node.hpp>
#include <mlang/token.hpp>
#include <unordered_map>

namespace mlang
{
enum class Precedence : std::uint8_t
{
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL,
    INDEX,
};

class Parser
{
    using PrefixParseFn = std::function<std::unique_ptr<Expression>()>;
    using InfixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>&&)>;

public:
    Parser(std::unique_ptr<ILexer>&& lexer);
    auto get_errors() const -> const std::vector<std::string>&;
    auto parse_program() -> std::unique_ptr<Program>;

private:
    auto parse_statement() -> std::unique_ptr<Statement>;
    auto parse_let_statement() -> std::unique_ptr<LetStatement>;
    auto parse_return_statement() -> std::unique_ptr<ReturnStatement>;
    auto parse_expression_statement() -> std::unique_ptr<ExpressionStatement>;
    auto parse_block_statement() -> std::unique_ptr<BlockStatement>;
    auto parse_while_statement() -> std::unique_ptr<WhileStatement>;

    auto parse_expression(Precedence precedence) -> std::unique_ptr<Expression>;
    auto parse_prefix_expression() -> std::unique_ptr<PrefixExpression>;
    auto parse_grouped_expression() -> std::unique_ptr<Expression>;
    auto parse_if_expression() -> std::unique_ptr<IfExpression>;
    auto parse_infix_expression(std::unique_ptr<Expression>&& left) -> std::unique_ptr<InfixExpression>;
    auto parse_call_expression(std::unique_ptr<Expression>&& function) -> std::unique_ptr<CallExpression>;
    auto parse_index_expression(std::unique_ptr<Expression>&& left) -> std::unique_ptr<IndexExpression>;

    auto parse_identifier() -> std::unique_ptr<Identifier>;
    auto parse_int() -> std::unique_ptr<IntegerLiteral>;
    auto parse_bool() -> std::unique_ptr<BooleanLiteral>;
    auto parse_fn() -> std::unique_ptr<FnLiteral>;
    auto parse_string() -> std::unique_ptr<StringLiteral>;
    auto parse_array() -> std::unique_ptr<ArrayLiteral>;
    auto parse_hash() -> std::unique_ptr<HashLiteral>;
    auto parse_call_arguments() -> std::vector<std::unique_ptr<Expression>>;
    auto parse_fn_parameters() -> std::vector<std::shared_ptr<Identifier>>;

    auto expect_peek(TokenType type) -> bool;
    auto peek_token() -> Token;
    void next_token();
    void peek_error(TokenType unwanted_token);
    auto get_precedence(TokenType type) -> Precedence;

private:
    std::unique_ptr<ILexer> m_lexer;
    Token m_curr;
    Token m_next;
    std::unordered_map<TokenType, PrefixParseFn> m_prefix_parse_fns;
    std::unordered_map<TokenType, InfixParseFn> m_infix_parse_fns;
    std::vector<std::string> m_errors;
};
}  // namespace mlang
