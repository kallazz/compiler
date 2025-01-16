#include "AbstractSyntaxTree.hpp"
#include <iostream>

void AbstractSyntaxTree::addNode(const std::shared_ptr<AbstractSyntaxTreeExpressionNode> node) {
    nodes_.push_back(node);
}

void AbstractSyntaxTree::printNodes() const {
    for (const auto &node : nodes_) {
        node->print();
    }
}