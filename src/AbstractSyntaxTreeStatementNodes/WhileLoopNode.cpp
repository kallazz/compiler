#include "WhileLoopNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

WhileLoopNode::WhileLoopNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), conditionNode_(conditionNode), commandsNode_(commandsNode) {}

void WhileLoopNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitWhileLoopNode(*this);
}

bool WhileLoopNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitWhileLoopNode(*this);
}

const std::unique_ptr<ConditionNode>& WhileLoopNode::getConditionNode() const {
    return conditionNode_;
}

const std::unique_ptr<CommandsNode>& WhileLoopNode::getCommandsNode() const {
    return commandsNode_;
}
