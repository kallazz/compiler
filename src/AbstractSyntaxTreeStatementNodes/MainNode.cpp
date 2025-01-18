#include "MainNode.hpp"
#include <iostream>

MainNode::MainNode(DeclarationsNode* declarationsNode, CommandsNode* commandsNode)
    : declarationsNode_(declarationsNode), commandsNode_(commandsNode) {}

MainNode::MainNode(CommandsNode* commandsNode) : commandsNode_(commandsNode) {}

void MainNode::print() const {
    std::cout << "MainNode\n";
    if (commandsNode_) {
        commandsNode_->print();
        std::cout << '\n';
    }
}