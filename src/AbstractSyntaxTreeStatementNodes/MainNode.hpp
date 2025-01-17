#ifndef MAIN_NODE_HPP
#define MAIN_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "DeclarationsNode.hpp"
#include <memory>

class MainNode : public AbstractSyntaxTreeStatementNode {
public:
    MainNode(DeclarationsNode* declarationsNode, CommandsNode* commandsNode);
    MainNode(CommandsNode* commandsNode);

    void print() const override;

private:
    const std::unique_ptr<DeclarationsNode> declarationsNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
};

#endif // MAIN_NODE_HPP