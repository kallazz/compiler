#include "IdentifierNode.hpp"
#include <iostream>

IdentifierNode::IdentifierNode(const std::string name)
    : name_(name), indexName_(std::nullopt), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const std::string name, const std::string indexName)
    : name_(name), indexName_(indexName), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const std::string name, long long indexValue)
    : name_(name), indexName_(std::nullopt), indexValue_(indexValue) {}

void IdentifierNode::print() const {
    std::cout << "IdentifierNode: " << name_;
    if (indexName_) {
        std::cout << "[" << *indexName_ << "]";
    }
    if (indexValue_) {
        std::cout << "[" << *indexValue_ << "]";
    }

    std::cout << '\n';
}
