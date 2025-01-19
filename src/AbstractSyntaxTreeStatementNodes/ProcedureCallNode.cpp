#include "ProcedureCallNode.hpp"
#include <iostream>

ProcedureCallNode::ProcedureCallNode(const int lineNumber, const std::string name, ArgumentsNode* argumentsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsNode_(argumentsNode) {}

bool ProcedureCallNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitProcedureCallNode(*this);
}

void ProcedureCallNode::print() const {
    std::cout << "ProcedureCallNode";
}

const std::string& ProcedureCallNode::getName() const {
    return name_;
}

const std::unique_ptr<ArgumentsNode>& ProcedureCallNode::getArgumentsNode() const {
    return argumentsNode_;
}
