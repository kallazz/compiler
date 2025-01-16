#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include "ValueNode.hpp"
#include "../enums/ComparsionOperator.hpp"
#include <string>
#include <optional>

class ConditionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ConditionNode(const std::shared_ptr<ValueNode> valueNode1, const std::shared_ptr<ValueNode> valueNode2, const ComparsionOperator comparsionOperator);

    void print() const override;

private:
    const std::shared_ptr<ValueNode> valueNode1_;
    const std::shared_ptr<ValueNode> valueNode2_;
    const ComparsionOperator comparsionOperator_;
};

#endif // CONDITION_NODE_HPP