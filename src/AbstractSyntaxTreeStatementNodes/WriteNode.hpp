#ifndef WRITE_NODE_HPP
#define WRITE_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"
#include <memory>

class WriteNode : public AbstractSyntaxTreeStatementNode {
public:
    WriteNode(const int lineNumber, ValueNode* valueNode);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::unique_ptr<ValueNode> valueNode_;
};

#endif // WRITE_NODE_HPP