#ifndef ABSTRACT_SYNTAX_TREE
#define ABSTRACT_SYNTAX_TREE

#include "AbstractSyntaxTreeStatementNodes/MainNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ProceduresNode.hpp"
#include <memory>

class AbstractSyntaxTree {
public:
    AbstractSyntaxTree() = default;

    void setProceduresNode(ProceduresNode* proceduresNode);
    void setMainNode(MainNode* mainNode);
    void printNodes() const;

private:
    std::unique_ptr<ProceduresNode> proceduresNode_;
    std::unique_ptr<MainNode> mainNode_;
};

#endif // ABSTRACT_SYNTAX_TREE




