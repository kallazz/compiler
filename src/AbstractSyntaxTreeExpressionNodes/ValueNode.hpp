#ifndef VALUE_NODE_HPP
#define VALUE_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include "IdentifierNode.hpp"
#include <string>
#include <optional>

class ValueNode : public AbstractSyntaxTreeExpressionNode {
public:
    ValueNode(const long long number);
    ValueNode(const std::shared_ptr<IdentifierNode> identifierNode);

    void print() const override;

private:
    const std::optional<long long> number_;
    const std::optional<std::shared_ptr<IdentifierNode>> identifierNode_;
};

#endif // VALUE_NODE_HPP