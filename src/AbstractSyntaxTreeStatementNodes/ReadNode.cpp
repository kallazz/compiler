#include "ReadNode.hpp"
#include <iostream>

ReadNode::ReadNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeStatementNode(lineNumber), identifierNode_(identifierNode) {}

bool ReadNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitReadNode(*this);
}

void ReadNode::print() const {
    std::cout << "ReadNode";
}

const std::unique_ptr<IdentifierNode>& ReadNode::getIdentifierNode() const {
    return identifierNode_;
}
