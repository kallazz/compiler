#include "SymbolTable.hpp"

#include <cmath>

constexpr long long FIRST_AVAILABLE_ADDRESS = 13;
const long long MAX_AVAILABLE_ADDRESS = pow(2, 62);

SymbolTable::SymbolTable() : currentAvailableAddress_(FIRST_AVAILABLE_ADDRESS), shouldReturnErrorInstantly_(false) {}

bool SymbolTable::declareGlobalConstant(const int lineNumber, const long long value) {
    const std::string constantName = "!" + std::to_string(value);

    if ((globalConstantTable_.find(constantName) == globalConstantTable_.end())) {
        if (!checkIfMemoryLimitWontGetExceeded(lineNumber, 1)) {
            return false;
        }

        globalConstantTable_.insert({constantName, {currentAvailableAddress_, value}});
        currentAvailableAddress_++;
    }

    return true;
}

bool SymbolTable::declareNumberVariableInMain(const int lineNumber, const std::string& name, const bool isIterator,
                                              const bool isInitialized) {
    return declareNumberVariable(lineNumber, name, mainVariableTable_, std::nullopt, isIterator, isInitialized);
}

bool SymbolTable::declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound,
                                             const long long upperBound) {
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, mainVariableTable_, std::nullopt);
}

bool SymbolTable::checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name,
                                                    const bool willVariableBeModified) {
    return checkIfNumberVariableExists(lineNumber, name, mainVariableTable_, willVariableBeModified);
}

bool SymbolTable::checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name,
                                                   const std::optional<long long> arrayIndex) {
    return checkIfArrayVariableExists(lineNumber, name, arrayIndex, mainVariableTable_);
}

bool SymbolTable::declareNumberVariableInProcedure(const int lineNumber, const std::string& name,
                                                   const std::string& procedureName, const bool isIterator,
                                                   const bool isInitialized) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    return declareNumberVariable(lineNumber, name, procedureVariableTable, procedureArgumentInfos, isIterator,
                                 isInitialized);
}

bool SymbolTable::declareArrayVariableInProcedure(const int lineNumber, const std::string& name,
                                                  const long long lowerBound, const long long upperBound,
                                                  const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, procedureVariableTable,
                                procedureArgumentInfos);
}

bool SymbolTable::checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name,
                                                         const bool willVariableBeModified,
                                                         const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    if (checkIfNumberVariableExists(lineNumber, name, procedureVariableTable, willVariableBeModified)) {
        return true;
    }
    if (shouldReturnErrorInstantly_) {
        return false;
    }
    return checkIfNumberVariableExistsInProcedureArguments(lineNumber, name, procedureArgumentInfos);
}

bool SymbolTable::checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name,
                                                        const std::optional<long long> arrayIndex,
                                                        const std::string& procedureName) {
    const auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    if (checkIfArrayVariableExists(lineNumber, name, arrayIndex, procedureVariableTable)) {
        return true;
    }
    if (shouldReturnErrorInstantly_) {
        return false;
    }
    return checkIfArrayVariableExistsInProcedureArguments(lineNumber, name, procedureArgumentInfos);
}

bool SymbolTable::declareProcedure(const int lineNumber, const std::string& name,
                                   const std::vector<ArgumentDeclaration>& argumentDeclarations) {
    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"redeclaration of procedure `" + name + "`.", lineNumber};
        return false;
    }

    const long long amountOfAddressesToReserve = argumentDeclarations.size() + 1;
    if (!checkIfMemoryLimitWontGetExceeded(lineNumber, amountOfAddressesToReserve)) {
        return false;
    }

    std::vector<ArgumentInfo> argumentInfos;
    for (const auto& argumentDeclaration : argumentDeclarations) {
        if (argumentDeclaration.name == name) {
            compilationError_ = {"argument with the same name as procedure `" + name + "`.",
                                 argumentDeclaration.lineNumber};
            return false;
        }

        if (findArgumentInfo(argumentDeclaration.name, argumentInfos)) {
            compilationError_ = {"redeclaration of argument `" + argumentDeclaration.name + "`.", lineNumber};
            return false;
        }

        argumentInfos.emplace_back(
            ArgumentInfo{argumentDeclaration.name, argumentDeclaration.argumentType, currentAvailableAddress_});
        currentAvailableAddress_++;
    }

    procedureTable_.insert(
        {name, {argumentInfos, std::unordered_map<std::string, VariableInfo>(), currentAvailableAddress_}});
    currentAvailableAddress_++;

    return true;
}

