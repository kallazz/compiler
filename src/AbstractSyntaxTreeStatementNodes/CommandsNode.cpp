#include "CommandsNode.hpp"
#include "../SemanticAnalysisVisitor.hpp"
#include <iostream>

CommandsNode::CommandsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void CommandsNode::addCommand(AbstractSyntaxTreeStatementNode* commandNode) {
    commandNodes_.emplace_back(commandNode);
}

bool CommandsNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitCommandsNode(*this);
}

void CommandsNode::print() const {
    for (const auto& command : commandNodes_) {
        command->print();
        std::cout << '\n';
    }
}

const std::vector<std::unique_ptr<AbstractSyntaxTreeStatementNode>>& CommandsNode::getCommandNodes() const {
    return commandNodes_;
}
