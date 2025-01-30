#include "CommandsNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

CommandsNode::CommandsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void CommandsNode::addCommand(AbstractSyntaxTreeStatementNode* commandNode) {
    commandNodes_.emplace_back(commandNode);
}

void CommandsNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitCommandsNode(*this);
}

bool CommandsNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitCommandsNode(*this);
}

const std::vector<std::unique_ptr<AbstractSyntaxTreeStatementNode>>& CommandsNode::getCommandNodes() const {
    return commandNodes_;
}
