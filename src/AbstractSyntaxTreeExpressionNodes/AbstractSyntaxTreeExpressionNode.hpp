#ifndef ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE
#define ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE

#include "SemanticAnalysisVisitor.hpp"

class AbstractSyntaxTreeExpressionNode {
public:
    AbstractSyntaxTreeExpressionNode(const int lineNumber) : lineNumber_(lineNumber) {}
    virtual ~AbstractSyntaxTreeExpressionNode() = default;

    int getLineNumber() const { return lineNumber_; }

    virtual bool accept(SemanticAnalysisVisitor&) const = 0;
    virtual void print() const = 0;

private:
    const int lineNumber_;
};

#endif // ABSTRACT_SYNTAX_TREE_EXPRESSION_NODE
