#include "IfNode.hpp"
#include <iostream>

IfNode::IfNode(ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode)
    : conditionNode_(conditionNode), thenCommandsNode_(thenCommandsNode), elseCommandsNode_(elseCommandsNode) {}

void IfNode::print() const {
    std::cout << "IfNode\n{\n";
    if (thenCommandsNode_) {
        thenCommandsNode_->print();
    }
    std::cout << "}";
    if (elseCommandsNode_) {
        std::cout << "\n!{\n";
        elseCommandsNode_->print();
        std::cout << "!}";
    }
}