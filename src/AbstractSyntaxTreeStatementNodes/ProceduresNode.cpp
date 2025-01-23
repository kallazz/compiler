#include "ProceduresNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

ProceduresNode::ProceduresNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, DeclarationsNode* declarationsNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, declarationsNode, commandsNode));
}

void ProceduresNode::addProcedure(ProcedureHeadNode* procedureHeadNode, CommandsNode* commandsNode) {
    procedures_.emplace_back(std::make_unique<Procedure>(procedureHeadNode, nullptr, commandsNode));
}

void ProceduresNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitProceduresNode(*this);
}

bool ProceduresNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitProceduresNode(*this);
}

void ProceduresNode::print() const {
    std::cout << "ProceduresNode\n";
    std::cout << getLineNumber();
}

const std::vector<std::unique_ptr<Procedure>>& ProceduresNode::getProcedures() const {
    return procedures_;
}
