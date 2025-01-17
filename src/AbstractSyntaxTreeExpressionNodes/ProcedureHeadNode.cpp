#include "ProcedureHeadNode.hpp"
#include <iostream>

ProcedureHeadNode::ProcedureHeadNode(const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode)
    : name_(name), argumentsDeclarationNode_(argumentsDeclarationNode) {}

void ProcedureHeadNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}
