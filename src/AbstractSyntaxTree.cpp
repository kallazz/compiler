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
    std::cout << '\n';

    if (mainNode_) {
        mainNode_->print();
    }
    std::cout << '\n';
}

bool AbstractSyntaxTree::fillSymbolTable() {
    // TODO: Add procedures node
    if (mainNode_ && !mainNode_->evaluateBySymbolTable(symbolTable_)) {
        compilationError_ = symbolTable_.getCompilationError();
        return false;
    }
    
    return true;
}

CompilationError AbstractSyntaxTree::getCompilationError() const {
    return compilationError_;
}