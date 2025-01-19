#include "AssignmentNode.hpp"
#include <iostream>

AssignmentNode::AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode), expressionNode_(expressionNode) {}

bool AssignmentNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitAssignmentNode(*this);
}

void AssignmentNode::print() const {
    std::cout << "AssignmentNode";
}

const std::unique_ptr<IdentifierNode>& AssignmentNode::getIdentifierNode() const {
    return identifierNode_;
}

const std::unique_ptr<ExpressionNode>& AssignmentNode::getExpressionNode() const {
    return expressionNode_;
}
