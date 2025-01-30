#ifndef WHILE_LOOP_NODE_HPP
#define WHILE_LOOP_NODE_HPP

#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"

#include <memory>

class WhileLoopNode : public AbstractSyntaxTreeStatementNode {
public:
    WhileLoopNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* commandsNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<ConditionNode>& getConditionNode() const;
    const std::unique_ptr<CommandsNode>& getCommandsNode() const;

private:
    const std::unique_ptr<ConditionNode> conditionNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
};

#endif  // WHILE_LOOP_NODE_HPP
