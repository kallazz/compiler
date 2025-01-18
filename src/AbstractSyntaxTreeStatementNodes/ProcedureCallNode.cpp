#include "ProcedureCallNode.hpp"
#include <iostream>

ProcedureCallNode::ProcedureCallNode(const std::string name, ArgumentsNode* argumentsNode)
    : name_(name), argumentsNode_(argumentsNode) {}

void ProcedureCallNode::print() const {
    std::cout << "ProcedureCallNode";
}