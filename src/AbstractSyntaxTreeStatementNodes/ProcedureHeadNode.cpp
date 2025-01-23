#include "ProcedureHeadNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

ProcedureHeadNode::ProcedureHeadNode(const int lineNumber, const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsDeclarationNode_(argumentsDeclarationNode) {}

void ProcedureHeadNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitProcedureHeadNode(*this);
}

bool ProcedureHeadNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitProcedureHeadNode(*this);
}

void ProcedureHeadNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}

const std::string& ProcedureHeadNode::getName() const {
    return name_;
}

const std::unique_ptr<ArgumentsDeclarationNode>& ProcedureHeadNode::getArgumentsDeclarationNode() const {
    return argumentsDeclarationNode_;
}
