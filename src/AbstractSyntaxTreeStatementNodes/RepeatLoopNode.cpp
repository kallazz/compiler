#include "RepeatLoopNode.hpp"
#include <iostream>

RepeatLoopNode::RepeatLoopNode(CommandsNode* commandsNode, ConditionNode* conditionNode)
    : commandsNode_(commandsNode), conditionNode_(conditionNode) {}

void RepeatLoopNode::print() const {
    std::cout << "RepeatLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}