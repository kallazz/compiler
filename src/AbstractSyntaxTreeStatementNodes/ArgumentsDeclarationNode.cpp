#include "ArgumentsDeclarationNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

ArgumentsDeclarationNode::ArgumentsDeclarationNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsDeclarationNode::addArgumentDeclaration(const int lineNumber, const std::string name, const bool isArray) {
    const ArgumentType argumentType = (isArray) ? ArgumentType::ARRAY : ArgumentType::NUMBER;
    argumentDeclarations_.push_back({lineNumber, name, argumentType});
}

void ArgumentsDeclarationNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitArgumentsDeclarationNode(*this);
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
