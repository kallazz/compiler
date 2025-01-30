#ifndef ARGUMENT_TYPE_HPP
#define ARGUMENT_TYPE_HPP

#include <string>

enum class ArgumentType {
    NUMBER,
    ARRAY
};

inline std::string toString(const ArgumentType argumentType) {
    switch (argumentType) {
        case ArgumentType::NUMBER: return "NUMBER";
        case ArgumentType::ARRAY: return "ARRAY";
        default: return "UNKNOWN";
    }
}

#endif // ARGUMENT_TYPE_HPP
