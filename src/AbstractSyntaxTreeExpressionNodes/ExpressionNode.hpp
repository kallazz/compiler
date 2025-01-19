#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"
#include "enums/MathematicalOperator.hpp"
#include <memory>

class ExpressionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ExpressionNode(const int lineNumber, ValueNode* valueNode);
    ExpressionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2_, const MathematicalOperator mathematicalOperator);

    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<ValueNode>& getValueNode1() const;
    const std::unique_ptr<ValueNode>& getValueNode2() const;
    const std::optional<MathematicalOperator>& getMathematicalOperator() const;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::unique_ptr<ValueNode> valueNode2_;
    const std::optional<MathematicalOperator> mathematicalOperator_;
};

#endif // EXPRESSION_NODE_HPP