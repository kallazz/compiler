#ifndef ASSIGNMENT_NODE_HPP
#define ASSIGNMENT_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"

class AssignmentNode : public AbstractSyntaxTreeStatementNode {
public:
    AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
    const std::unique_ptr<ExpressionNode> expressionNode_;
};

#endif // ASSIGNMENT_NODE_HPP