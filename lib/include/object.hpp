#pragma once

#include "node.hpp"
#include "string_hash.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class Context;

enum class ObjectType
{
    INTEGER,
    BOOLEAN,
    NIL,
    RETURN,
    ERROR,
    FUNCTION,
    STRING,
    BUILTIN,
    ARRAY,
    HASH,
};

class Object
{
public:
    virtual auto get_type() -> ObjectType = 0;
    virtual auto inspect() -> std::string = 0;
    virtual ~Object() = 0;
};

class IntegerObj : public Object
{
public:
    IntegerObj(std::int64_t value);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::int64_t m_value;
};

class BooleanObj : public Object
{
public:
    BooleanObj(bool value);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    bool m_value;
};

class StringObj : public Object
{
public:
    StringObj(std::string_view value);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::string m_value;
};

class NullObj : public Object
{
public:
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;
};

class ReturnValueObj : public Object
{
public:
    ReturnValueObj(std::shared_ptr<Object> obj);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::shared_ptr<Object> m_value;
};

class BuiltInObj : public Object
{
public:
    using BuiltInFn = std::function<std::shared_ptr<Object>(const std::vector<std::shared_ptr<Object>>&)>;
    BuiltInObj(const BuiltInFn&);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    BuiltInFn m_value;
};

class ArrayObj : public Object
{
public:
    ArrayObj(const std::vector<std::shared_ptr<Object>>& values);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::vector<std::shared_ptr<Object>> m_values;
};

class ErrorObj : public Object
{
public:
    ErrorObj(std::string what);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::string m_what;
};

class FunctionObj : public Object
{
public:
    FunctionObj(const std::vector<std::shared_ptr<Identifier>>& parameters,
                const std::shared_ptr<BlockStatement>& body,
                const std::shared_ptr<Context>& env);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    std::vector<std::shared_ptr<Identifier>> m_parameters;
    std::shared_ptr<BlockStatement> m_body;
    std::shared_ptr<Context> m_env;
};

namespace detail
{
struct object_hash
{
    std::size_t operator()(const std::shared_ptr<Object>& expr) const
    {
        const auto expr_type = expr->get_type();
        if (expr_type == ObjectType::INTEGER)
        {
            return std::hash<decltype(static_cast<IntegerObj&>(*expr).m_value)>()(static_cast<IntegerObj&>(*expr).m_value);
        }
        if (expr_type == ObjectType::STRING)
        {
            return std::hash<decltype(static_cast<StringObj&>(*expr).m_value)>()(static_cast<StringObj&>(*expr).m_value);
        }
        if (expr_type == ObjectType::BOOLEAN)
        {
            return std::hash<decltype(static_cast<BooleanObj&>(*expr).m_value)>()(static_cast<BooleanObj&>(*expr).m_value);
        }
        throw std::runtime_error(fmt::format("object of type {} is not hashable", expr_type));
    }
};

struct object_eq
{
    std::size_t operator()(const std::shared_ptr<Object>& lhs, const std::shared_ptr<Object>& rhs) const
    {
        const auto l_type = lhs->get_type();
        const auto r_type = rhs->get_type();
        if (l_type != r_type)
        {
            return false;
        }
        if (l_type == ObjectType::INTEGER)
        {
            return static_cast<IntegerObj&>(*lhs).m_value == static_cast<IntegerObj&>(*rhs).m_value;
        }
        if (l_type == ObjectType::STRING)
        {
            return static_cast<StringObj&>(*lhs).m_value == static_cast<StringObj&>(*rhs).m_value;
        }
        if (l_type == ObjectType::BOOLEAN)
        {
            return static_cast<BooleanObj&>(*lhs).m_value == static_cast<BooleanObj&>(*rhs).m_value;
        }
        return false;
    }
};
}  // namespace detail

class HashObj : public Object
{
public:
    using ObjHashMap = std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>, detail::object_hash, detail::object_eq>;
    HashObj() = default;
    HashObj(ObjHashMap&& objects);
    HashObj(const ObjHashMap& objects);
    auto get_type() -> ObjectType override;
    auto inspect() -> std::string override;

public:
    ObjHashMap m_pairs;
};

class Context
{
    using ObjectsMap = std::unordered_map<std::string, std::shared_ptr<Object>, string_hash, std::equal_to<>>;

public:
    Context() = default;
    Context(const std::shared_ptr<Context>& parent_env);
    auto get_obj(std::string_view name) -> std::shared_ptr<Object>;
    void set_obj(std::string_view name, const std::shared_ptr<Object>& obj);

private:
    ObjectsMap m_objects;
    std::shared_ptr<Context> m_parent_env;
};
