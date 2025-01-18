#include "AssignmentNode.hpp"
#include <iostream>

AssignmentNode::AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode), expressionNode_(expressionNode) {}

bool AssignmentNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (identifierNode_ && !identifierNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (expressionNode_ && !expressionNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void AssignmentNode::print() const {
    std::cout << "AssignmentNode";
}