bool SymbolTable::verifyProcedureCall(const int lineNumber, const std::string& name,
                                      const std::vector<Argument>& arguments,
                                      const std::optional<std::string> scopeProcedureName) {
    const auto it = procedureTable_.find(name);
    if (it == procedureTable_.end() || name == scopeProcedureName) {
        compilationError_ = {"procedure `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& argumentInfos = it->second.argumentInfos;
    const size_t actualNumberOfArguments = arguments.size();
    const size_t expectedNumberOfArguments = argumentInfos.size();

    if (actualNumberOfArguments < expectedNumberOfArguments) {
        compilationError_ = {"not enough arguments provided to `" + name +
                                 "`. Expected: " + std::to_string(expectedNumberOfArguments) +
                                 ", Got: " + std::to_string(actualNumberOfArguments) + ".",
                             lineNumber};
        return false;
    }
    if (actualNumberOfArguments > expectedNumberOfArguments) {
        compilationError_ = {"too many arguments provided to `" + name +
                                 "`. Expected: " + std::to_string(expectedNumberOfArguments) +
                                 ", Got: " + std::to_string(actualNumberOfArguments) + ".",
                             lineNumber};
        return false;
    }

    int i = 0;
    for (const auto& argumentInfo : argumentInfos) {
        const std::optional<std::pair<bool, ArgumentType>> isIteratorAndArgumentType =
            checkIfVariableExistsAndSetItAsInitializedAndGetItsInfo(arguments[i].name, scopeProcedureName);
        if (!isIteratorAndArgumentType) {
            compilationError_ = {"variable `" + arguments[i].name + "` was not declared.", arguments[i].lineNumber};
            return false;
        }

        const auto& [isIterator, argumentType] = *isIteratorAndArgumentType;

        if (isIterator) {
            compilationError_ = {"iterator variable `" + arguments[i].name + "` cannot be passed to procedure.",
                                 arguments[i].lineNumber};
            return false;
        }

        if (argumentType != argumentInfo.type) {
            compilationError_ = {"argument `" + arguments[i].name + "` has incorrect type. Expected: " +
                                     toString(argumentInfo.type) + ", Got: " + toString(argumentType) + ".",
                                 arguments[i].lineNumber};
            return false;
        }

        i++;
    }

    return true;
}

void SymbolTable::renameVariableInMain(const std::string& name, const std::string& newName) {
    VariableInfo variableInfo = mainVariableTable_.at(name);
    mainVariableTable_.erase(name);
    mainVariableTable_.insert({newName, variableInfo});
}

void SymbolTable::renameVariableInProcedure(const std::string& name, const std::string& newName,
                                            const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    VariableInfo variableInfo = procedureVariableTable.at(name);
    procedureVariableTable.erase(name);
    procedureVariableTable.insert({newName, variableInfo});
}

std::optional<long long> SymbolTable::getGlobalConstantAddress(const long long value) const {
    const std::string constantName = "!" + std::to_string(value);

    const auto it = globalConstantTable_.find(constantName);
    if (it != globalConstantTable_.end()) {
        return it->second.address;
    }

    return std::nullopt;
}

long long SymbolTable::getVariableAddressInMain(const std::string& name) const {
    return mainVariableTable_.at(name).address;
}

std::pair<long long, bool> SymbolTable::getVariableAddressInProcedure(const std::string& name,
                                                                      const std::string& procedureName) const {
    const auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;

    const auto it = procedureVariableTable.find(name);
    if (it != procedureVariableTable.end()) {
        const bool isPointer = false;
        return {it->second.address, isPointer};
    }

    const bool isPointer = true;
    return {(*findArgumentInfo(name, procedureArgumentInfos)).address, isPointer};
}

std::vector<long long> SymbolTable::getProcedureArgumentsAddresses(const std::string& procedureName) const {
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    std::vector<long long> argumentsAddresses;

    for (const auto& argumentInfo : procedureArgumentInfos) {
        argumentsAddresses.push_back(argumentInfo.address);
    }

    return argumentsAddresses;
}

long long SymbolTable::getProcedureReturnAddress(const std::string& procedureName) const {
    return procedureTable_.at(procedureName).returnAddress;
}

std::vector<ConstantInfo> SymbolTable::getGlobalConstantInfos() const {
    std::vector<ConstantInfo> constantInfos;
    for (const auto& [_, constantInfo] : globalConstantTable_) {
        constantInfos.push_back(constantInfo);
    }

    return constantInfos;
}

bool SymbolTable::declareNumberVariable(
    const int lineNumber, const std::string& name, std::unordered_map<std::string, VariableInfo>& variableTable,
    const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos, const bool isIterator,
    const bool isInitialized) {
    if ((variableTable.find(name) != variableTable.end()) ||
        (argumentInfos && findArgumentInfo(name, *argumentInfos))) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    if (!checkIfMemoryLimitWontGetExceeded(lineNumber, 1)) {
        return false;
    }

    variableTable.insert({name, {currentAvailableAddress_, std::nullopt, isIterator, isInitialized}});
    currentAvailableAddress_++;

    return true;
}

bool SymbolTable::declareArrayVariable(
    const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound,
    std::unordered_map<std::string, VariableInfo>& variableTable,
    const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos) {
    if ((variableTable.find(name) != variableTable.end()) ||
        (argumentInfos && findArgumentInfo(name, *argumentInfos))) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    if (lowerBound > upperBound) {
        compilationError_ = {"incorrect array range in `" + name + "`. " + std::to_string(lowerBound) +
                                 " is bigger than " + std::to_string(upperBound) + ".",
                             lineNumber};
        return false;
    }

    const long long arraySize = upperBound - lowerBound + 1;
    const long long zeroAddress = currentAvailableAddress_ - lowerBound;

    if (!checkIfMemoryLimitWontGetExceeded(lineNumber, arraySize)) {
        return false;
    }

    variableTable.insert({name, {zeroAddress, std::make_pair(lowerBound, upperBound)}});
    currentAvailableAddress_ += arraySize;

    return true;
}

bool SymbolTable::checkIfNumberVariableExists(const int lineNumber, const std::string& name,
                                              std::unordered_map<std::string, VariableInfo>& variableTable,
                                              const bool willVariableBeModified) {
    auto it = variableTable.find(name);
    if (it == variableTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    VariableInfo& variableInfo = it->second;
    if (variableInfo.arrayRange) {
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used when accessing it.", lineNumber};
        shouldReturnErrorInstantly_ = true;
        return false;
    }

    if (variableInfo.isIterator && willVariableBeModified) {
        compilationError_ = {"modification of iterator variable `" + name + "` is not allowed.", lineNumber};
        shouldReturnErrorInstantly_ = true;
        return false;
    }

    if (!variableInfo.isInitialized && !willVariableBeModified) {
        compilationError_ = {"variable `" + name + "` was not initialized.", lineNumber};
        shouldReturnErrorInstantly_ = true;
        return false;
    }

    if (willVariableBeModified) {
        variableInfo.isInitialized = true;
    }

    return true;
}

bool SymbolTable::checkIfArrayVariableExists(const int lineNumber, const std::string& name,
                                             const std::optional<long long> arrayIndex,
                                             const std::unordered_map<std::string, VariableInfo>& variableTable) {
    const auto it = variableTable.find(name);
    if (it == variableTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const VariableInfo& variableInfo = it->second;
    if (!variableInfo.arrayRange) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used when accessing it.",
                             lineNumber};
        return false;
    }

    if (arrayIndex) {
        const auto& [lowerBound, upperBound] = *variableInfo.arrayRange;
        if (*arrayIndex < lowerBound || *arrayIndex > upperBound) {
            shouldReturnErrorInstantly_ = true;
            compilationError_ = {"array index out of bounds for variable `" + name +
                                     "`. Index: " + std::to_string(*arrayIndex) + ", Range: [" +
                                     std::to_string(lowerBound) + ", " + std::to_string(upperBound) + "].",
                                 lineNumber};
            return false;
        }
    }

    return true;
}

bool SymbolTable::checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name,
                                                                  const std::vector<ArgumentInfo>& argumentInfos) {
    const auto argumentInfo = findArgumentInfo(name, argumentInfos);
    if (!argumentInfo) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    if ((*argumentInfo).type == ArgumentType::ARRAY) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used when accessing it.", lineNumber};
        return false;
    }

    return true;
}

