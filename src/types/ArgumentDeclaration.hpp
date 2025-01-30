#ifndef ARGUMENT_DECLARATION_HPP
#define ARGUMENT_DECLARATION_HPP

#include "ArgumentType.hpp"

#include <string>

struct ArgumentDeclaration {
    const int lineNumber;
    const std::string name;
    const ArgumentType argumentType;
};

#endif  // ARGUMENT_DECLARATION_HPP
