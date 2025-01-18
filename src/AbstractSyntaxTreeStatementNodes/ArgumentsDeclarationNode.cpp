#include "ArgumentsDeclarationNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ArgumentsDeclarationNode::ArgumentsDeclarationNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsDeclarationNode::addArgumentDeclaration(const int lineNumber, const std::string name, const bool isArray) {
    argumentDeclarations_.push_back({lineNumber, name, isArray});
}

bool ArgumentsDeclarationNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    // for (const auto& argumentDeclaration : argumentDeclarations_) {
    //     if (argumentDeclaration.isArray) {
    //         symbolTable.declareArrayInProcedure(argumentDeclaration.lineNumber, argumentDeclaration.name, )
    //     }
    // }

    // TODO: Add logic

    return true;
}

void ArgumentsDeclarationNode::print() const {
    std::cout << "ArgumentsDeclarationNode: ";

    std::cout << '\n';
}