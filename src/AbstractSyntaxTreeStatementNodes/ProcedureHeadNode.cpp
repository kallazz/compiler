#include "ProcedureHeadNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ProcedureHeadNode::ProcedureHeadNode(const int lineNumber, const std::string name,
                                     ArgumentsDeclarationNode* argumentsDeclarationNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsDeclarationNode_(argumentsDeclarationNode) {}

void ProcedureHeadNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitProcedureHeadNode(*this);
}

bool ProcedureHeadNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitProcedureHeadNode(*this);
}

const std::string& ProcedureHeadNode::getName() const {
    return name_;
}

const std::unique_ptr<ArgumentsDeclarationNode>& ProcedureHeadNode::getArgumentsDeclarationNode() const {
    return argumentsDeclarationNode_;
}
