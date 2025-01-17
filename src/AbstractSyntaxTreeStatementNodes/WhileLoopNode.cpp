#include "WhileLoopNode.hpp"
#include <iostream>

WhileLoopNode::WhileLoopNode(ConditionNode* conditionNode, CommandsNode* commandsNode)
    : conditionNode_(conditionNode), commandsNode_(commandsNode) {}

void WhileLoopNode::print() const {
    std::cout << "WhileLoopNode: ";

    std::cout << '\n';
}