#include "ValueNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ValueNode::ValueNode(const int lineNumber, const long long number) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(number), identifierNode_(nullptr) {}

ValueNode::ValueNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(std::nullopt), identifierNode_(identifierNode) {}

bool ValueNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitValueNode(*this);
}

void ValueNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}

const std::optional<long long>& ValueNode::getNumber() const {
    return number_;
}

const std::unique_ptr<IdentifierNode>& ValueNode::getIdentifierNode() const {
    return identifierNode_;
}
