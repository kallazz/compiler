#include "ArgumentsDeclarationNode.hpp"
#include <iostream>

void ArgumentsDeclarationNode::addArgumentDeclaration(const std::string name, const bool isArray) {
    argumentDeclarations_.push_back({name, isArray});
}

void ArgumentsDeclarationNode::print() const {
    std::cout << "ArgumentsDeclarationNode: ";

    std::cout << '\n';
}