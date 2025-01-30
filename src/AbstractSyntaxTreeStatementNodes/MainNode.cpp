#include "MainNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

MainNode::MainNode(const int lineNumber, DeclarationsNode* declarationsNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), declarationsNode_(declarationsNode), commandsNode_(commandsNode) {}

MainNode::MainNode(const int lineNumber, CommandsNode* commandsNode) : AbstractSyntaxTreeStatementNode(lineNumber), commandsNode_(commandsNode) {}

void MainNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitMainNode(*this);
}

bool MainNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitMainNode(*this);
}

const std::unique_ptr<DeclarationsNode>& MainNode::getDeclarationsNode() const {
    return declarationsNode_;
}

const std::unique_ptr<CommandsNode>& MainNode::getCommandsNode() const {
    return commandsNode_;
}
