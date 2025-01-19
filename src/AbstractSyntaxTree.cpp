#include "AbstractSyntaxTree.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

AbstractSyntaxTree::AbstractSyntaxTree() : proceduresNode_(nullptr), mainNode_(nullptr), symbolTable_() {}

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
    SemanticAnalysisVisitor semanticAnalysisVisitor(symbolTable_);
    // TODO: Add procedures node
    if (mainNode_ && !mainNode_->accept(semanticAnalysisVisitor)) {
        compilationError_ = symbolTable_.getCompilationError();
        return false;
    }
    
    return true;
}

CompilationError AbstractSyntaxTree::getCompilationError() const {
    return compilationError_;
}