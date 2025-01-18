#include "WhileLoopNode.hpp"
#include <iostream>

WhileLoopNode::WhileLoopNode(ConditionNode* conditionNode, CommandsNode* commandsNode)
    : conditionNode_(conditionNode), commandsNode_(commandsNode) {}

void WhileLoopNode::print() const {
    std::cout << "WhileLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}