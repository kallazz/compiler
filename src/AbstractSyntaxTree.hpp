#ifndef ABSTRACT_SYNTAX_TREE
#define ABSTRACT_SYNTAX_TREE

#include "AbstractSyntaxTreeExpressionNodes/AbstractSyntaxTreeExpressionNode.hpp"
#include <vector>

class AbstractSyntaxTree {
public:
    AbstractSyntaxTree() = default;

    void addNode(const std::shared_ptr<AbstractSyntaxTreeExpressionNode> node);
    void printNodes() const;

private:
    std::vector<std::shared_ptr<AbstractSyntaxTreeExpressionNode>> nodes_;
};

#endif // ABSTRACT_SYNTAX_TREE




