#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"
#include "enums/ComparsionOperator.hpp"
#include <memory>

class ConditionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<ValueNode>& getValueNode1() const;
    const std::unique_ptr<ValueNode>& getValueNode2() const;
    ComparsionOperator getComparisonOperator() const;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::unique_ptr<ValueNode> valueNode2_;
    const ComparsionOperator comparsionOperator_;
};

#endif // CONDITION_NODE_HPP