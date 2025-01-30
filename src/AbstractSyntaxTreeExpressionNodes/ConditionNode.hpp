#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"

#include <memory>

enum class ComparisonOperator {
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    EQUAL,
    NOT_EQUAL
};

class ConditionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const ComparisonOperator comparisonOperator);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<ValueNode>& getValueNode1() const;
    const std::unique_ptr<ValueNode>& getValueNode2() const;
    ComparisonOperator getComparisonOperator() const;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::unique_ptr<ValueNode> valueNode2_;
    const ComparisonOperator comparisonOperator_;
};

#endif // CONDITION_NODE_HPP