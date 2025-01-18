#include "IdentifierNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(std::nullopt), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name, const std::string indexName)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(indexName), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name, long long indexValue)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(std::nullopt), indexValue_(indexValue) {}

bool IdentifierNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (!indexName_ && !indexValue_) {
        return symbolTable.checkIfVariableExists(getLineNumber(), name_);
    }

    if (!indexName_) {
        return symbolTable.checkIfArrayExists(getLineNumber(), name_, indexValue_);
    }

    return symbolTable.checkIfArrayExists(getLineNumber(), name_, std::nullopt);
}

void IdentifierNode::print() const {
    std::cout << "IdentifierNode: " << name_;
    if (indexName_) {
        std::cout << "[" << *indexName_ << "]";
    }
    if (indexValue_) {
        std::cout << "[" << *indexValue_ << "]";
    }

    std::cout << '\n';
}
