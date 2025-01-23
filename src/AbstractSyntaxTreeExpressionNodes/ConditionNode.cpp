#include "ConditionNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

ConditionNode::ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator)
    : AbstractSyntaxTreeExpressionNode(lineNumber), valueNode1_(valueNode1), valueNode2_(valueNode2), comparsionOperator_(comparsionOperator) {}

void ConditionNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    assemblerGeneratorVisitor.visitConditionNode(*this);
}

bool ConditionNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitConditionNode(*this);
}

void ConditionNode::print() const {
    std::cout << "ValueNode: ";

    std::cout << '\n';
}

const std::unique_ptr<ValueNode>& ConditionNode::getValueNode1() const {
    return valueNode1_;
}

const std::unique_ptr<ValueNode>& ConditionNode::getValueNode2() const {
    return valueNode2_;
}

const std::optional<ComparsionOperator> ConditionNode::getComparisonOperator() const {
    return comparsionOperator_;
}
