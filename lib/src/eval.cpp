#include "mlang/eval.hpp"

#include <memory>
#include <range/v3/view.hpp>
#include <type_traits>

namespace rv = ::ranges::views;
using namespace std::literals;

namespace
{
template <typename Getter>
auto eval_getter_pos(Getter&& callable, std::string_view name, const std::vector<std::shared_ptr<mlang::Object>>& args) -> std::shared_ptr<mlang::Object>
{
    if (std::size(args) != 1)
    {
        return std::make_shared<mlang::ErrorObj>(fmt::format("invalid number of parameters for {}, expected 1 got {}", name, std::size(args)));
    }
    auto& arg = *args[0];
    const auto arg_type = arg.get_type();
    if (arg_type == mlang::ObjectType::ARRAY)
    {
        auto& values = static_cast<mlang::ArrayObj&>(arg).m_values;
        if (values.empty())
        {
            return mlang::detail::NIL;
        }
        return std::invoke(std::forward<Getter>(callable), values);
    }
    return std::make_shared<mlang::ErrorObj>(fmt::format("{} is not implemented for type {}", name, arg_type));
}
}  // namespace

namespace mlang
{
namespace detail
{
const std::shared_ptr<BooleanObj> TRUE = std::make_shared<BooleanObj>(true);
const std::shared_ptr<BooleanObj> FALSE = std::make_shared<BooleanObj>(false);
const std::shared_ptr<NullObj> NIL = std::make_shared<NullObj>();

const std::shared_ptr<BuiltInObj> LEN = std::make_shared<BuiltInObj>(&eval_len);
const std::shared_ptr<BuiltInObj> REST = std::make_shared<BuiltInObj>(&eval_rest);
const std::shared_ptr<BuiltInObj> PUTS = std::make_shared<BuiltInObj>(&eval_puts);
const std::shared_ptr<BuiltInObj> PUSH = std::make_shared<BuiltInObj>(&eval_push);
const std::shared_ptr<BuiltInObj> ERASE = std::make_shared<BuiltInObj>(&eval_erase);
const std::shared_ptr<BuiltInObj> FIRST = std::make_shared<BuiltInObj>([](const std::vector<std::shared_ptr<Object>>& args)
                                                                       { return eval_getter_pos([](const auto& cont)
                                                                                                { return cont.front(); },
                                                                                                "first"sv, args); });
const std::shared_ptr<BuiltInObj> LAST = std::make_shared<BuiltInObj>([](const std::vector<std::shared_ptr<Object>>& args)
                                                                      { return eval_getter_pos([](const auto& cont)
                                                                                               { return cont.back(); },
                                                                                               "last"sv, args); });

const std::unordered_map<std::string_view, std::shared_ptr<Object>> BUILTINS = {
    std::make_pair("len"sv, LEN),
    std::make_pair("first"sv, FIRST),
    std::make_pair("last"sv, LAST),
    std::make_pair("rest"sv, REST),
    std::make_pair("push"sv, PUSH),
    std::make_pair("puts"sv, PUTS),
    std::make_pair("erase"sv, ERASE),
};

auto eval_len(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::size(args) != 1)
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for len, expected 1 got {}", std::size(args)));
    }
    auto& arg = *args[0];
    if (arg.get_type() == ObjectType::STRING)
    {
        return std::make_shared<IntegerObj>(static_cast<StringObj&>(arg).m_value.size());
    }
    if (arg.get_type() == ObjectType::ARRAY)
    {
        return std::make_shared<IntegerObj>(static_cast<ArrayObj&>(arg).m_values.size());
    }
    return std::make_shared<ErrorObj>(fmt::format("len is not implemented for type {}", arg.get_type()));
}

auto eval_puts(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::empty(args))
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for puts, expected  at least 1 got {}", std::size(args)));
    }
    for (const auto& obj : args)
    {
        fmt::println("{}", obj->inspect());
    }
    return NIL;
}

