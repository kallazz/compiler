#include "DeclarationsNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

DeclarationsNode::DeclarationsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name) {
    declarations_.push_back({lineNumber, name, std::nullopt, std::nullopt});
}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name, long long arrayLowerBound, long long arrayUpperBound) {
    declarations_.push_back({lineNumber, name, arrayLowerBound, arrayUpperBound});
}

bool DeclarationsNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    for (const auto& declaration : declarations_) {
        bool wasEvaluationSuccessful = false;

        if (declaration.arrayLowerBound && declaration.arrayUpperBound) {
            wasEvaluationSuccessful = symbolTable.declareArray(declaration.lineNumber, declaration.name, *declaration.arrayLowerBound, *declaration.arrayUpperBound);
        } else {
            wasEvaluationSuccessful = symbolTable.declareVariable(declaration.lineNumber, declaration.name);
        }

        if (!wasEvaluationSuccessful) {
            return false;
        }
    }

    return true;
}

void DeclarationsNode::print() const {
    std::cout << "DeclarationsNode: ";
    for (const auto& declaration : declarations_) {
        std::cout << declaration.name;
        if (declaration.arrayLowerBound && declaration.arrayUpperBound) {
            std::cout << "[" << *declaration.arrayLowerBound << ":" << *declaration.arrayUpperBound << "]";
        }
        std::cout << " ";
    }

    std::cout << '\n';
}