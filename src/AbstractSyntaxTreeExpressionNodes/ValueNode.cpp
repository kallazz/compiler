#include "ValueNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ValueNode::ValueNode(const int lineNumber, const long long number) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(number), identifierNode_(std::nullopt) {}

ValueNode::ValueNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(std::nullopt), identifierNode_(identifierNode) {}

bool ValueNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (number_) {
        return true;
    }

    return (*identifierNode_)->evaluateBySymbolTable(symbolTable);
}

void ValueNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}