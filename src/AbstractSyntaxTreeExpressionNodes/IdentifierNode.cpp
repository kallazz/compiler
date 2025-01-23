#include "IdentifierNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(std::nullopt), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name, const std::string indexName)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(indexName), indexValue_(std::nullopt) {}

IdentifierNode::IdentifierNode(const int lineNumber, const std::string name, long long indexValue)
    : AbstractSyntaxTreeExpressionNode(lineNumber), name_(name), indexName_(std::nullopt), indexValue_(indexValue) {}

void IdentifierNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    assemblerGeneratorVisitor.visitIdentifierNode(*this);
}

bool IdentifierNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitIdentifierNode(*this);
}

void IdentifierNode::print() const {
    std::cout << "IdentifierNode: " << name_;
    if (indexName_) {
        std::cout << "[" << *indexName_ << "]";
    }
    if (indexValue_) {
        std::cout << "[" << *indexValue_ << "]";
    }

    std::cout << '\n';
}

const std::string& IdentifierNode::getName() const {
    return name_;
}

const std::optional<std::string>& IdentifierNode::getIndexName() const {
    return indexName_;
}

const std::optional<long long>& IdentifierNode::getIndexValue() const {
    return indexValue_;
}