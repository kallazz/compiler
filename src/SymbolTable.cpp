#include "SymbolTable.hpp"

constexpr long long FIRST_AVAILABLE_ADDRESS = 1;

SymbolTable::SymbolTable() : currentAddress_(FIRST_AVAILABLE_ADDRESS), shouldReturnErrorInstantly_(false) {}

bool SymbolTable::declareNumberVariableInMain(const int lineNumber, const std::string& name, const bool isIterator) {
    return declareNumberVariable(lineNumber, name, mainVariableTable_, std::nullopt, isIterator);
}

bool SymbolTable::declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound) {
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, mainVariableTable_, std::nullopt);
}

bool SymbolTable::checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name, const bool willVariableBeModified) {
    return checkIfNumberVariableExists(lineNumber, name, mainVariableTable_, willVariableBeModified);
}

bool SymbolTable::checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex) {
    return checkIfArrayVariableExists(lineNumber, name, arrayIndex, mainVariableTable_);
}

bool SymbolTable::declareNumberVariableInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName, const bool isIterator) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    return declareNumberVariable(lineNumber, name, procedureVariableTable, procedureArgumentInfos, isIterator);
}

bool SymbolTable::declareArrayVariableInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, procedureVariableTable, procedureArgumentInfos);
}

bool SymbolTable::checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name, const bool willVariableBeMofidied, const std::string& procedureName) {
    const auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentInfos = procedureTable_.at(procedureName).argumentInfos;
    if (checkIfNumberVariableExists(lineNumber, name, procedureVariableTable, willVariableBeMofidied)) {
        return true;
    }
    if (shouldReturnErrorInstantly_) {
        return false;
    }
    return checkIfNumberVariableExistsInProcedureArguments(lineNumber, name, procedureArgumentInfos);
}

bool SymbolTable::checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::string& procedureName) {
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


bool SymbolTable::declareProcedure(const int lineNumber, const std::string& name, const std::vector<ArgumentDeclaration>& argumentDeclarations) {
    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"redeclaration of procedure `" + name + "`.", lineNumber};
        return false;
    }

    std::vector<ArgumentInfo> argumentInfos;
    for (const auto& argumentDeclaration : argumentDeclarations) {
        if (argumentDeclaration.name == name) {
            compilationError_ = {"argument with the same name as procedure `" + name + "`.", argumentDeclaration.lineNumber};
            return false;
        }

        if (findArgumentInfo(argumentDeclaration.name, argumentInfos)) {
            compilationError_ = {"redeclaration of argument `" + argumentDeclaration.name + "`.", lineNumber};
            return false;
        }

        argumentInfos.emplace_back(ArgumentInfo{argumentDeclaration.name, argumentDeclaration.argumentType, currentAddress_});
        currentAddress_++;
    }

    procedureTable_.insert({name, {argumentInfos, std::map<std::string, VariableInfo>()}});

    return true;
}

bool SymbolTable::verifyProcedureCall(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments, const std::optional<std::string> scopeProcedureName) {
    const auto it = procedureTable_.find(name);
    if (it == procedureTable_.end()) {
        compilationError_ = {"procedure `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& argumentInfos = it->second.argumentInfos;
    const size_t actualNumberOfArguments = arguments.size();
    const size_t expectedNumberOfArguments = argumentInfos.size();

    if (actualNumberOfArguments < expectedNumberOfArguments) {
        compilationError_ = {"not enough arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }
    if (actualNumberOfArguments > expectedNumberOfArguments) {
        compilationError_ = {"too many arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }

    int i = 0;
    for (const auto& argumentInfo : argumentInfos) {
        const std::optional<std::pair<bool, ArgumentType>> isIteratorAndArgumentType = checkIfVariableExistsAndGetItsInfo(arguments[i].name, scopeProcedureName);
        if (!isIteratorAndArgumentType) {
            compilationError_ = {"variable `" + arguments[i].name + "` was not declared.", arguments[i].lineNumber};
            return false;
        }

        const auto& [isIterator, argumentType] = *isIteratorAndArgumentType;

        if (isIterator) {
            compilationError_ = {"iterator variable `" + arguments[i].name + "` cannot be passed to procedure.", arguments[i].lineNumber};
            return false;
        }


        if (argumentType != argumentInfo.type) {
            compilationError_ = {"argument `" + arguments[i].name + "` has incorrect type. Expected: " + toString(argumentInfo.type) + ", Got: " + toString(argumentType) + "." , arguments[i].lineNumber};
            return false;
        }

        i++;
    }

    return true;
}

void SymbolTable::removeVariableFromMain(const std::string& name) {
    mainVariableTable_.erase(name);
}

void SymbolTable::removeVariableFromProcedure(const std::string& name, const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    procedureVariableTable.erase(name);
}

bool SymbolTable::declareNumberVariable(const int lineNumber, const std::string& name, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos, const bool isIterator) {
    if ((variableTable.find(name) != variableTable.end()) || (argumentInfos && findArgumentInfo(name, *argumentInfos))) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    variableTable.insert({name, {currentAddress_, std::nullopt, isIterator}});
    currentAddress_++;

    return true;
}

bool SymbolTable::declareArrayVariable(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::vector<ArgumentInfo>>> argumentInfos) {
    if ((variableTable.find(name) != variableTable.end()) || (argumentInfos && findArgumentInfo(name, *argumentInfos))) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    if (lowerBound > upperBound) {
        compilationError_ = {"incorrect array range in `" + name + "`. " + std::to_string(lowerBound) + " is bigger than " + std::to_string(upperBound) + ".", lineNumber};
        return false;
    }

    const long long arraySize = upperBound - lowerBound + 1;

    variableTable.insert({name, {currentAddress_, std::make_pair(lowerBound, upperBound)}});
    currentAddress_ += arraySize;

    return true;
}

bool SymbolTable::checkIfNumberVariableExists(const int lineNumber, const std::string& name, const std::map<std::string, VariableInfo>& variableTable, const bool willVariableBeModified) {
    const auto it = variableTable.find(name);
    if (it == variableTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const VariableInfo& variableInfo = it->second;
    if (variableInfo.arrayRange) {
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used in assignment.", lineNumber};
        shouldReturnErrorInstantly_ = true;
        return false;
    }

    if (variableInfo.isIterator && willVariableBeModified) {
        compilationError_ = {"modification of iterator variable `" + name + "` is not allowed.", lineNumber};
        shouldReturnErrorInstantly_ = true;
        return false;
    }

    return true;
}

bool SymbolTable::checkIfArrayVariableExists(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::map<std::string, VariableInfo>& variableTable) {
    const auto it = variableTable.find(name);
    if (it == variableTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const VariableInfo& variableInfo = it->second;
    if (!variableInfo.arrayRange) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used in assignment.", lineNumber};
        return false;
    }

    if (arrayIndex) {
        const auto& [lowerBound, upperBound] = *variableInfo.arrayRange;
        if (*arrayIndex < lowerBound || *arrayIndex > upperBound) {
            shouldReturnErrorInstantly_ = true;
            compilationError_ = {
                "array index out of bounds for variable `" + name + "`. Index: " + std::to_string(*arrayIndex) + ", Range: [" + std::to_string(lowerBound) + ", " + std::to_string(upperBound) + "].",
                lineNumber
            };
            return false;
        }
    }

    return true;
}

bool SymbolTable::checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::vector<ArgumentInfo>& argumentInfos) {
    const auto argumentInfo = findArgumentInfo(name, argumentInfos);
    if (!argumentInfo) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    if ((*argumentInfo).type == ArgumentType::ARRAY) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used in assignment.", lineNumber};
        return false;
    }

    return true;
}

bool SymbolTable::checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::vector<ArgumentInfo>& argumentInfos) {
    const auto argumentInfo = findArgumentInfo(name, argumentInfos);
    if (!argumentInfo ) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    if ((*argumentInfo).type != ArgumentType::ARRAY) {
        shouldReturnErrorInstantly_ = true;
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used in assignment.", lineNumber};
        return false;
    }

    return true;
}

std::optional<std::pair<bool, ArgumentType>> SymbolTable::checkIfVariableExistsAndGetItsInfo(const std::string& name, const std::optional<std::string> scopeProcedureName) {
    if (!scopeProcedureName) {
        const auto it = mainVariableTable_.find(name);
        if (it == mainVariableTable_.end()) {
            return std::nullopt;
        }

        const bool isIterator = it->second.isIterator;
        const ArgumentType argumentType = (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
        return std::make_pair(isIterator, argumentType);
    }

    const auto& scopeProcedureVariableTable = procedureTable_.at(*scopeProcedureName).variableTable;
    const auto& scopeProcedureArgumentInfos = procedureTable_.at(*scopeProcedureName).argumentInfos;

    const auto it = scopeProcedureVariableTable.find(name);
    if (it != scopeProcedureVariableTable.end()) {
        const bool isIterator = it->second.isIterator;
        const ArgumentType argumentType = (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
        return std::make_pair(isIterator, argumentType);
    }

    const auto argumentInfo = findArgumentInfo(name, scopeProcedureArgumentInfos);
    if (!argumentInfo ) {
        return std::nullopt;
    }
    return std::make_pair(false, (*argumentInfo).type);
}

std::optional<ArgumentInfo> SymbolTable::findArgumentInfo(const std::string& argumentName, const std::vector<ArgumentInfo> &argumentInfos) {
    for (const auto& argumentInfo : argumentInfos) {
        if (argumentName == argumentInfo.name) {
            return argumentInfo;
        }
    }

    return std::nullopt;
}

CompilationError SymbolTable::getCompilationError() const {
    return compilationError_;
}
