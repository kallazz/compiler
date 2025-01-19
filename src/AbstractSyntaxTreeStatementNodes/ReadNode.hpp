#ifndef READ_NODE_HPP
#define READ_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include <memory>

class ReadNode : public AbstractSyntaxTreeStatementNode {
public:
    ReadNode(const int lineNumber, IdentifierNode* identifierNode);

    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::unique_ptr<IdentifierNode>& getIdentifierNode() const;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
};

#endif // READ_NODE_HPP