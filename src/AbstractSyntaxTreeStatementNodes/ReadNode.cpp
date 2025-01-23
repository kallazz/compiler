#include "ReadNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

ReadNode::ReadNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode) {}

void ReadNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitReadNode(*this);
}

bool ReadNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitReadNode(*this);
}

void ReadNode::print() const {
    std::cout << "ReadNode";
}

const std::unique_ptr<IdentifierNode>& ReadNode::getIdentifierNode() const {
    return identifierNode_;
}
