#include "SymbolTable.hpp"

constexpr long long FIRST_AVAILABLE_ADDRESS = 1;

SymbolTable::SymbolTable() : currentAddress_(FIRST_AVAILABLE_ADDRESS) {}

bool SymbolTable::declareVariable(const int lineNumber, const std::string& name) {
    return declareVariableWithNamePrefix(lineNumber, name, "");
}

bool SymbolTable::declareArray(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound) {
    return declareArrayWithNamePrefix(lineNumber, name, "", lowerBound, upperBound);
}

bool SymbolTable::checkIfVariableExists(const int lineNumber, const std::string& name) {
    return checkIfVariableExistsWithNamePrefix(lineNumber, name, "");
}

bool SymbolTable::checkIfArrayExists(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex) {
    return checkIfArrayExistsWithNamePrefix(lineNumber, name, "", arrayIndex);
}

bool SymbolTable::checkIfVariableOrArrayExists(const int lineNumber, const std::string& name) {
    return checkIfVariableOrArrayExistsWithNamePrefix(lineNumber, name, "");
}

bool SymbolTable::declareVariableInProcedure(const int lineNumber, const std::string& name, const int procedureIndex) {
    return declareVariableWithNamePrefix(lineNumber, name, std::to_string(procedureIndex));
}

bool SymbolTable::declareArrayInProcedure(const int lineNumber, const std::string& name, const long long lowerBound, const long long upperBound, const int procedureIndex) {
    return declareArrayWithNamePrefix(lineNumber, name, std::to_string(procedureIndex), lowerBound, upperBound);
}

bool SymbolTable::checkIfVariableExistsInProcedure(const int lineNumber, const std::string& name, const int procedureIndex) {
    return checkIfVariableExistsWithNamePrefix(lineNumber, name, std::to_string(procedureIndex));
}

bool SymbolTable::checkIfArrayExistsInProcedure(const int lineNumber, const std::string& name, const std::optional<long long> arrayIndex, const int procedureIndex) {
    return checkIfArrayExistsWithNamePrefix(lineNumber, name, std::to_string(procedureIndex), arrayIndex);
}

bool SymbolTable::checkIfVariableOrArrayExistsInProcedure(const int lineNumber, const std::string& name, const int procedureIndex) {
    return checkIfVariableOrArrayExistsWithNamePrefix(lineNumber, name, std::to_string(procedureIndex));
}

bool SymbolTable::declareProcedure(const int lineNumber, const std::string& name, const std::vector<ArgumentDeclaration>& argumentDeclarations, const int procedureIndex) {
    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"redeclaration of procedure `" + name + "`.", lineNumber};
        return false;
    }

    std::map<std::string, std::pair<ArgumentType, long long>> argumentNameToTypeAndAddress;
    for (const auto& argumentDeclaration : argumentDeclarations) {
        if (argumentDeclaration.name == name) {
            compilationError_ = {"argument with the same name as procedure `" + name + "`.", argumentDeclaration.lineNumber};
            return false;
        }

        if (!declareVariableInProcedure(argumentDeclaration.lineNumber, argumentDeclaration.name, procedureIndex)) {
            return false;
        }

        argumentNameToTypeAndAddress.insert({argumentDeclaration.name, {argumentDeclaration.argumentType, currentAddress_ - 1}});
    }

    procedureTable_.insert({name, {argumentNameToTypeAndAddress}});

    return true;
}

bool SymbolTable::checkIfProcedureExists(const int lineNumber, const std::string& name, const std::vector<Argument>& arguments) {
    const auto it = procedureTable_.find(name);
    if (it == procedureTable_.end()) {
        compilationError_ = {"procedure `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const auto& procedureArgumentNameToTypeAndAddress = it->second.argumentNameToTypeAndAddress;
    const size_t actualNumberOfArguments = arguments.size();
    const size_t expectedNumberOfArguments = procedureArgumentNameToTypeAndAddress.size();

    if (actualNumberOfArguments < expectedNumberOfArguments) {
        compilationError_ = {"not enough arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }
    if (actualNumberOfArguments > expectedNumberOfArguments) {
        compilationError_ = {"too many arguments provided to `" + name + "`. Expected: " + std::to_string(expectedNumberOfArguments) + ", Got: " + std::to_string(actualNumberOfArguments) + ".", lineNumber};
        return false;
    }

    int i = 0;
    for (const auto& keyAndValue : procedureArgumentNameToTypeAndAddress) {
        const auto& [expectedArgumentType, _] = keyAndValue.second;

        const auto it2 = variableTable_.find(arguments[i].name);
        if (it2 == variableTable_.end()) {
            compilationError_ = {"variable `" + arguments[i].name + "` was not declared.", arguments[i].lineNumber};
            return false;
        }

        const ArgumentType actualArgumentType = (it2->second.arrayRange) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
        if (actualArgumentType != expectedArgumentType) {
            compilationError_ = {"argument `" + arguments[i].name + "` has incorrect type. Expected: " + toString(expectedArgumentType) + ", Got: " + toString(actualArgumentType) + "." , arguments[i].lineNumber};
            return false;

        }

        i++;
    }

    return true;
}

bool SymbolTable::declareVariableWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    if (variableTable_.find(internalName) != variableTable_.end()) {
        compilationError_ = {"redeclaration of variable `" + name + "`.", lineNumber};
        return false;
    }

    if (procedureTable_.find(name) != procedureTable_.end()) {
        compilationError_ = {"variable with the same name as procedure `" + name + "`.", lineNumber};
        return false;
    }

    variableTable_.insert({internalName, {currentAddress_, std::nullopt}});
    currentAddress_++;

    return true;
}

bool SymbolTable::declareArrayWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const long long lowerBound, const long long upperBound) {
    const std::string internalName = namePrefix + name;

    if (variableTable_.find(internalName) != variableTable_.end()) {
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

    variableTable_.insert({internalName, {currentAddress_, std::make_pair(lowerBound, upperBound)}});
    currentAddress_ += arraySize;

    return true;
}

bool SymbolTable::checkIfVariableExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    const auto it = variableTable_.find(internalName);
    if (it == variableTable_.end()) {
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

bool SymbolTable::checkIfArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const std::optional<long long> arrayIndex) {
    const std::string internalName = namePrefix + name;

    const auto it = variableTable_.find(internalName);
    if (it == variableTable_.end()) {
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

bool SymbolTable::checkIfVariableOrArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    const auto it = variableTable_.find(internalName);
    if (it == variableTable_.end()) {
        compilationError_ = {"variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    return true;
}

CompilationError SymbolTable::getCompilationError() const {
    return compilationError_;
}
