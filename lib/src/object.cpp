#include "mlang/object.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <range/v3/view.hpp>

namespace rv = ranges::views;

namespace mlang
{
Object::~Object() = default;

IntegerObj::IntegerObj(std::int64_t value)
    : m_value(value)
{
}

auto IntegerObj::get_type() -> ObjectType
{
    return ObjectType::INTEGER;
}

auto IntegerObj::inspect() -> std::string
{
    return fmt::format("{}", m_value);
}

BooleanObj::BooleanObj(bool value)
    : m_value(value)
{
}

auto BooleanObj::get_type() -> ObjectType
{
    return ObjectType::BOOLEAN;
}

auto BooleanObj::inspect() -> std::string
{
    return fmt::format("{}", m_value);
}

StringObj::StringObj(std::string_view value)
    : m_value(value)
{
}

auto StringObj::get_type() -> ObjectType
{
    return ObjectType::STRING;
}

auto StringObj::inspect() -> std::string
{
    return "\"" + m_value + "\"";
}

auto NullObj::get_type() -> ObjectType
{
    return ObjectType::NIL;
}

auto NullObj::inspect() -> std::string
{
    return std::string("null");
}

ReturnValueObj::ReturnValueObj(std::shared_ptr<Object> obj)
    : m_value(std::move(obj))
{
}

auto ReturnValueObj::get_type() -> ObjectType
{
    return ObjectType::RETURN;
}

auto ReturnValueObj::inspect() -> std::string
{
    return m_value->inspect();
}

BuiltInObj::BuiltInObj(const BuiltInFn& fn)
    : m_value(fn)
{
}

auto BuiltInObj::get_type() -> ObjectType
{
    return ObjectType::BUILTIN;
}

auto BuiltInObj::inspect() -> std::string
{
    return "builtin function";
}

ErrorObj::ErrorObj(std::string what)
    : m_what(std::move(what))
{
}

auto ErrorObj::get_type() -> ObjectType
{
    return ObjectType::ERROR;
}

auto ErrorObj::inspect() -> std::string
{
    return fmt::format("ERROR: {}", m_what);
}

ArrayObj::ArrayObj(const std::vector<std::shared_ptr<Object>>& values)
    : m_values(values)
{
}

auto ArrayObj::get_type() -> ObjectType
{
    return ObjectType::ARRAY;
}

auto ArrayObj::inspect() -> std::string
{
    using value_t = typename decltype(m_values)::value_type;
    return fmt::format("[{}]", fmt::join(m_values | rv::transform([](const value_t& val)
                                                                  { return val->inspect(); }),
                                         ", "));
}

FunctionObj::FunctionObj(const std::vector<std::shared_ptr<Identifier>>& parameters,
                         const std::shared_ptr<BlockStatement>& body,
                         const std::shared_ptr<Context>& env)
    : m_parameters(parameters)
    , m_body(body)
    , m_env(env)
{
}

auto FunctionObj::get_type() -> ObjectType
{
    return ObjectType::FUNCTION;
}

auto FunctionObj::inspect() -> std::string
{
    using value_t = typename decltype(m_parameters)::value_type;
    return fmt::format("fn({}){{\n{}\n}}", fmt::join(m_parameters | rv::transform([](const value_t& ptr)
                                                                                  { return ptr->to_string(); }),
                                                     ", "),
                       m_body->to_string());
}

HashObj::HashObj(ObjHashMap&& objects)
    : m_pairs(std::move(objects))
{
}

HashObj::HashObj(const ObjHashMap& objects)
    : m_pairs(objects)
{
}

auto HashObj::get_type() -> ObjectType
{
    return ObjectType::HASH;
}

auto HashObj::inspect() -> std::string
{
    using value_t = typename decltype(m_pairs)::value_type;
    return fmt::format("{{{}}}", fmt::join(m_pairs | rv::transform([](const value_t& ptr_pair)
                                                                   { return ptr_pair.first->inspect() + ":" + ptr_pair.second->inspect(); }),
                                           ", "));
}

Context::Context(const std::shared_ptr<Context>& parent_env)
    : m_parent_env(parent_env)
{
}

auto Context::get_obj(std::string_view name) -> std::shared_ptr<Object>
{
    const auto it = m_objects.find(name);
    if (it != std::end(m_objects))
    {
        return it->second;
    }
    if (m_parent_env)
    {
        return m_parent_env->get_obj(name);
    }
    return nullptr;
}

void Context::set_obj(std::string_view name, const std::shared_ptr<Object>& obj)
{
    m_objects[std::string(name)] = obj;
}
}  // namespace mlang