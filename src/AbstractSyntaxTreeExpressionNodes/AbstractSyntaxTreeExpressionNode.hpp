#ifndef ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE
#define ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE

#include <memory>

class AbstractSyntaxTreeExpressionNode {
public:
    virtual void print() const = 0;
};

#endif // ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE