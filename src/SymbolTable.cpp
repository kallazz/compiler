#include "SymbolTable.hpp"

constexpr long long FIRST_AVAILABLE_ADDRESS = 1;

SymbolTable::SymbolTable() : currentAddress_(FIRST_AVAILABLE_ADDRESS) {}

bool SymbolTable::declareNumberVariableInMain(const int lineNumber, const std::string& name) {
    return declareNumberVariable(lineNumber, name, mainVariableTable_, std::nullopt);
}

bool SymbolTable::declareArrayVariableInMain(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound) {
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, mainVariableTable_, std::nullopt);
}

bool SymbolTable::checkIfNumberVariableExistsInMain(const int lineNumber, const std::string& name) {
    return checkIfNumberVariableExists(lineNumber, name, mainVariableTable_);
}

bool SymbolTable::checkIfArrayVariableExistsInMain(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex) {
    return checkIfArrayVariableExists(lineNumber, name, arrayIndex, mainVariableTable_);
}

bool SymbolTable::declareNumberVariableInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentTable = procedureTable_.at(procedureName).argumentTable;
    return declareNumberVariable(lineNumber, name, procedureVariableTable, procedureArgumentTable);
}

bool SymbolTable::declareArrayVariableInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const std::string& procedureName) {
    auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentTable = procedureTable_.at(procedureName).argumentTable;
    return declareArrayVariable(lineNumber, name, lowerBound, upperBound, procedureVariableTable, procedureArgumentTable);
}

bool SymbolTable::checkIfNumberVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::string& procedureName) {
    const auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentTable = procedureTable_.at(procedureName).argumentTable;
    return checkIfNumberVariableExists(lineNumber, name, procedureVariableTable) || checkIfNumberVariableExistsInProcedureArguments(lineNumber, name, procedureArgumentTable);
}

bool SymbolTable::checkIfArrayVariableExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const std::string& procedureName) {
    const auto& procedureVariableTable = procedureTable_.at(procedureName).variableTable;
    const auto& procedureArgumentTable = procedureTable_.at(procedureName).argumentTable;
    return checkIfArrayVariableExists(lineNumber, name, arrayIndex, procedureVariableTable) || checkIfArrayVariableExistsInProcedureArguments(lineNumber, name, procedureArgumentTable);
}


bool SymbolTable::declareProcedure(const int lineNumber, const std::string& name, const std::vector<ArgumentDeclaration>& argumentDeclarations) {
    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"redeclaration of procedure `" + name + "`.", lineNumber};
        return false;
    }

    std::map<std::string, std::pair<ArgumentType, long long>> argumentTable;
    for (const auto& argumentDeclaration : argumentDeclarations) {
        if (argumentDeclaration.name == name) {
            compilationError_ = {"argument with the same name as procedure `" + name + "`.", argumentDeclaration.lineNumber};
            return false;
        }

        if (argumentTable.find(argumentDeclaration.name) != argumentTable.end()) {
            compilationError_ = {"redeclaration of argument `" + argumentDeclaration.name + "`.", lineNumber};
            return false;
        }

        argumentTable.insert({argumentDeclaration.name, {argumentDeclaration.argumentType, currentAddress_}});
        currentAddress_++;
    }

    procedureTable_.insert({name, {argumentTable, std::map<std::string, VariableInfo>()}});

    return true;
}

bool SymbolTable::checkIfProcedureExists(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments, const std::optional<std::string> scopeProcedureName) {
    const auto it = procedureTable_.find(name);
    if (it == procedureTable_.end()) {
        compilationError_ = {"procedure `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& argumentTable = it->second.argumentTable;
    const size_t actualNumberOfArguments = arguments.size();
    const size_t expectedNumberOfArguments = argumentTable.size();

    if (actualNumberOfArguments < expectedNumberOfArguments) {
        compilationError_ = {"not enough arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }
    if (actualNumberOfArguments > expectedNumberOfArguments) {
        compilationError_ = {"too many arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }

    int i = 0;
    for (const auto& keyAndValue : argumentTable) {
        const auto& [expectedArgumentType, _] = keyAndValue.second;

        const std::optional actualArgumentType = checkIfVariableExistsAndGetItsType(arguments[i].name, scopeProcedureName);
        if (!actualArgumentType) {
            compilationError_ = {"variable `" + arguments[i].name + "` was not declared.", arguments[i].lineNumber};
            return false;
        }

        if (*actualArgumentType != expectedArgumentType) {
            compilationError_ = {"argument `" + arguments[i].name + "` has incorrect type. Expected: " + toString(expectedArgumentType) + ", Got: " + toString(*actualArgumentType) + "." , arguments[i].lineNumber};
            return false;
        }

        i++;
    }

    return true;
}

bool SymbolTable::declareNumberVariable(const int lineNumber, const std::string& name, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::map<std::string, std::pair<ArgumentType, long long>>>> argumentTable) {
    if ((variableTable.find(name) != variableTable.end()) || (argumentTable && (argumentTable->get().find(name) != argumentTable->get().end()))) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    variableTable.insert({name, {currentAddress_, std::nullopt}});
    currentAddress_++;

    return true;
}

bool SymbolTable::declareArrayVariable(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, std::map<std::string, VariableInfo>& variableTable, const std::optional<std::reference_wrapper<const std::map<std::string, std::pair<ArgumentType, long long>>>> argumentTable) {
    if ((variableTable.find(name) != variableTable.end()) || (argumentTable && (argumentTable->get().find(name) != argumentTable->get().end()))) {
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

bool SymbolTable::checkIfNumberVariableExists(const int lineNumber, const std::string& name, const std::map<std::string, VariableInfo>& variableTable) {
    const auto it = variableTable.find(name);
    if (it == variableTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const VariableInfo& variableInfo = it->second;
    if (variableInfo.arrayRange) {
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used in assignment.", lineNumber};
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
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used in assignment.", lineNumber};
        return false;
    }

    if (arrayIndex) {
        const auto& [lowerBound, upperBound] = *variableInfo.arrayRange;
        if (*arrayIndex < lowerBound || *arrayIndex > upperBound) {
            compilationError_ = {
                "array index out of bounds for variable `" + name + "`. Index: " + std::to_string(*arrayIndex) + ", Range: [" + std::to_string(lowerBound) + ", " + std::to_string(upperBound) + "].",
                lineNumber
            };
            return false;
        }
    }

    return true;
}

bool SymbolTable::checkIfNumberVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::map<std::string, std::pair<ArgumentType, long long>>& argumentTable) {

    const auto it = argumentTable.find(name);
    if (it == argumentTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& [argumentType, _] = it->second;
    if (argumentType == ArgumentType::ARRAY) {
        compilationError_ = {"variable `" + name + "` is an array. [] need to be used in assignment.", lineNumber};
        return false;
    }

    return true;
}

bool SymbolTable::checkIfArrayVariableExistsInProcedureArguments(const int lineNumber, const std::string& name, const std::map<std::string, std::pair<ArgumentType, long long>>& argumentTable) {
    const auto it = argumentTable.find(name);
    if (it == argumentTable.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& [argumentType, _] = it->second;
    if (argumentType != ArgumentType::ARRAY) {
        compilationError_ = {"variable `" + name + "` is not an array. [] cannot be used in assignment.", lineNumber};
        return false;
    }

    return true;
}

std::optional<ArgumentType> SymbolTable::checkIfVariableExistsAndGetItsType(const std::string& name, const std::optional<std::string> scopeProcedureName) {
    if (!scopeProcedureName) {
        const auto it = mainVariableTable_.find(name);
        if (it == mainVariableTable_.end()) {
            return std::nullopt;
        }
        return (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
    }

    const auto& scopeProcedureVariableTable = procedureTable_.at(*scopeProcedureName).variableTable;
    const auto& scopeProcedureArgumentTable = procedureTable_.at(*scopeProcedureName).argumentTable;

    const auto it = scopeProcedureVariableTable.find(name);
    if (it != scopeProcedureVariableTable.end()) {
        return (it->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;    
    }

    const auto it2 = scopeProcedureArgumentTable.find(name);
    if (it2 == scopeProcedureArgumentTable.end()) {
        return std::nullopt;
    }
    return it2->second.first;
}

CompilationError SymbolTable::getCompilationError() const {
    return compilationError_;
}
