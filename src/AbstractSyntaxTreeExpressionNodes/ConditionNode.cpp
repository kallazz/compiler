#include "ConditionNode.hpp"
#include <iostream>

ConditionNode::ConditionNode(const std::shared_ptr<ValueNode> valueNode1, const std::shared_ptr<ValueNode> valueNode2, const ComparsionOperator comparsionOperator)
    : valueNode1_(valueNode1), valueNode2_(valueNode2), comparsionOperator_(comparsionOperator) {}

void ConditionNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}