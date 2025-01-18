#include "ConditionNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ConditionNode::ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator)
    : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode1), valueNode2_(valueNode2), comparsionOperator_(comparsionOperator) {}

bool ConditionNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (valueNode1_ && !valueNode1_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (valueNode2_ && !valueNode2_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void ConditionNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}