#ifndef MAIN_NODE_HPP
#define MAIN_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "DeclarationsNode.hpp"

#include <memory>

class MainNode : public AbstractSyntaxTreeStatementNode {
public:
    MainNode(const int lineNumber, DeclarationsNode* declarationsNode, CommandsNode* commandsNode);
    MainNode(const int lineNumber, CommandsNode* commandsNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<DeclarationsNode>& getDeclarationsNode() const;
    const std::unique_ptr<CommandsNode>& getCommandsNode() const;

private:
    const std::unique_ptr<DeclarationsNode> declarationsNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
};

#endif // MAIN_NODE_HPP