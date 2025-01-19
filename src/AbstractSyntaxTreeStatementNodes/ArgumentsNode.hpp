#ifndef ARGUMENTS_NODE_HPP
#define ARGUMENTS_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "structs/Argument.hpp"
#include <string>
#include <vector>


class ArgumentsNode : public AbstractSyntaxTreeStatementNode {
public:
    ArgumentsNode(const int lineNumber);

    void addArgument(const int lineNumber, const std::string name);

    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::vector<Argument>& getArguments() const;

private:
    std::vector<Argument> arguments_;
};

#endif // ARGUMENTS_NODE_HPP