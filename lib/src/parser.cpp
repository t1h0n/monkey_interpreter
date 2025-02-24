#include <charconv>
#include <mlang/parser.hpp>
#include <mlang/raii_wrapper.hpp>
#include <utility>

#if defined(ENABLE_PARSE_TRACING)
#define TRACE() const auto MACRO_trace_var_tmp_ = trace(__func__)

namespace
{
inline auto trace(std::string_view s)
{
    static std::int32_t counter = 0;
    fmt::println("{}BEGIN {}", std::string(counter * 4, ' '), s);
    ++counter;
    return mlang::RaiiWrapper([s]()
                              {
                        --counter;
                        fmt::println("{}END {}", std::string(counter * 4, ' '), s); });
}
}  // namespace
#else
#define TRACE() void(0)
#endif

namespace mlang
{
Parser::Parser(std::unique_ptr<ILexer>&& lexer)
    : m_lexer(std::move(lexer))
{
    assert(m_lexer);
    next_token();
    next_token();

    m_prefix_parse_fns.emplace(TokenType::IDENT, std::bind(&Parser::parse_identifier, this));
    m_prefix_parse_fns.emplace(TokenType::INT, std::bind(&Parser::parse_int, this));
    m_prefix_parse_fns.emplace(TokenType::TRUE, std::bind(&Parser::parse_bool, this));
    m_prefix_parse_fns.emplace(TokenType::FALSE, std::bind(&Parser::parse_bool, this));
    m_prefix_parse_fns.emplace(TokenType::BANG, std::bind(&Parser::parse_prefix_expression, this));
    m_prefix_parse_fns.emplace(TokenType::MINUS, std::bind(&Parser::parse_prefix_expression, this));
    m_prefix_parse_fns.emplace(TokenType::LPAREN, std::bind(&Parser::parse_grouped_expression, this));
    m_prefix_parse_fns.emplace(TokenType::IF, std::bind(&Parser::parse_if_expression, this));
    m_prefix_parse_fns.emplace(TokenType::FUNCTION, std::bind(&Parser::parse_fn, this));
    m_prefix_parse_fns.emplace(TokenType::STRING, std::bind(&Parser::parse_string, this));
    m_prefix_parse_fns.emplace(TokenType::LBRACKET, std::bind(&Parser::parse_array, this));
    m_prefix_parse_fns.emplace(TokenType::LBRACE, std::bind(&Parser::parse_hash, this));

    m_infix_parse_fns.emplace(TokenType::LBRACKET, std::bind_front(&Parser::parse_index_expression, this));
    m_infix_parse_fns.emplace(TokenType::PLUS, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::MINUS, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::SLASH, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::ASTERISK, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::EQ, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::NOT_EQ, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::LT, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::GT, std::bind_front(&Parser::parse_infix_expression, this));
    m_infix_parse_fns.emplace(TokenType::LPAREN, std::bind_front(&Parser::parse_call_expression, this));
}

auto Parser::parse_program() -> std::unique_ptr<Program>
{
    TRACE();
    auto program = std::make_unique<Program>();
    while (m_curr.type != TokenType::EOFILE)
    {
        auto statement = parse_statement();
        if (statement)
        {
            program->m_statements.push_back(std::move(statement));
        }
        next_token();
    }
    return program;
}

void Parser::peek_error(TokenType unwanted_token)
{
    m_errors.push_back(fmt::format("expected next token to be {}, got {} instead", unwanted_token, m_next));
}

auto Parser::get_errors() const -> const std::vector<std::string>&
{
    return m_errors;
}

void Parser::next_token()
{
    m_curr = m_next;
    m_next = m_lexer->next_token();
}

auto Parser::parse_statement() -> std::unique_ptr<Statement>
{
    TRACE();
    switch (m_curr.type)
    {
    case TokenType::LET:
    {
        return parse_let_statement();
    }
    case TokenType::RETURN:
    {
        return parse_return_statement();
    }
    case TokenType::WHILE:
    {
        return parse_while_statement();
    }
    default:
        return parse_expression_statement();
    };
}

auto Parser::parse_let_statement() -> std::unique_ptr<LetStatement>
{
    TRACE();
    auto let_statement = std::make_unique<LetStatement>();
    let_statement->m_token = m_curr;
    if (!expect_peek(TokenType::IDENT))
    {
        return nullptr;
    }
    let_statement->m_name = std::make_unique<Identifier>();
    let_statement->m_name->m_token = m_curr;
    let_statement->m_name->m_value = m_curr.literal;

    if (!expect_peek(TokenType::ASSIGN))
    {
        return nullptr;
    }
    next_token();

    let_statement->m_value = parse_expression(Precedence::LOWEST);
    while (m_curr.type != TokenType::SEMICOLON)
    {
        next_token();
        if (m_curr.type == TokenType::EOFILE)
        {
            m_errors.push_back(fmt::format("expected {}, got {}", TokenType::SEMICOLON, TokenType::EOFILE));
            return nullptr;
        }
    }
    return let_statement;
}

auto Parser::parse_return_statement() -> std::unique_ptr<ReturnStatement>
{
    TRACE();
    auto return_statement = std::make_unique<ReturnStatement>();
    return_statement->m_token = m_curr;

    next_token();
    return_statement->m_return_value = parse_expression(Precedence::LOWEST);
    if (m_curr.type != TokenType::SEMICOLON)
    {
        next_token();
    }
    return return_statement;
}

auto Parser::parse_while_statement() -> std::unique_ptr<WhileStatement>
{
    TRACE();
    auto while_statement = std::make_unique<WhileStatement>();
    while_statement->m_token = m_curr;
    if (!expect_peek(TokenType::LPAREN))
    {
        return nullptr;
    }
    next_token();
    while_statement->m_condition = parse_expression(Precedence::LOWEST);
    if (!expect_peek(TokenType::RPAREN) || !while_statement->m_condition)
    {
        return nullptr;
    }
    next_token();
    while_statement->m_loop_body = parse_block_statement();
    if (!while_statement->m_loop_body)
    {
        return nullptr;
    }
    return while_statement;
}

auto Parser::parse_expression_statement() -> std::unique_ptr<ExpressionStatement>
{
    TRACE();
    auto expr_statement = std::make_unique<ExpressionStatement>();
    expr_statement->m_token = m_curr;

    expr_statement->m_expression = parse_expression(Precedence::LOWEST);
    if (m_next.type == TokenType::SEMICOLON)
    {
        next_token();
    }
    return expr_statement;
}

auto Parser::parse_expression(Precedence precedence) -> std::unique_ptr<Expression>
{
    TRACE();
    const auto prefix_it = m_prefix_parse_fns.find(m_curr.type);
    if (prefix_it == std::end(m_prefix_parse_fns))
    {
        m_errors.push_back(fmt::format("No prefix parse function found for {}", m_curr));
        return nullptr;
    }
    auto left_expr = prefix_it->second();
    while (m_next.type != TokenType::SEMICOLON && precedence < get_precedence(m_next.type))
    {
        const auto infix_it = m_infix_parse_fns.find(m_next.type);
        if (infix_it == std::end(m_infix_parse_fns))
        {
            return left_expr;
        }
        next_token();
        left_expr = infix_it->second(std::move(left_expr));
    }
    return left_expr;
}

bool Parser::expect_peek(TokenType type)
{
    if (m_next.type == type)
    {
        next_token();
        return true;
    }
    peek_error(type);
    return false;
}

auto Parser::parse_identifier() -> std::unique_ptr<Identifier>
{
    TRACE();
    auto expr = std::make_unique<Identifier>();
    expr->m_token = m_curr;
    expr->m_value = m_curr.literal;
    return expr;
}

auto Parser::parse_int() -> std::unique_ptr<IntegerLiteral>
{
    TRACE();
    auto expr = std::make_unique<IntegerLiteral>();
    expr->m_token = m_curr;

    auto [ptr, ec] = std::from_chars(m_curr.literal.data(), m_curr.literal.data() + m_curr.literal.size(), expr->m_value);
    if (ec != std::errc())
    {
        m_errors.push_back(fmt::format("failed to parse integer {} {}", m_curr.literal, ec));
        return nullptr;
    }
    return expr;
}

auto Parser::parse_bool() -> std::unique_ptr<BooleanLiteral>
{
    TRACE();
    auto expr = std::make_unique<BooleanLiteral>();
    expr->m_token = m_curr;
    expr->m_value = m_curr.type == TokenType::TRUE;
    return expr;
}

auto Parser::parse_fn_parameters() -> std::vector<std::shared_ptr<Identifier>>
{
    TRACE();
    if (m_next.type == TokenType::RPAREN)
    {
        next_token();
        return {};
    }
    next_token();
    auto ident = std::make_unique<Identifier>();
    ident->m_token = m_curr;
    ident->m_value = m_curr.literal;

    std::vector<std::shared_ptr<Identifier>> identifiers;
    identifiers.push_back(std::move(ident));

    while (m_next.type == TokenType::COMMA)
    {
        next_token();
        next_token();
        auto ident = std::make_shared<Identifier>();
        ident->m_token = m_curr;
        ident->m_value = m_curr.literal;

        identifiers.push_back(std::move(ident));
    }
    if (!expect_peek(TokenType::RPAREN))
    {
        return {};
    }
    return identifiers;
}

auto Parser::parse_string() -> std::unique_ptr<StringLiteral>
{
    TRACE();
    auto str_expr = std::make_unique<StringLiteral>();
    str_expr->m_token = m_curr;
    str_expr->m_value = m_curr.literal;
    return str_expr;
}

auto Parser::parse_array() -> std::unique_ptr<ArrayLiteral>
{
    TRACE();
    auto arr_expr = std::make_unique<ArrayLiteral>();
    arr_expr->m_token = m_curr;

    next_token();
    if (m_curr.type == TokenType::RBRACKET)
    {
        return arr_expr;
    }
    if (m_curr.type == TokenType::EOFILE)
    {
        m_errors.push_back("Expected expression got EOFILE");
        return nullptr;
    }
    arr_expr->m_expressions.push_back(parse_expression(Precedence::LOWEST));
    while (m_next.type == TokenType::COMMA)
    {
        next_token();
        next_token();
        arr_expr->m_expressions.push_back(parse_expression(Precedence::LOWEST));
    }
    if (!expect_peek(TokenType::RBRACKET))
    {
        return nullptr;
    }

    return arr_expr;
}

auto Parser::parse_hash() -> std::unique_ptr<HashLiteral>
{
    TRACE();
    auto hash_lit = std::make_unique<HashLiteral>();
    while (m_next.type != TokenType::RBRACE)
    {
        if (m_next.type == TokenType::EOFILE)
        {
            m_errors.push_back(fmt::format("expected expression or {}, got {}", TokenType::RBRACE, TokenType::EOFILE));
            return nullptr;
        }
        next_token();
        auto key = parse_expression(Precedence::LOWEST);
        if (!expect_peek(TokenType::COLON))
        {
            return nullptr;
        }
        next_token();
        auto val = parse_expression(Precedence::LOWEST);
        hash_lit->m_pairs.emplace_back(std::move(key), std::move(val));
        if (m_next.type != TokenType::RBRACE && !expect_peek(TokenType::COMMA))
        {
            return nullptr;
        }
    }
    if (!expect_peek(TokenType::RBRACE))
    {
        return nullptr;
    }
    return hash_lit;
}

auto Parser::parse_fn() -> std::unique_ptr<FnLiteral>
{
    TRACE();
    auto fn_expr = std::make_unique<FnLiteral>();
    if (!expect_peek(TokenType::LPAREN))
    {
        return nullptr;
    }
    fn_expr->m_parameters = parse_fn_parameters();
    if (!expect_peek(TokenType::LBRACE))
    {
        return nullptr;
    }
    fn_expr->m_body = parse_block_statement();
    return fn_expr;
}

auto Parser::parse_prefix_expression() -> std::unique_ptr<PrefixExpression>
{
    TRACE();
    auto expr = std::make_unique<PrefixExpression>();
    expr->m_token = m_curr;
    expr->m_operator = m_curr.literal;
    next_token();
    expr->m_right = parse_expression(Precedence::PREFIX);
    return expr;
}

auto Parser::parse_infix_expression(std::unique_ptr<Expression>&& left) -> std::unique_ptr<InfixExpression>
{
    TRACE();
    auto expr = std::make_unique<InfixExpression>();
    expr->m_token = m_curr;
    expr->m_operator = m_curr.literal;
    expr->m_left = std::move(left);

    const auto p = get_precedence(m_curr.type);
    next_token();
    expr->m_right = parse_expression(p);
    return expr;
}

auto Parser::parse_index_expression(std::unique_ptr<Expression>&& left) -> std::unique_ptr<IndexExpression>
{
    TRACE();
    auto index_expr = std::make_unique<IndexExpression>();
    index_expr->m_token = m_curr;
    index_expr->m_left = std::move(left);
    next_token();
    index_expr->m_index = parse_expression(Precedence::LOWEST);
    if (!expect_peek(TokenType::RBRACKET))
    {
        return nullptr;
    }
    return index_expr;
}

auto Parser::parse_grouped_expression() -> std::unique_ptr<Expression>
{
    TRACE();
    next_token();
    auto expr = parse_expression(Precedence::LOWEST);
    if (!expect_peek(TokenType::RPAREN))
    {
        return nullptr;
    }
    return expr;
}

auto Parser::parse_if_expression() -> std::unique_ptr<IfExpression>
{
    TRACE();
    auto expr = std::make_unique<IfExpression>();
    expr->m_token = m_curr;

    if (!expect_peek(TokenType::LPAREN))
    {
        return nullptr;
    }
    next_token();
    expr->m_condition = parse_expression(Precedence::LOWEST);
    if (!expect_peek(TokenType::RPAREN) || !expect_peek(TokenType::LBRACE))
    {
        return nullptr;
    }
    expr->m_consequence = parse_block_statement();
    if (m_next.type == TokenType::ELSE)
    {
        next_token();
        if (!expect_peek(TokenType::LBRACE))
        {
            return nullptr;
        }
        expr->m_alternative = parse_block_statement();
    }
    return expr;
}

auto Parser::parse_call_expression(std::unique_ptr<Expression>&& function) -> std::unique_ptr<CallExpression>
{
    TRACE();
    auto call_expr = std::make_unique<CallExpression>();
    call_expr->m_token = m_curr;
    call_expr->m_function = std::move(function);
    call_expr->m_arguments = parse_call_arguments();
    return call_expr;
}

auto Parser::parse_call_arguments() -> std::vector<std::unique_ptr<Expression>>
{
    TRACE();
    if (m_next.type == TokenType::RPAREN)
    {
        next_token();
        return {};
    }
    next_token();
    std::vector<std::unique_ptr<Expression>> args;
    args.push_back(parse_expression(Precedence::LOWEST));

    while (m_next.type == TokenType::COMMA)
    {
        next_token();
        next_token();
        args.push_back(parse_expression(Precedence::LOWEST));
    }
    if (!expect_peek(TokenType::RPAREN))
    {
        return {};
    }
    return args;
}

auto Parser::parse_block_statement() -> std::unique_ptr<BlockStatement>
{
    TRACE();
    auto block = std::make_unique<BlockStatement>();
    block->m_token = m_curr;
    next_token();
    while (m_curr.type != TokenType::RBRACE && m_curr.type != TokenType::EOFILE)
    {
        auto stmt = parse_statement();
        if (stmt)
        {
            block->m_statements.push_back(std::move(stmt));
        }
        next_token();
    }
    return block;
}

auto Parser::get_precedence(TokenType type) -> Precedence
{
    static constexpr auto PRECEDENCE_ORDER = std::array{
        std::make_pair(TokenType::EQ, Precedence::EQUALS),
        std::make_pair(TokenType::NOT_EQ, Precedence::EQUALS),
        std::make_pair(TokenType::LT, Precedence::LESSGREATER),
        std::make_pair(TokenType::GT, Precedence::LESSGREATER),
        std::make_pair(TokenType::PLUS, Precedence::SUM),
        std::make_pair(TokenType::MINUS, Precedence::SUM),
        std::make_pair(TokenType::SLASH, Precedence::PRODUCT),
        std::make_pair(TokenType::ASTERISK, Precedence::PRODUCT),
        std::make_pair(TokenType::LPAREN, Precedence::CALL),
        std::make_pair(TokenType::LBRACKET, Precedence::INDEX),
    };
    using value_t = typename decltype(PRECEDENCE_ORDER)::value_type;
    const auto it = std::find_if(std::begin(PRECEDENCE_ORDER), std::end(PRECEDENCE_ORDER), [type](const value_t& v) -> bool
                                 { return type == v.first; });
    if (it == std::end(PRECEDENCE_ORDER))
    {
        return Precedence::LOWEST;
    }
    return it->second;
}
}  // namespace mlang
