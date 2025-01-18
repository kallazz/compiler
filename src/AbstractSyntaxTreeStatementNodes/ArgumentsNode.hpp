#ifndef ARGUMENTS_NODE_HPP
#define ARGUMENTS_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include <string>
#include <vector>

struct Argument {
    const int lineNumber;
    const std::string name;
};

class ArgumentsNode : public AbstractSyntaxTreeStatementNode {
public:
    ArgumentsNode(const int lineNumber);

    void addArgument(const int lineNumber, const std::string name);
    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    std::vector<Argument> arguments_;
};

#endif // ARGUMENTS_NODE_HPP