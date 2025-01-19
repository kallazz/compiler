#ifndef IF_NODE_HPP
#define IF_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include <memory>

class IfNode : public AbstractSyntaxTreeStatementNode {
public:
    IfNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode = nullptr);

    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<ConditionNode>& getConditionNode() const;
    const std::unique_ptr<CommandsNode>& getThenCommandsNode() const;
    const std::unique_ptr<CommandsNode>& getElseCommandsNode() const;

private:
    const std::unique_ptr<ConditionNode> conditionNode_;
    const std::unique_ptr<CommandsNode> thenCommandsNode_;
    const std::unique_ptr<CommandsNode> elseCommandsNode_;
};

#endif // IF_NODE_HPP