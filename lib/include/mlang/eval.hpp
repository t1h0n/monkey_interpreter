#pragma once

#include "mlang/object.hpp"
#include "node.hpp"

#include <memory>


namespace mlang
{
auto eval(Node* node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;

namespace detail
{
extern const std::shared_ptr<BooleanObj> TRUE;
extern const std::shared_ptr<BooleanObj> FALSE;
extern const std::shared_ptr<NullObj> NIL;

extern const std::unordered_map<std::string_view, std::shared_ptr<Object>> BUILTINS;
extern const std::shared_ptr<BuiltInObj> LEN;
extern const std::shared_ptr<BuiltInObj> REST;
extern const std::shared_ptr<BuiltInObj> PUTS;
extern const std::shared_ptr<BuiltInObj> PUSH;
extern const std::shared_ptr<BuiltInObj> ERASE;
extern const std::shared_ptr<BuiltInObj> FIRST;
extern const std::shared_ptr<BuiltInObj> LAST;

auto is_truth(const std::shared_ptr<Object>& obj) -> bool;

auto eval_len(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_puts(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_rest(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_push(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_erase(const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_program(Program& prog, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;
auto eval_block_statement(BlockStatement& stmt, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;
auto eval_identifier(Identifier& node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;
auto eval_expressions(const std::vector<std::unique_ptr<Expression>>& nodes, const std::shared_ptr<Context>& env) -> std::vector<std::shared_ptr<Object>>;
auto apply_function(const std::shared_ptr<FunctionObj>& fn, const std::vector<std::shared_ptr<Object>>& args) -> std::shared_ptr<Object>;
auto eval_int_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_string_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_bool_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_infix_expression(const std::string& op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_if_expression(IfExpression& expr, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;
auto eval_minus_prefix_operator(const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_bang_expression(const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_prefix_expression(const std::string& op, const std::shared_ptr<Object>& right) -> std::shared_ptr<Object>;
auto eval_index_expression(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& index) -> std::shared_ptr<Object>;
}  // namespace detail
}  // namespace mlang