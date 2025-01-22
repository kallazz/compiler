#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "CompilationError.hpp"
#include "enums/ArgumentType.hpp"
#include "structs/Argument.hpp"
#include "structs/ArgumentDeclaration.hpp"
#include <unordered_map>
#include <optional>
#include <string>
#include <vector>

struct VariableInfo {
    const long long address;
    const std::optional<std::pair<long long, long long>> arrayRange;
    const bool isIterator = false;
    bool isInitialized = false;
};

struct ArgumentInfo {
    const std::string name;
    const ArgumentType type;
    const long long address;
};

struct ProcedureInfo {
    const std::vector<ArgumentInfo> argumentInfos;
    std::unordered_map<std::string, VariableInfo> variableTable;
};

class SymbolTable {
public:
    SymbolTable();

    bool declareNumberVariableInMain(const int lineNumber, const std::string& name, const bool isIterator=false, const bool isInitialized=false);
    bool declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound);
    bool checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name, const bool willVariableBeModified);
    bool checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex);

    bool declareNumberVariableInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName, const bool isIterator=false, const bool isInitialized=false);
    bool declareArrayVariableInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const std::string& procedureName);
    bool checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name, const bool willVariableBeModified, const std::string& procedureName);
    bool checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::string& procedureName);

    bool declareProcedure(const int lineNumber, const std::string& name, const std::vector<ArgumentDeclaration>& argumentDeclarations);
    bool verifyProcedureCall(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments, const std::optional<std::string> scopeProcedureName);

    void removeVariableFromMain(const std::string& name);
    void removeVariableFromProcedure(const std::string& name, const std::string& procedureName);

    CompilationError getCompilationError() const;

private:
    bool declareNumberVariable(const int lineNumber, const std::string& name, std::unordered_map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos, const bool isIterator, const bool isInitialized);
    bool declareArrayVariable(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, std::unordered_map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos);
    bool checkIfNumberVariableExists(const int lineNumber, const std::string& name, std::unordered_map<std::string, VariableInfo>& variableTable, const bool willVariableBeModified);
    bool checkIfArrayVariableExists(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::unordered_map<std::string, VariableInfo>& variableTable);
    bool checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::vector<ArgumentInfo>& argumentInfos);
    bool checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::vector<ArgumentInfo>& argumentInfos);
    std::optional<std::pair<bool, ArgumentType>> checkIfVariableExistsAndSetItAsInitializedAndGetItsInfo(const std::string& name, const std::optional<std::string> scopeProcedureName);
    std::optional<ArgumentInfo> findArgumentInfo(const std::string& argumentName, const std::vector<ArgumentInfo> &argumentInfos);

    std::unordered_map<std::string, VariableInfo> mainVariableTable_;
    std::unordered_map<std::string, ProcedureInfo> procedureTable_;
    long long currentAddress_;
    CompilationError compilationError_;
    bool shouldReturnErrorInstantly_;
};

#endif // SYMBOL_TABLE_HPP
