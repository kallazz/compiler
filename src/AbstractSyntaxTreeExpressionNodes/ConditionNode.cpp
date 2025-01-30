#include "ConditionNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ConditionNode::ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2,
                             const ComparisonOperator comparisonOperator)
    : AbstractSyntaxTreeExpressionNode(lineNumber),
      valueNode1_(valueNode1),
      valueNode2_(valueNode2),
      comparisonOperator_(comparisonOperator) {}

void ConditionNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    assemblerGeneratorVisitor.visitConditionNode(*this);
}

bool ConditionNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitConditionNode(*this);
}

const std::unique_ptr<ValueNode>& ConditionNode::getValueNode1() const {
    return valueNode1_;
}

const std::unique_ptr<ValueNode>& ConditionNode::getValueNode2() const {
    return valueNode2_;
}

ComparisonOperator ConditionNode::getComparisonOperator() const {
    return comparisonOperator_;
}
