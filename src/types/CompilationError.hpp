#ifndef COMPILATION_ERROR_HPP
#define COMPILATION_ERROR_HPP

#include <iostream>
#include <string>

struct CompilationError {
    std::string message;
    int lineNumber;
};

inline void printCompilationError(const CompilationError &compilationError) {
    std::cerr << "Error at line " << compilationError.lineNumber << ": " << compilationError.message << '\n';
}

#endif  // COMPILATION_ERROR_HPP
