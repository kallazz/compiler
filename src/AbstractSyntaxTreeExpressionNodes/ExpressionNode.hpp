#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include "ValueNode.hpp"
#include "../enums/MathematicalOperator.hpp"
#include <memory>

class ExpressionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ExpressionNode(const std::shared_ptr<ValueNode> valueNode);
    ExpressionNode(const std::shared_ptr<ValueNode> valueNode1, const std::shared_ptr<ValueNode> valueNode2_, const MathematicalOperator mathematicalOperator);

    void print() const override;

private:
    const std::shared_ptr<ValueNode> valueNode1_;
    const std::optional<std::shared_ptr<ValueNode>> valueNode2_;
    const std::optional<MathematicalOperator> mathematicalOperator_;
};

#endif // EXPRESSION_NODE_HPP