auto eval_rest(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::size(args) != 1)
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for rest, expected 1 got {}", std::size(args)));
    }
    auto& arg = *args[0];
    const auto arg_type = arg.get_type();
    if (arg_type == ObjectType::ARRAY)
    {
        const auto& values = static_cast<ArrayObj&>(arg).m_values;
        if (values.empty())
        {
            return NIL;
        }
        return std::make_shared<ArrayObj>(decltype(values){std::next(std::begin(values)), std::end(values)});
    }
    if (arg_type == ObjectType::STRING)
    {
        const auto& value = static_cast<StringObj&>(arg).m_value;
        if (value.empty())
        {
            return NIL;
        }
        return std::make_shared<StringObj>(decltype(value){std::next(std::begin(value)), std::end(value)});
    }
    return std::make_shared<ErrorObj>(fmt::format("rest is not implemented for type {}", arg_type));
}

auto eval_push(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::size(args) < 1)
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for push {}", std::size(args)));
    }
    auto& arg = *args[0];
    const auto arg_type = arg.get_type();
    if (arg_type == ObjectType::ARRAY)
    {
        if (std::size(args) != 2)
        {
            return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for push, expected 2 got {}", std::size(args)));
        }
        auto values = static_cast<ArrayObj&>(arg).m_values;
        values.push_back(args[1]);
        return std::make_shared<ArrayObj>(values);
    }
    if (arg_type == ObjectType::HASH)
    {
        auto values = static_cast<HashObj&>(arg).m_pairs;
        values[args[1]] = args[2];
        return std::make_shared<HashObj>(std::move(values));
    }
    return std::make_shared<ErrorObj>(fmt::format("push is not implemented for type {}", arg.get_type()));
}

auto eval_erase(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::size(args) != 2)
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of parameters for erase, expected 2 got {}", std::size(args)));
    }
    auto& arg = *args[0];
    if (arg.get_type() == ObjectType::HASH)
    {
        auto values = static_cast<HashObj&>(arg).m_pairs;
        values.erase(args[1]);
        return std::make_shared<HashObj>(values);
    }
    return std::make_shared<ErrorObj>(fmt::format("erase is not implemented for type {}", arg.get_type()));
}

auto is_truth(const std::shared_ptr<Object>& obj) -> bool
{
    return obj != FALSE && obj != NIL;
}

