#include "ProceduresNode.hpp"
#include <iostream>

ProceduresNode::ProceduresNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, DeclarationsNode* declarationsNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, declarationsNode, commandsNode));
}

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, nullptr, commandsNode));
}

bool ProceduresNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    // TODO: Add logic
    return true;
}

void ProceduresNode::print() const {
    std::cout << "ProceduresNode\n";
    std::cout << getLineNumber();
}