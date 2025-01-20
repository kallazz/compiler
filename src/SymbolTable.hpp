#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "CompilationError.hpp"
#include "enums/ArgumentType.hpp"
#include "structs/Argument.hpp"
#include "structs/ArgumentDeclaration.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>

struct VariableInfo {
    const long long address;
    const std::optional<std::pair<long long, long long>> arrayRange;
};

struct ProcedureInfo {
    const std::map<std::string, std::pair<ArgumentType, long long>> argumentTable;
    std::map<std::string, VariableInfo> variableTable;
};

class SymbolTable {
public:
    SymbolTable();

    bool declareNumberVariableInMain(const int lineNumber, const std::string& name);
    bool declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound);
    bool checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name);
    bool checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex);

    bool declareNumberVariableInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName);
    bool declareArrayVariableInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const std::string& procedureName);
    bool checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName);
    bool checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::string& procedureName);

    bool declareProcedure(const int lineNumber, const std::string& name, const std::vector<ArgumentDeclaration>& argumentDeclarations);
    bool checkIfProcedureExists(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments, const std::optional<std::string> scopeProcedureName);

    CompilationError getCompilationError() const;

private:
    bool declareNumberVariable(const int lineNumber, const std::string& name, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::map<std::string, std::pair<ArgumentType, long long>>>> argumentTable);
    bool declareArrayVariable(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::map<std::string, std::pair<ArgumentType, long long>>>> argumentTable);
    bool checkIfNumberVariableExists(const int lineNumber, const std::string& name, const std::map<std::string, VariableInfo>& variableTable);
    bool checkIfArrayVariableExists(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::map<std::string, VariableInfo>& variableTable);
    bool checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::map<std::string, std::pair<ArgumentType, long long>>& argumentTable);
    bool checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::map<std::string, std::pair<ArgumentType, long long>>& argumentTable);
    std::optional<ArgumentType> checkIfVariableExistsAndGetItsType(const std::string& name, const std::optional<std::string> scopeProcedureName);

    std::map<std::string, VariableInfo> mainVariableTable_;
    std::map<std::string, ProcedureInfo> procedureTable_;
    long long currentAddress_;
    CompilationError compilationError_;
};

#endif // SYMBOL_TABLE_HPP
