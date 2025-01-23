#ifndef COMMANDS_NODE_HPP
#define COMMANDS_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include <memory>
#include <vector>

class SemanticAnalysisVisitor;

class CommandsNode : public AbstractSyntaxTreeStatementNode {
public:
    CommandsNode(const int lineNumber);

    void addCommand(AbstractSyntaxTreeStatementNode* commandNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::vector<std::unique_ptr<AbstractSyntaxTreeStatementNode>>& getCommandNodes() const;

private:
    std::vector<std::unique_ptr<AbstractSyntaxTreeStatementNode>> commandNodes_;
};

#endif // COMMANDS_NODE_HPP