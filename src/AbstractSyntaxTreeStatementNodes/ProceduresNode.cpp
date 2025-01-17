#include "ProceduresNode.hpp"
#include <iostream>

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, DeclarationsNode* declarationsNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, declarationsNode, commandsNode));
}

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, nullptr, commandsNode));
}

void ProceduresNode::print() const {
    std::cout << "ProceduresNode: ";

    std::cout << "\n";
}