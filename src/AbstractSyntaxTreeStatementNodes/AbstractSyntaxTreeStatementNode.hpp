#ifndef ABSTRACT_SYNTAX_TREE_STATEMENT_NODE
#define ABSTRACT_SYNTAX_TREE_STATEMENT_NODE

class AbstractSyntaxTreeStatementNode {
public:
    virtual ~AbstractSyntaxTreeStatementNode() = default;

    virtual void print() const = 0;
};

#endif // ABSTRACT_SYNTAX_TREE_STATEMENT_NODE