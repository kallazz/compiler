#include "CommandsNode.hpp"
#include <iostream>

void CommandsNode::addCommand(AbstractSyntaxTreeStatementNode* commandNode) {
    commandNodes_.emplace_back(commandNode);
}

void CommandsNode::print() const {
    for (const auto& command : commandNodes_) {
        command->print();
        std::cout << '\n';
    }
}