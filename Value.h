#pragma once

#include <variant>
#include <vector>
#include <deque>

// Value type: Nystrom uses a self constructed tagged union for the Lox Value,
// but I rather learn about std::variant instead

struct Nil {};

using Number = double;
using Value  = std::variant<Nil, bool, Number>;

using Values     = std::vector<Value>;
using ValueStack = std::deque<Value>; // std::stack doesn't allow random access...

// accessors for readability
#define IS_NIL(v)    (v.index() == 0)
#define IS_BOOL(v)   (v.index() == 1)
#define IS_NUMBER(v) (v.index() == 2)

#define AS_NIL(v)    (std::get<Nil>(v))
#define AS_BOOL(v)   (std::get<bool>(v))
#define AS_NUMBER(v) (std::get<Number>(v))