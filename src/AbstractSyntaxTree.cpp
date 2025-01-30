#include "AbstractSyntaxTree.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

AbstractSyntaxTree::AbstractSyntaxTree() : proceduresNode_(nullptr), mainNode_(nullptr), symbolTable_(), isMultiplicationProcedureNeeded_(false), isDivisionProcedureNeeded_(false), isModuloProcedureNeeded_(false) {}

void AbstractSyntaxTree::setProceduresNode(ProceduresNode* proceduresNode) {
    proceduresNode_ = std::unique_ptr<ProceduresNode>(proceduresNode);
}

void AbstractSyntaxTree::setMainNode(MainNode* mainNode) {
    mainNode_ = std::unique_ptr<MainNode>(mainNode);
}

void AbstractSyntaxTree::setIsMultiplicationProcedureNeeded(const bool value) {
    isMultiplicationProcedureNeeded_ = value;
}

void AbstractSyntaxTree::setIsDivisionProcedureNeeded(const bool value) {
    isDivisionProcedureNeeded_ = value;
}

void AbstractSyntaxTree::setIsModuloProcedureNeeded(const bool value) {
    isModuloProcedureNeeded_ = value;
}

bool AbstractSyntaxTree::fillSymbolTable() {
    const bool isAnyMathematicalProcedureNeeded = (isMultiplicationProcedureNeeded_ || isDivisionProcedureNeeded_ || isModuloProcedureNeeded_);

    SemanticAnalysisVisitor semanticAnalysisVisitor(symbolTable_, isAnyMathematicalProcedureNeeded);

    if (proceduresNode_ && !proceduresNode_->accept(semanticAnalysisVisitor)) {
        compilationError_ = symbolTable_.getCompilationError();
        return false;
    }

    if (mainNode_ && !mainNode_->accept(semanticAnalysisVisitor)) {
        compilationError_ = symbolTable_.getCompilationError();
        return false;
    }
    
    return true;
}

std::string AbstractSyntaxTree::generateAssembler() const {
    AssemblerGeneratorVisitor assemblerGeneratorVisitor(symbolTable_, isMultiplicationProcedureNeeded_, isDivisionProcedureNeeded_, isModuloProcedureNeeded_);

    proceduresNode_->accept(assemblerGeneratorVisitor);
    mainNode_->accept(assemblerGeneratorVisitor);

    return assemblerGeneratorVisitor.getGeneratedAssemblerCode();
}

CompilationError AbstractSyntaxTree::getCompilationError() const {
    return compilationError_;
}