bool SymbolTable::checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name,
                                                                 const std::vector<ArgumentInfo>& argumentInfos) {
    const auto argumentInfo = findArgumentInfo(name, argumentInfos);
    if (!argumentInfo) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    if ((*argumentInfo).type != ArgumentType::ARRAY) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used when accessing it.",
                             lineNumber};
        return false;
    }

    return true;
}

std::optional<std::pair<bool, ArgumentType>> SymbolTable::checkIfVariableExistsAndSetItAsInitializedAndGetItsInfo(
    const std::string& name, const std::optional<std::string> scopeProcedureName) {
    if (!scopeProcedureName) {
        const auto it = mainVariableTable_.find(name);
        if (it == mainVariableTable_.end()) {
            return std::nullopt;
        }

        it->second.isInitialized = true;

        const bool isIterator = it->second.isIterator;
        const ArgumentType argumentType = (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
        return std::make_pair(isIterator, argumentType);
    }

    auto& scopeProcedureVariableTable = procedureTable_.at(*scopeProcedureName).variableTable;
    const auto& scopeProcedureArgumentInfos = procedureTable_.at(*scopeProcedureName).argumentInfos;

    const auto it = scopeProcedureVariableTable.find(name);
    if (it != scopeProcedureVariableTable.end()) {
        it->second.isInitialized = true;

        const bool isIterator = it->second.isIterator;
        const ArgumentType argumentType = (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
        return std::make_pair(isIterator, argumentType);
    }

    const auto argumentInfo = findArgumentInfo(name, scopeProcedureArgumentInfos);
    if (!argumentInfo) {
        return std::nullopt;
    }
    return std::make_pair(false, (*argumentInfo).type);
}

std::optional<ArgumentInfo> SymbolTable::findArgumentInfo(const std::string& argumentName,
                                                          const std::vector<ArgumentInfo>& argumentInfos) const {
    for (const auto& argumentInfo : argumentInfos) {
        if (argumentName == argumentInfo.name) {
            return argumentInfo;
        }
    }

    return std::nullopt;
}

bool SymbolTable::checkIfMemoryLimitWontGetExceeded(const int lineNumber, const long long amountOfAdressesToReserve) {
    const long long amountOfAvailableAddresses = MAX_AVAILABLE_ADDRESS - currentAvailableAddress_;
    if (amountOfAdressesToReserve > amountOfAvailableAddresses) {
        compilationError_ = {"memory limit exceeded.", lineNumber};
        return false;
    }
    return true;
}

CompilationError SymbolTable::getCompilationError() const {
    return compilationError_;
}
