#include "AssignmentNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

AssignmentNode::AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode), expressionNode_(expressionNode) {}

void AssignmentNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitAssignmentNode(*this);
}

bool AssignmentNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitAssignmentNode(*this);
}

const std::unique_ptr<IdentifierNode>& AssignmentNode::getIdentifierNode() const {
    return identifierNode_;
}

const std::unique_ptr<ExpressionNode>& AssignmentNode::getExpressionNode() const {
    return expressionNode_;
}
