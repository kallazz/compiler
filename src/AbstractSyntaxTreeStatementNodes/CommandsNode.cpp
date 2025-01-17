#include "CommandsNode.hpp"

void CommandsNode::addCommand(AbstractSyntaxTreeStatementNode* commandNode) {
    commandNodes_.emplace_back(commandNode);
}

void CommandsNode::print() const {
    for (const auto& command : commandNodes_) {
        command->print();
    }
}