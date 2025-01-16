#ifndef IDENTIFIER_NODE_HPP
#define IDENTIFIER_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include <string>
#include <optional>

class IdentifierNode : public AbstractSyntaxTreeExpressionNode {
public:
    IdentifierNode(const std::string name);
    IdentifierNode(const std::string name, const std::string indexName);
    IdentifierNode(const std::string name, const long long indexValue);

    void print() const override;

private:
    const std::string name_;
    const std::optional<std::string> indexName_;
    const std::optional<long long> indexValue_;
};

#endif // IDENTIFIER_NODE_HPP