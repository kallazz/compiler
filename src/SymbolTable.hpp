#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "types/Argument.hpp"
#include "types/ArgumentDeclaration.hpp"
#include "types/CompilationError.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct VariableInfo {
    const long long address;
    const std::optional<std::pair<long long, long long>> arrayRange;
    const bool isIterator = false;
    bool isInitialized = false;
};

struct ConstantInfo {
    const long long address;
    const long long value;
};

struct ArgumentInfo {
    const std::string name;
    const ArgumentType type;
    const long long address;
};

struct ProcedureInfo {
    const std::vector<ArgumentInfo> argumentInfos;
    std::unordered_map<std::string, VariableInfo> variableTable;
    const long long returnAddress;
};

class SymbolTable {
public:
    SymbolTable();

    bool declareGlobalConstant(const int lineNumber, const long long value);

    bool declareNumberVariableInMain(const int lineNumber, const std::string& name, const bool isIterator = false,
                                     const bool isInitialized = false);
    bool declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound,
                                    const long long upperBound);
    bool checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name,
                                           const bool willVariableBeModified);
    bool checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name,
                                          const std::optional<long long> arrayIndex);

    bool declareNumberVariableInProcedure(const int lineNumber, const std::string& name,
                                          const std::string& procedureName, const bool isIterator = false,
                                          const bool isInitialized = false);
    bool declareArrayVariableInProcedure(const int lineNumber, const std::string& name, const long long lowerBound,
                                         const long long upperBound, const std::string& procedureName);
    bool checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name,
                                                const bool willVariableBeModified, const std::string& procedureName);
    bool checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name,
                                               const std::optional<long long> arrayIndex,
                                               const std::string& procedureName);

    bool declareProcedure(const int lineNumber, const std::string& name,
                          const std::vector<ArgumentDeclaration>& argumentDeclarations);
    bool verifyProcedureCall(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments,
                             const std::optional<std::string> scopeProcedureName);

    void renameVariableInMain(const std::string& name, const std::string& newName);
    void renameVariableInProcedure(const std::string& name, const std::string& newName,
                                   const std::string& procedureName);

    long long getGlobalConstantAddress(const long long value) const;
    long long getVariableAddressInMain(const std::string& name) const;
    std::pair<long long, bool> getVariableAddressInProcedure(const std::string& name,
                                                             const std::string& procedureName) const;
    std::vector<long long> getProcedureArgumentsAddresses(const std::string& procedureName) const;
    long long getProcedureReturnAddress(const std::string& procedureName) const;

    bool checkIfGlobalConstantExists(const long long value) const;

    std::vector<ConstantInfo> getGlobalConstantInfos() const;
    CompilationError getCompilationError() const;

private:
    bool declareNumberVariable(
        const int lineNumber, const std::string& name, std::unordered_map<std::string, VariableInfo>& variableTable,
        const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos,
        const bool isIterator, const bool isInitialized);
    bool declareArrayVariable(
        const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound,
        std::unordered_map<std::string, VariableInfo>& variableTable,
        const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos);
    bool checkIfNumberVariableExists(const int lineNumber, const std::string& name,
                                     std::unordered_map<std::string, VariableInfo>& variableTable,
                                     const bool willVariableBeModified);
    bool checkIfArrayVariableExists(const int lineNumber, const std::string& name,
                                    const std::optional<long long> arrayIndex,
                                    const std::unordered_map<std::string, VariableInfo>& variableTable);
    bool checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name,
                                                         const std::vector<ArgumentInfo>& argumentInfos);
    bool checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name,
                                                        const std::vector<ArgumentInfo>& argumentInfos);
    std::optional<std::pair<bool, ArgumentType>> checkIfVariableExistsAndSetItAsInitializedAndGetItsInfo(
        const std::string& name, const std::optional<std::string> scopeProcedureName);
    std::optional<ArgumentInfo> findArgumentInfo(const std::string& argumentName,
                                                 const std::vector<ArgumentInfo>& argumentInfos) const;
    bool checkIfMemoryLimitWontGetExceeded(const int lineNumber, const long long amountOfAdressesToReserve);

    std::unordered_map<std::string, VariableInfo> mainVariableTable_;
    std::unordered_map<std::string, ProcedureInfo> procedureTable_;
    std::unordered_map<std::string, ConstantInfo> globalConstantTable_;
    long long currentAvailableAddress_;
    CompilationError compilationError_;
    bool shouldReturnErrorInstantly_;
};

#endif  // SYMBOL_TABLE_HPP
