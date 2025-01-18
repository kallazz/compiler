#include "ExpressionNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ExpressionNode::ExpressionNode(const int lineNumber, ValueNode* valueNode) : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode), valueNode2_(std::nullopt), mathematicalOperator_(std::nullopt) {}

ExpressionNode::ExpressionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const MathematicalOperator mathematicalOperator)
    : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode1), valueNode2_(valueNode2), mathematicalOperator_(mathematicalOperator) {}

bool ExpressionNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (valueNode1_ && !valueNode1_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (valueNode2_ && *valueNode2_ && !((*valueNode2_)->evaluateBySymbolTable(symbolTable))) {
        return false;
    }

    return true;
}

void ExpressionNode::print() const {
    std::cout << "ExpressionNode: ";

    std::cout << '\n';
}