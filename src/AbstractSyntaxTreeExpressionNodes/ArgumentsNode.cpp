#include "ArgumentsNode.hpp"
#include <iostream>

void ArgumentsNode::addArgument(const std::string name) {
    argumentNames_.push_back(name);
}

void ArgumentsNode::print() const {
    std::cout << "ArgumentNode: ";

    std::cout << '\n';
}
