#ifndef ASSIGNMENT_NODE_HPP
#define ASSIGNMENT_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"

class AssignmentNode : public AbstractSyntaxTreeStatementNode {
public:
    AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<IdentifierNode>& getIdentifierNode() const;
    const std::unique_ptr<ExpressionNode>& getExpressionNode() const;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
    const std::unique_ptr<ExpressionNode> expressionNode_;
};

#endif // ASSIGNMENT_NODE_HPP