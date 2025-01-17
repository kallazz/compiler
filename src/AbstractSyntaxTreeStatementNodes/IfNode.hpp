#ifndef IF_NODE_HPP
#define IF_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include <memory>

class IfNode : public AbstractSyntaxTreeStatementNode {
public:
    IfNode(ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode = nullptr);

    void print() const override;

private:
    const std::unique_ptr<ConditionNode> conditionNode_;
    const std::unique_ptr<CommandsNode> thenCommandsNode_;
    const std::unique_ptr<CommandsNode> elseCommandsNode_;
};

#endif // IF_NODE_HPP