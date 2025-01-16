#ifndef ARGUMENTS_NODE_HPP
#define ARGUMENTS_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include <string>

class ArgumentsNode : public AbstractSyntaxTreeExpressionNode {
public:
    ArgumentsNode() = default;

    void addArgument(const std::string name);
    void print() const override;

private:
    std::vector<std::string> argumentNames_;
};

#endif // ARGUMENTS_NODE_HPP