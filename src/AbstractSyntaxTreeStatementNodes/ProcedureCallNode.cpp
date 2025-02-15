#include "ProcedureCallNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ProcedureCallNode::ProcedureCallNode(const int lineNumber, const std::string name, ArgumentsNode* argumentsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), name_(name), argumentsNode_(argumentsNode) {}

void ProcedureCallNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitProcedureCallNode(*this);
}

bool ProcedureCallNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitProcedureCallNode(*this);
}

const std::string& ProcedureCallNode::getName() const {
    return name_;
}

const std::unique_ptr<ArgumentsNode>& ProcedureCallNode::getArgumentsNode() const {
    return argumentsNode_;
}
