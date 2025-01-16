#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"
#include "../enums/MathematicalOperator.hpp"
#include <memory>

class ExpressionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ExpressionNode(ValueNode* valueNode);
    ExpressionNode(ValueNode* valueNode1, ValueNode* valueNode2_, const MathematicalOperator mathematicalOperator);

    void print() const override;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::optional<std::unique_ptr<ValueNode>> valueNode2_;
    const std::optional<MathematicalOperator> mathematicalOperator_;
};

#endif // EXPRESSION_NODE_HPP