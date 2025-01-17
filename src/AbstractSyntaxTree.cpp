#include "AbstractSyntaxTree.hpp"
#include <iostream>

void AbstractSyntaxTree::setProceduresNode(ProceduresNode* proceduresNode) {
    proceduresNode_ = std::unique_ptr<ProceduresNode>(proceduresNode);
}

void AbstractSyntaxTree::setMainNode(MainNode* mainNode) {
    mainNode_ = std::unique_ptr<MainNode>(mainNode);
}

void AbstractSyntaxTree::printNodes() const {
    proceduresNode_->print();
    mainNode_->print();
}