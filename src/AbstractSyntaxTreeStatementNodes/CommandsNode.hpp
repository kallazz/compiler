#ifndef COMMANDS_NODE_HPP
#define COMMANDS_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include <memory>
#include <vector>

class CommandsNode : public AbstractSyntaxTreeStatementNode {
public:
    CommandsNode() = default;

    void addCommand(AbstractSyntaxTreeStatementNode* commandNode);
    void print() const override;
private:
    std::vector<std::unique_ptr<AbstractSyntaxTreeStatementNode>> commandNodes_;
};

#endif // COMMANDS_NODE_HPP