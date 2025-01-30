#include "DeclarationsNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

DeclarationsNode::DeclarationsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name) {
    declarations_.push_back({lineNumber, name, std::nullopt, std::nullopt});
}

void DeclarationsNode::addDeclaration(const int lineNumber, const std::string name, long long arrayLowerBound, long long arrayUpperBound) {
    declarations_.push_back({lineNumber, name, arrayLowerBound, arrayUpperBound});
}

void DeclarationsNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitDeclarationsNode(*this);
}

bool DeclarationsNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitDeclarationsNode(*this);
}

const std::vector<Declaration>& DeclarationsNode::getDeclarations() const {
    return declarations_;
}
