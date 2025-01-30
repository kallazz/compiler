#ifndef ASSIGNMENT_NODE_HPP
#define ASSIGNMENT_NODE_HPP

#include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include "AbstractSyntaxTreeStatementNode.hpp"

class AssignmentNode : public AbstractSyntaxTreeStatementNode {
public:
    AssignmentNode(const int lineNumber, IdentifierNode* identifierNode, ExpressionNode* expressionNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<IdentifierNode>& getIdentifierNode() const;
    const std::unique_ptr<ExpressionNode>& getExpressionNode() const;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
    const std::unique_ptr<ExpressionNode> expressionNode_;
};

#endif // ASSIGNMENT_NODE_HPP