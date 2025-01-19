#include "ExpressionNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ExpressionNode::ExpressionNode(const int lineNumber, ValueNode* valueNode) : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode), valueNode2_(nullptr), mathematicalOperator_(std::nullopt) {}

ExpressionNode::ExpressionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const MathematicalOperator mathematicalOperator)
    : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode1), valueNode2_(valueNode2), mathematicalOperator_(mathematicalOperator) {}

bool ExpressionNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitExpressionNode(*this);
}

void ExpressionNode::print() const {
    std::cout << "ExpressionNode: ";

    std::cout << '\n';
}

const std::unique_ptr<ValueNode>& ExpressionNode::getValueNode1() const {
    return valueNode1_;
}

const std::unique_ptr<ValueNode>& ExpressionNode::getValueNode2() const {
    return valueNode2_;
}

const std::optional<MathematicalOperator>& ExpressionNode::getMathematicalOperator() const {
    return mathematicalOperator_;
}