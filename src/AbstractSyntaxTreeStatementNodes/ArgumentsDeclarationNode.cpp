#include "ArgumentsDeclarationNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ArgumentsDeclarationNode::ArgumentsDeclarationNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsDeclarationNode::addArgumentDeclaration(const int lineNumber, const std::string name, const bool isArray) {
    argumentDeclarations_.push_back({lineNumber, name, isArray});
}

bool ArgumentsDeclarationNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitArgumentsDeclarationNode(*this);
}

void ArgumentsDeclarationNode::print() const {
    std::cout << "ArgumentsDeclarationNode: ";

    std::cout << '\n';
}

const std::vector<ArgumentDeclaration>& ArgumentsDeclarationNode::getArgumentDeclarations() const {
    return argumentDeclarations_;
}
