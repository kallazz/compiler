#ifndef WHILE_LOOP_NODE_HPP
#define WHILE_LOOP_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include <memory>

class WhileLoopNode : public AbstractSyntaxTreeStatementNode {
public:
    WhileLoopNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* commandsNode);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::unique_ptr<ConditionNode> conditionNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
};

#endif // WHILE_LOOP_NODE_HPP