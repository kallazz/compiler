#include "ReadNode.hpp"
#include <iostream>

ReadNode::ReadNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode) {}

bool ReadNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (identifierNode_ && !identifierNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void ReadNode::print() const {
    std::cout << "ReadNode";
}