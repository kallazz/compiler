#include "AssignmentNode.hpp"
#include <iostream>

AssignmentNode::AssignmentNode(IdentifierNode* identifierNode, ExpressionNode* expressionNode)
    : identifierNode_(identifierNode), expressionNode_(expressionNode) {}

void AssignmentNode::print() const {
    std::cout << "AssignmentNode: ";

    std::cout << std::endl;
}