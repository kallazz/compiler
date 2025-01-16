#include "ConditionNode.hpp"
#include <iostream>

ConditionNode::ConditionNode(ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator)
    : valueNode1_(valueNode1), valueNode2_(valueNode2), comparsionOperator_(comparsionOperator) {}

void ConditionNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}