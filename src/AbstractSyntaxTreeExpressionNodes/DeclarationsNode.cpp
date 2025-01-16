#include "DeclarationsNode.hpp"
#include <iostream>

void DeclarationsNode::addDeclaration(const std::string name) {
    declarations_.push_back({name, std::nullopt, std::nullopt});
}

void DeclarationsNode::addDeclaration(const std::string name, long long arrayLowerBound, long long arrayUpperBound) {
    declarations_.push_back({name, arrayLowerBound, arrayUpperBound});
}

void DeclarationsNode::print() const {
    std::cout << "DeclarationNode: ";
    for (const auto& declaration : declarations_) {
        std::cout << declaration.name;
        if (declaration.arrayLowerBound) {
            std::cout << "[" << *declaration.arrayLowerBound << ":" << *declaration.arrayUpperBound << "]";
        }
        std::cout << " ";
    }

    std::cout << '\n';
}