#include "IfNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

IfNode::IfNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* thenCommandsNode,
               CommandsNode* elseCommandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber),
      conditionNode_(conditionNode),
      thenCommandsNode_(thenCommandsNode),
      elseCommandsNode_(elseCommandsNode) {}

void IfNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitIfNode(*this);
}

bool IfNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitIfNode(*this);
}

const std::unique_ptr<ConditionNode>& IfNode::getConditionNode() const {
    return conditionNode_;
}

const std::unique_ptr<CommandsNode>& IfNode::getThenCommandsNode() const {
    return thenCommandsNode_;
}

const std::unique_ptr<CommandsNode>& IfNode::getElseCommandsNode() const {
    return elseCommandsNode_;
}
