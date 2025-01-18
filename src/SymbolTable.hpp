#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "CompilationError.hpp"
#include <map>
#include <optional>
#include <string>

struct SymbolInfo {
    const long long address;
    const std::optional<std::pair<long long, long long>> arrayRange;
};

class SymbolTable {
public:
    SymbolTable();

    bool declareVariable(const int lineNumber, const std::string& name);
    bool declareArray(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound);
    bool checkIfVariableExists(const int lineNumber, const std::string& name);
    bool checkIfArrayExists(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex);
    bool checkIfVariableOrArrayExists(const int lineNumber, const std::string& name); // this is only for handling arguments in procedure calls

    bool declareVariableInProcedure(const int lineNumber, const std::string& name, const int procedureIndex);
    bool declareArrayInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const int procedureIndex);
    bool checkIfVariableExistsInProcedure(const int lineNumber, const std::string& name, const int procedureIndex);
    bool checkIfArrayExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const int procedureIndex);
    bool checkIfVariableOrArrayExistsInProcedure(const int lineNumber, const std::string& name, const int procedureIndex); // this is only for handling arguments in procedure calls

    // TODO: declare procedure(with types?)
    // TODO: check if procedure exists

    CompilationError getCompilationError() const;

private:
    bool declareVariableWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix);
    bool declareArrayWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const long long lowerBound, const long long upperBound);
    bool checkIfVariableExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix);
    bool checkIfArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const std::optional<long long> arrayIndex);
    bool checkIfVariableOrArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix);

    std::map<std::string, SymbolInfo> table_;
    long long currentAddress_;
    CompilationError compilationError_;
};

#endif // SYMBOL_TABLE_HPP
