#include "AbstractSyntaxTree.hpp"
#include <iostream>

AbstractSyntaxTree::AbstractSyntaxTree() : proceduresNode_(nullptr), mainNode_(nullptr) {}

void AbstractSyntaxTree::setProceduresNode(ProceduresNode* proceduresNode) {
    proceduresNode_ = std::unique_ptr<ProceduresNode>(proceduresNode);
}

void AbstractSyntaxTree::setMainNode(MainNode* mainNode) {
    mainNode_ = std::unique_ptr<MainNode>(mainNode);
}

void AbstractSyntaxTree::printNodes() const {
    if (proceduresNode_) {
        proceduresNode_->print();
    }
    if (mainNode_) {
        mainNode_->print();
    }
}