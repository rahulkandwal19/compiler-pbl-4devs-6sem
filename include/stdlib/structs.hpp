#pragma once
#include <string>
#include <map>

#define STRUCT(type, ...) struct type { __VA_ARGS__ }

#define DEFINE_STRUCT(type, ...) \
    struct type { __VA_ARGS__ };

#define CREATE_STRUCT(type) type()

#define SET_FIELD(obj, field, value) obj.field = value
#define GET_FIELD(obj, field) obj.field
