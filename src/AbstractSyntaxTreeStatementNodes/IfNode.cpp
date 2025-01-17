#include "IfNode.hpp"
#include <iostream>

IfNode::IfNode(ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode)
    : conditionNode_(conditionNode), thenCommandsNode_(thenCommandsNode), elseCommandsNode_(elseCommandsNode) {}

void IfNode::print() const {
    std::cout << "IfNode: ";

    std::cout << '\n';
}