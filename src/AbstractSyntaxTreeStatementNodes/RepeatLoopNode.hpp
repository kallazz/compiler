#ifndef REPEAT_LOOP_NODE_HPP
#define REPEAT_LOOP_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include <memory>

class RepeatLoopNode : public AbstractSyntaxTreeStatementNode {
public:
    RepeatLoopNode(CommandsNode* commandsNode, ConditionNode* conditionNode);

    void print() const override;

private:
    const std::unique_ptr<CommandsNode> commandsNode_;
    const std::unique_ptr<ConditionNode> conditionNode_;
};

#endif // REPEAT_LOOP_NODE_HPP