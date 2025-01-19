#include "DeclarationsNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

DeclarationsNode::DeclarationsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name) {
    declarations_.push_back({lineNumber, name, std::nullopt, std::nullopt});
}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name, long long arrayLowerBound, long long arrayUpperBound) {
    declarations_.push_back({lineNumber, name, arrayLowerBound, arrayUpperBound});
}

bool DeclarationsNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitDeclarationsNode(*this);
}

void DeclarationsNode::print() const {
    std::cout << "DeclarationsNode: ";
    for (const auto& declaration : declarations_) {
        std::cout << declaration.name;
        if (declaration.arrayLowerBound && declaration.arrayUpperBound) {
            std::cout << "[" << *declaration.arrayLowerBound << ":" << *declaration.arrayUpperBound << "]";
        }
        std::cout << " ";
    }

    std::cout << '\n';
}

const std::vector<Declaration>& DeclarationsNode::getDeclarations() const {
    return declarations_;
}
