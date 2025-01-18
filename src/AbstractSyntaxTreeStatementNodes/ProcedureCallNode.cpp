#include "ProcedureCallNode.hpp"
#include <iostream>

ProcedureCallNode::ProcedureCallNode(const int lineNumber, const std::string name, ArgumentsNode* argumentsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsNode_(argumentsNode) {}

bool ProcedureCallNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    // TODO: Add logic
    return true;
}

void ProcedureCallNode::print() const {
    std::cout << "ProcedureCallNode";
}