auto eval_program(Program& prog, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>
{
    std::shared_ptr<Object> res;
    using value_t = typename decltype(prog.m_statements)::value_type;
    for (auto* s : prog.m_statements | rv::transform([](const value_t& ptr)
                                                     { return ptr.get(); }))
    {
        res = eval(s, env);
        if (res && res->get_type() == ObjectType::RETURN)
        {
            return static_cast<ReturnValueObj&>(*res).m_value;
        }
        else if (res && res->get_type() == ObjectType::ERROR)
        {
            return res;
        }
    }
    return res;
}

auto eval_block_statement(BlockStatement& stmt, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>
{
    std::shared_ptr<Object> res;
    using value_t = typename decltype(stmt.m_statements)::value_type;
    for (auto* stmt : stmt.m_statements | rv::transform([](const value_t& val)
                                                        { return val.get(); }))
    {
        res = eval(stmt, env);
        if (res && (res->get_type() == ObjectType::RETURN || res->get_type() == ObjectType::ERROR))
        {
            return res;
        }
    }
    return res;
}

auto eval_identifier(Identifier& node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>
{
    auto val = env->get_obj(node.m_value);
    if (val)
    {
        return val;
    }
    if (const auto it = BUILTINS.find(node.m_value); it != std::end(BUILTINS))
    {
        return it->second;
    }
    return std::make_shared<ErrorObj>(fmt::format("identifier not found: {}", node.m_value));
}

auto eval_expressions(const std::vector<std::unique_ptr<Expression>>& nodes, const std::shared_ptr<Context>& env) -> std::vector<std::shared_ptr<Object>>
{
    std::vector<std::shared_ptr<Object>> res;
    using value_t = typename std::remove_cvref_t<decltype(nodes)>::value_type;
    for (auto* expr : nodes | rv::transform([](const value_t& val)
                                            { return val.get(); }))
    {
        auto evaluated = eval(expr, env);
        if (evaluated->get_type() == ObjectType::ERROR)
        {
            return {std::move(evaluated)};
        }
        res.push_back(std::move(evaluated));
    }
    return res;
}

auto apply_function(const std::shared_ptr<FunctionObj>& fn, const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>
{
    if (std::size(fn->m_parameters) != std::size(args))
    {
        return std::make_shared<ErrorObj>(fmt::format("invalid number of args expected {} got {}", std::size(fn->m_parameters), std::size(args)));
    }
    auto extended_env = std::make_shared<Context>(fn->m_env);
    for (const auto& [i, arg] : rv::enumerate(fn->m_parameters))
    {
        extended_env->set_obj(arg->m_value, args.at(i));
    }
    auto evaluated = eval(fn->m_body.get(), extended_env);
    if (evaluated->get_type() == ObjectType::RETURN)
    {
        return static_cast<ReturnValueObj&>(*evaluated).m_value;
    }
    return evaluated;
}

auto eval_int_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    const auto right_val = static_cast<IntegerObj&>(*right).m_value;
    const auto left_val = static_cast<IntegerObj&>(*left).m_value;

    if (op == "+")
    {
        return std::make_shared<IntegerObj>(left_val + right_val);
    }
    else if (op == "-")
    {
        return std::make_shared<IntegerObj>(left_val - right_val);
    }
    else if (op == "*")
    {
        return std::make_shared<IntegerObj>(left_val * right_val);
    }
    else if (op == "/")
    {
        return std::make_shared<IntegerObj>(left_val / right_val);
    }
    else if (op == ">")
    {
        return left_val > right_val ? TRUE : FALSE;
    }
    else if (op == "<")
    {
        return left_val < right_val ? TRUE : FALSE;
    }
    else if (op == "!=")
    {
        return left_val != right_val ? TRUE : FALSE;
    }
    else if (op == "==")
    {
        return left_val == right_val ? TRUE : FALSE;
    }
    return std::make_shared<ErrorObj>(fmt::format("unknown operator: {} {} {}", left->get_type(), op, right->get_type()));
}

auto eval_string_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    const auto right_val = static_cast<StringObj&>(*right).m_value;
    const auto left_val = static_cast<StringObj&>(*left).m_value;

    if (op == "+")
    {
        return std::make_shared<StringObj>(left_val + right_val);
    }
    return std::make_shared<ErrorObj>(fmt::format("unknown operator: {} {} {}", left->get_type(), op, right->get_type()));
}

auto eval_bool_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    const auto right_val = static_cast<BooleanObj&>(*right).m_value;
    const auto left_val = static_cast<BooleanObj&>(*left).m_value;

    if (op == "==")
    {
        return std::make_shared<BooleanObj>(left_val == right_val);
    }
    else if (op == "!=")
    {
        return std::make_shared<BooleanObj>(left_val != right_val);
    }
    return std::make_shared<ErrorObj>(fmt::format("unknown operator: {} {} {}", left->get_type(), op, right->get_type()));
}

auto eval_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    const auto left_type = left->get_type();
    if (left_type != right->get_type())
    {
        return std::make_shared<ErrorObj>(fmt::format("type mismatch: {} {} {}", left->get_type(), op, right->get_type()));
    }
    else if (left_type == ObjectType::INTEGER)
    {
        return eval_int_infix_expression(op, left, right);
    }
    else if (left_type == ObjectType::BOOLEAN)
    {
        return eval_bool_infix_expression(op, left, right);
    }
    else if (left_type == ObjectType::STRING)
    {
        return eval_string_infix_expression(op, left, right);
    }
    return std::make_shared<ErrorObj>(fmt::format("unknown operator: {} {} {}", left->get_type(), op, right->get_type()));
}

