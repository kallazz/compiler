#include "WriteNode.hpp"
#include <iostream>

WriteNode::WriteNode(ValueNode* valueNode) : valueNode_(valueNode) {}

void WriteNode::print() const {
    std::cout << "WriteNode";
}