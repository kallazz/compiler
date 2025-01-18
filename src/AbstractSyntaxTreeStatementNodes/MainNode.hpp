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

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::unique_ptr<DeclarationsNode> declarationsNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
};

#endif // MAIN_NODE_HPP