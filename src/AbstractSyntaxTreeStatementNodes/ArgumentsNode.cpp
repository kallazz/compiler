#include "ArgumentsNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ArgumentsNode::ArgumentsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsNode::addArgument(const int lineNumber, const std::string name) {
    arguments_.push_back({lineNumber, name});
}

void ArgumentsNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitArgumentsNode(*this);
}

bool ArgumentsNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitArgumentsNode(*this);
}

const std::vector<Argument>& ArgumentsNode::getArguments() const {
    return arguments_;
}
