#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ValueNode.hpp"
#include "enums/ComparsionOperator.hpp"
#include <memory>

class ConditionNode : public AbstractSyntaxTreeExpressionNode {
public:
    ConditionNode(const int lineNumber, ValueNode* valueNode1, ValueNode* valueNode2, const ComparsionOperator comparsionOperator);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::unique_ptr<ValueNode> valueNode1_;
    const std::unique_ptr<ValueNode> valueNode2_;
    const ComparsionOperator comparsionOperator_;
};

#endif // CONDITION_NODE_HPP