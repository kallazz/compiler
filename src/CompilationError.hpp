#ifndef COMPILATION_ERROR_HPP
#define COMPILATION_ERROR_HPP

#include <string>

struct CompilationError {
    std::string message;
    int lineNumber;
};

#endif // COMPILATION_ERROR_HPP