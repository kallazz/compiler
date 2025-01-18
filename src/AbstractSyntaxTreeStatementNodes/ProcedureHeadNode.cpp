#include "ProcedureHeadNode.hpp"
#include <iostream>

ProcedureHeadNode::ProcedureHeadNode(const int lineNumber, const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsDeclarationNode_(argumentsDeclarationNode) {}

bool ProcedureHeadNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    // TODO: Add logic
    return true;
}

void ProcedureHeadNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}
