#ifndef READ_NODE_HPP
#define READ_NODE_HPP

#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include "AbstractSyntaxTreeStatementNode.hpp"

#include <memory>

class ReadNode : public AbstractSyntaxTreeStatementNode {
public:
    ReadNode(const int lineNumber, IdentifierNode* identifierNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<IdentifierNode>& getIdentifierNode() const;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
};

#endif // READ_NODE_HPP