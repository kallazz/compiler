#ifndef READ_NODE_HPP
#define READ_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include <memory>

class ReadNode : public AbstractSyntaxTreeStatementNode {
public:
    ReadNode(IdentifierNode* identifierNode);

    void print() const override;

private:
    const std::unique_ptr<IdentifierNode> identifierNode_;
};

#endif // READ_NODE_HPP