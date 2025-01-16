#include "ExpressionNode.hpp"
#include <iostream>

ExpressionNode::ExpressionNode(ValueNode* valueNode) : valueNode1_(valueNode), valueNode2_(std::nullopt), mathematicalOperator_(std::nullopt) {}

ExpressionNode::ExpressionNode(ValueNode* valueNode1, ValueNode* valueNode2, const MathematicalOperator mathematicalOperator)
    : valueNode1_(valueNode1), valueNode2_(valueNode2), mathematicalOperator_(mathematicalOperator) {}

void ExpressionNode::print() const {
    std::cout << "ExpressionNode: ";

    std::cout << '\n';
}