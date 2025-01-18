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

bool SymbolTable::declareVariableWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    if (table_.find(internalName) != table_.end()) {
        compilationError_ = {"Variable `" + name + "` redeclared.", lineNumber};
        return false;
    }

    table_.insert({internalName, {currentAddress_, std::nullopt}});
    currentAddress_++;

    return true;
}

bool SymbolTable::declareArrayWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const long long lowerBound, const long long upperBound) {
    const std::string internalName = namePrefix + name;

    if (table_.find(internalName) != table_.end()) {
        compilationError_ = {"Variable `" + name + "` redeclared.", lineNumber};
        return false;
    }

    if (lowerBound > upperBound) {
        compilationError_ = {"Incorrect array range in `" + name + "`. " + std::to_string(lowerBound) + " is bigger than " + std::to_string(upperBound) + ".", lineNumber};
        return false;
    }

    const long long arraySize = upperBound - lowerBound + 1;

    table_.insert({internalName, {currentAddress_, std::make_pair(lowerBound, upperBound)}});
    currentAddress_ += arraySize;

    return true;
}

bool SymbolTable::checkIfVariableExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    const auto it = table_.find(internalName);
    if (it == table_.end()) {
        compilationError_ = {"Variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const SymbolInfo& symbolInfo = it->second;
    if (symbolInfo.arrayRange) {
        compilationError_ = {"Variable `" + name + "` is an array. [] need to be used in assignment.", lineNumber};
        return false;
    }

    return true;
}

bool SymbolTable::checkIfArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix, const std::optional<long long> arrayIndex) {
    const std::string internalName = namePrefix + name;

    const auto it = table_.find(internalName);
    if (it == table_.end()) {
        compilationError_ = {"Variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    const SymbolInfo& symbolInfo = it->second;
    if (!symbolInfo.arrayRange) {
        compilationError_ = {"Variable `" + name + "` is not an array. [] cannot be used in assignment.", lineNumber};
        return false;
    }

    if (arrayIndex) {
        const auto& [lowerBound, upperBound] = *symbolInfo.arrayRange;
        if (*arrayIndex < lowerBound || *arrayIndex > upperBound) {
            compilationError_ = {
                "Array index out of bounds for variable `" + name + "`. Index: " + std::to_string(*arrayIndex) + ", Range: [" + std::to_string(lowerBound) + ", " + std::to_string(upperBound) + "].",
                lineNumber
            };
            return false;
        }
    }

    return true;
}

bool SymbolTable::checkIfVariableOrArrayExistsWithNamePrefix(const int lineNumber, const std::string& name, const std::string& namePrefix) {
    const std::string internalName = namePrefix + name;

    const auto it = table_.find(internalName);
    if (it == table_.end()) {
        compilationError_ = {"Variable `" + name + "` was not declared.", lineNumber};
        return false;
    }

    return true;
}

CompilationError SymbolTable::getCompilationError() const {
    return compilationError_;
}
