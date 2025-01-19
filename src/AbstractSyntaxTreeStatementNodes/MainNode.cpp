#include "MainNode.hpp"
#include <iostream>

MainNode::MainNode(const int lineNumber, DeclarationsNode* declarationsNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), declarationsNode_(declarationsNode), commandsNode_(commandsNode) {}

MainNode::MainNode(const int lineNumber, CommandsNode* commandsNode) : AbstractSyntaxTreeStatementNode(lineNumber), commandsNode_(commandsNode) {}

bool MainNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitMainNode(*this);
}

void MainNode::print() const {
    std::cout << "MainNode\n";
    if (commandsNode_) {
        commandsNode_->print();
        std::cout << '\n';
    }
}

const std::unique_ptr<DeclarationsNode>& MainNode::getDeclarationsNode() const {
    return declarationsNode_;
}

const std::unique_ptr<CommandsNode>& MainNode::getCommandsNode() const {
    return commandsNode_;
}
