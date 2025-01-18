#include "CommandsNode.hpp"
#include <iostream>

CommandsNode::CommandsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void CommandsNode::addCommand(AbstractSyntaxTreeStatementNode* commandNode) {
    commandNodes_.emplace_back(commandNode);
}

bool CommandsNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    for (const auto& command : commandNodes_) {
        if (!command->evaluateBySymbolTable(symbolTable)) {
            return false;
        }
    }

    return true;
}

void CommandsNode::print() const {
    for (const auto& command : commandNodes_) {
        command->print();
        std::cout << '\n';
    }
}