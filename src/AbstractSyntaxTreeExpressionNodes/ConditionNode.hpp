#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"
#include "enums/ComparsionOperator.hpp"
#include <memory>
#include <string>

class ConditionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ConditionNode(ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator);

    void print() const override;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::unique_ptr<ValueNode> valueNode2_;
    const ComparsionOperator comparsionOperator_;
};

#endif // CONDITION_NODE_HPP