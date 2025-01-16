#include "ExpressionNode.hpp"
#include <iostream>

ExpressionNode::ExpressionNode(const std::shared_ptr<ValueNode> valueNode) : valueNode1_(valueNode), valueNode2_(std::nullopt), mathematicalOperator_(std::nullopt) {}

ExpressionNode::ExpressionNode(const std::shared_ptr<ValueNode> valueNode1, const std::shared_ptr<ValueNode> valueNode2, const MathematicalOperator mathematicalOperator)
    : valueNode1_(valueNode1), valueNode2_(valueNode2), mathematicalOperator_(mathematicalOperator) {}

void ExpressionNode::print() const {
    std::cout << "ExpressionNode: ";

    std::cout << '\n';
}