#ifndef ABSTRACT_SYNTAX_TREE_STATEMENT_NODE
#define ABSTRACT_SYNTAX_TREE_STATEMENT_NODE

class SymbolTable;

class AbstractSyntaxTreeStatementNode {
public:
    AbstractSyntaxTreeStatementNode(const int lineNumber) : lineNumber_(lineNumber) {}
    virtual ~AbstractSyntaxTreeStatementNode() = default;

    int getLineNumber() const { return lineNumber_; }
    virtual bool evaluateBySymbolTable(SymbolTable&) const = 0;
    virtual void print() const = 0;

private:
    const int lineNumber_;
};

#endif // ABSTRACT_SYNTAX_TREE_STATEMENT_NODE