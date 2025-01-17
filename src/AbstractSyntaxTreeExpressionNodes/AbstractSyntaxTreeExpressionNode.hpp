#ifndef ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE
#define ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE

class AbstractSyntaxTreeExpressionNode {
public:
    virtual ~AbstractSyntaxTreeExpressionNode() = default;

    virtual void print() const = 0;
};

#endif // ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE
