#ifndef REPEAT_LOOP_NODE_HPP
#define REPEAT_LOOP_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include <memory>

class RepeatLoopNode : public AbstractSyntaxTreeStatementNode {
public:
    RepeatLoopNode(const int lineNumber, CommandsNode* commandsNode, ConditionNode* conditionNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<CommandsNode>& getCommandsNode() const;
    const std::unique_ptr<ConditionNode>& getConditionNode() const;

private:
    const std::unique_ptr<CommandsNode> commandsNode_;
    const std::unique_ptr<ConditionNode> conditionNode_;
};

#endif // REPEAT_LOOP_NODE_HPP