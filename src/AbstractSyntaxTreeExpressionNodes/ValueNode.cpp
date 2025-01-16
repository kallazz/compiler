#include "ValueNode.hpp"
#include <iostream>

ValueNode::ValueNode(const long long number) : number_(number), identifierNode_(std::nullopt) {}

ValueNode::ValueNode(const std::shared_ptr<IdentifierNode> identifierNode) : number_(std::nullopt), identifierNode_(identifierNode) {}

void ValueNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}