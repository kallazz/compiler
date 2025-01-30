#include "ValueNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ValueNode::ValueNode(const int lineNumber, const long long number) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(number), identifierNode_(nullptr) {}

ValueNode::ValueNode(const int lineNumber, IdentifierNode* identifierNode) : AbstractSyntaxTreeExpressionNode(lineNumber), number_(std::nullopt), identifierNode_(identifierNode) {}

void ValueNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    assemblerGeneratorVisitor.visitValueNode(*this);
}

bool ValueNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitValueNode(*this);
}

const std::optional<long long>& ValueNode::getNumber() const {
    return number_;
}

const std::unique_ptr<IdentifierNode>& ValueNode::getIdentifierNode() const {
    return identifierNode_;
}
