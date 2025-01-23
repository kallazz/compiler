#include "ArgumentsNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

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

void ArgumentsNode::print() const {
    std::cout << "ArgumentNode: ";

    std::cout << '\n';
}

const std::vector<Argument>& ArgumentsNode::getArguments() const {
    return arguments_;
}