auto eval_if_expression(IfExpression& expr, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>
{
    auto condition = eval(expr.m_condition.get(), env);
    if (condition->get_type() == ObjectType::ERROR)
    {
        return condition;
    }
    if (is_truth(condition))
    {
        return eval(expr.m_consequence.get(), env);
    }
    else if (expr.m_alternative)
    {
        return eval(expr.m_alternative.get(), env);
    }
    return NIL;
}

auto eval_minus_prefix_operator(const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    if (right->get_type() != ObjectType::INTEGER)
    {
        return std::make_shared<ErrorObj>(fmt::format("unknown operator: -{}", right->get_type()));
    }
    const auto val = static_cast<IntegerObj&>(*right).m_value;
    return std::make_shared<IntegerObj>(-val);
}

auto eval_bang_expression(const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    if (right == FALSE)
    {
        return TRUE;
    }
    else if (right == TRUE)
    {
        return FALSE;
    }
    else if (right == NIL)
    {
        return TRUE;
    }
    return FALSE;
}

auto eval_prefix_expression(const std::string& op, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>
{
    if (op == "!")
    {
        return eval_bang_expression(right);
    }
    else if (op == "-")
    {
        return eval_minus_prefix_operator(right);
    }
    return std::make_shared<ErrorObj>(fmt::format("unknown operator: {}{}", op, right->get_type()));
}

auto eval_index_expression(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& index) -> std::shared_ptr<Object>
{
    const auto obj_type = obj->get_type();
    if (obj_type == ObjectType::ARRAY)
    {
        if (index->get_type() != ObjectType::INTEGER)
        {
            return std::make_shared<ErrorObj>(fmt::format("Expected index type to be {}, got {}", ObjectType::INTEGER, index->get_type()));
        }
        const auto& arr_obj = static_cast<ArrayObj&>(*obj);
        const auto idx = static_cast<IntegerObj&>(*index).m_value;
        const auto max_element = static_cast<std::int64_t>(std::size(arr_obj.m_values));

        if (idx < 0 || idx >= max_element)
        {
            return NIL;
        }
        return arr_obj.m_values[idx];
    }
    if (obj_type == ObjectType::HASH)
    {
        const auto& hash_obj = static_cast<HashObj&>(*obj);
        const auto it = hash_obj.m_pairs.find(index);
        if (it == std::end(hash_obj.m_pairs))
        {
            return NIL;
        }
        return it->second;
    }
    return std::make_shared<ErrorObj>(fmt::format("Index operator not supported for type {}", obj->get_type()));
}
}  // namespace detail

auto eval(Node* node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>
{
    assert(node);
    assert(env);
    const auto node_type = node->get_type();
    if (node_type == NodeType::Program)
    {
        auto* nd = static_cast<Program*>(node);
        return detail::eval_program(*nd, env);
    }
    else if (node_type == NodeType::PrefixExpression)
    {
        auto* nd = static_cast<PrefixExpression*>(node);
        auto right = eval(nd->m_right.get(), env);
        return detail::eval_prefix_expression(nd->m_operator, right);
    }
    else if (node_type == NodeType::ExpressionStatement)
    {
        auto* nd = static_cast<ExpressionStatement*>(node);
        return eval(nd->m_expression.get(), env);
    }
    else if (node_type == NodeType::BlockStatement)
    {
        auto* nd = static_cast<BlockStatement*>(node);
        return detail::eval_block_statement(*nd, env);
    }
    else if (node_type == NodeType::IfExpression)
    {
        auto* nd = static_cast<IfExpression*>(node);
        return detail::eval_if_expression(*nd, env);
    }
    else if (node_type == NodeType::IntegerLiteral)
    {
        auto* nd = static_cast<IntegerLiteral*>(node);
        auto obj = std::make_shared<IntegerObj>(nd->m_value);
        return obj;
    }
    else if (node_type == NodeType::StringLiteral)
    {
        auto* nd = static_cast<StringLiteral*>(node);
        auto obj = std::make_shared<StringObj>(nd->m_value);
        return obj;
    }
    else if (node_type == NodeType::BooleanLiteral)
    {
        auto* nd = static_cast<BooleanLiteral*>(node);
        return nd->m_value ? detail::TRUE : detail::FALSE;
    }
    else if (node_type == NodeType::InfixExpression)
    {
        auto* nd = static_cast<InfixExpression*>(node);
        auto left = eval(nd->m_left.get(), env);
        if (left->get_type() == ObjectType::ERROR)
        {
            return left;
        }
        auto right = eval(nd->m_right.get(), env);
        if (left->get_type() == ObjectType::ERROR)
        {
            return right;
        }
        return detail::eval_infix_expression(nd->m_operator, left, right);
    }
    else if (node_type == NodeType::ReturnStatement)
    {
        auto* nd = static_cast<ReturnStatement*>(node);
        auto val = eval(nd->m_return_value.get(), env);
        return std::make_shared<ReturnValueObj>(std::move(val));
    }
    else if (node_type == NodeType::Identifier)
    {
        auto* nd = static_cast<Identifier*>(node);
        return detail::eval_identifier(*nd, env);
    }
    else if (node_type == NodeType::LetStatement)
    {
        auto* nd = static_cast<LetStatement*>(node);
        auto val = eval(nd->m_value.get(), env);
        if (val->get_type() == ObjectType::ERROR)
        {
            return val;
        }
        env->set_obj(nd->m_name->m_value, val);
        return detail::NIL;
    }
    else if (node_type == NodeType::FnLiteral)
    {
        auto* nd = static_cast<FnLiteral*>(node);
        return std::make_shared<FunctionObj>(nd->m_parameters, nd->m_body, env);
    }
    else if (node_type == NodeType::ArrayLiteral)
    {
        auto* nd = static_cast<ArrayLiteral*>(node);
        auto elements = detail::eval_expressions(nd->m_expressions, env);
        if (elements.size() == 1 && elements.front()->get_type() == ObjectType::ERROR)
        {
            return std::move(elements.front());
        }
        return std::make_shared<ArrayObj>(elements);
    }
    else if (node_type == NodeType::IndexExpression)
    {
        auto* nd = static_cast<IndexExpression*>(node);
        auto left = eval(nd->m_left.get(), env);
        if (left->get_type() == ObjectType::ERROR)
        {
            return left;
        }
        auto index = eval(nd->m_index.get(), env);
        return detail::eval_index_expression(left, index);
    }
    else if (node_type == NodeType::HashLiteral)
    {
        auto* nd = static_cast<HashLiteral*>(node);
        auto hash_obj = std::make_shared<HashObj>();
        for (const auto& [key, val] : nd->m_pairs)
        {
            auto key_obj = eval(key.get(), env);
            if (key_obj->get_type() == ObjectType::ERROR)
            {
                return key_obj;
            }
            auto val_obj = eval(val.get(), env);
            if (val_obj->get_type() == ObjectType::ERROR)
            {
                return val_obj;
            }
            hash_obj->m_pairs[std::move(key_obj)] = std::move(val_obj);
        }
        return hash_obj;
    }
    else if (node_type == NodeType::WhileStatement)
    {
        auto* nd = static_cast<WhileStatement*>(node);
        auto condition = eval(nd->m_condition.get(), env);
        if (condition->get_type() == ObjectType::ERROR)
        {
            return condition;
        }
        while (detail::is_truth(condition))
        {
            auto body = eval(nd->m_loop_body.get(), env);
            if (body->get_type() == ObjectType::ERROR)
            {
                return body;
            }
            condition = eval(nd->m_condition.get(), env);
            if (condition->get_type() == ObjectType::ERROR)
            {
                return condition;
            }
        }
        return detail::NIL;
    }
    else if (node_type == NodeType::CallExpression)
    {
        auto* nd = static_cast<CallExpression*>(node);
        auto func = eval(nd->m_function.get(), env);
        if (func->get_type() == ObjectType::ERROR)
        {
            return func;
        }
        auto args = detail::eval_expressions(nd->m_arguments, env);
        if (args.size() == 1 && args.front()->get_type() == ObjectType::ERROR)
        {
            return std::move(args.front());
        }
        if (func->get_type() == ObjectType::BUILTIN)
        {
            return static_cast<BuiltInObj&>(*func).m_value(args);
        }
        return detail::apply_function(std::static_pointer_cast<FunctionObj>(func), args);
    }
    return nullptr;
}
}  // namespace mlang