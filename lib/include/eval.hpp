#pragma once

#include "node.hpp"
#include "object.hpp"

#include <memory>

auto eval(Node* node, const std::shared_ptr<Context>& env) -> std::shared_ptr<Object>;