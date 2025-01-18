#ifndef VALUE_NODE_HPP
#define VALUE_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "IdentifierNode.hpp"
#include <memory>
#include <optional>

class ValueNode : public AbstractSyntaxTreeExpressionNode {
public:
    ValueNode(const int lineNumber, const long long number);
    ValueNode(const int lineNumber, IdentifierNode* identifierNode);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::optional<long long> number_;
    const std::optional<std::unique_ptr<IdentifierNode>> identifierNode_;
};

#endif // VALUE_NODE_HPP