#ifndef DECLARATIONS_NODE_HPP
#define DECLARATIONS_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include <optional>
#include <string>
#include <vector>

struct Declaration {
    const std::string name;
    const std::optional<long long> arrayLowerBound;
    const std::optional<long long> arrayUpperBound;
};

class DeclarationsNode : public AbstractSyntaxTreeExpressionNode {
public:
    DeclarationsNode() = default;

    void addDeclaration(const std::string name);
    void addDeclaration(const std::string name, long long arrayLowerBound, long long arrayUpperBound);
    void print() const override;

private:
    std::vector<Declaration> declarations_;
};

#endif // DECLARATIONS_NODE_HPP
