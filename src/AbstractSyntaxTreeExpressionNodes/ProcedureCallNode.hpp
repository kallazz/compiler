#ifndef PROCEDURE_CALL_NODE_HPP
#define PROCEDURE_CALL_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include "ArgumentsNode.hpp"
#include <string>
#include <memory>

class ProcedureCallNode : public AbstractSyntaxTreeExpressionNode {
public:
    ProcedureCallNode(const std::string name, ArgumentsNode* argumentsNode);

    void print() const override;

private:
    std::string name_;
    std::unique_ptr<ArgumentsNode> argumentsNode_;
};

#endif // PROCEDURE_CALL_NODE_HPP
