#include "ArgumentsNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ArgumentsNode::ArgumentsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsNode::addArgument(const int lineNumber, const std::string name) {
    arguments_.push_back({lineNumber, name});
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
