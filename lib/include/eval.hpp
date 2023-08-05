#pragma once

#include "node.hpp"
#include "object.hpp"

#include <memory>

inline const auto TRUE = std::make_shared<BooleanObj>(true);
inline const auto FALSE = std::make_shared<BooleanObj>(false);
inline const auto NIL = std::make_shared<NullObj>();

auto eval(Node* node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;