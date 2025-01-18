#include "ReadNode.hpp"
#include <iostream>

ReadNode::ReadNode(IdentifierNode* identifierNode) : identifierNode_(identifierNode) {}

void ReadNode::print() const {
    std::cout << "ReadNode";
}