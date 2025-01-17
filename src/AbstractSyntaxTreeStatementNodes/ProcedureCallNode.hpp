#ifndef PROCEDURE_CALL_NODE_HPP
#define PROCEDURE_CALL_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ArgumentsNode.hpp"
#include <string>
#include <memory>

class ProcedureCallNode : public AbstractSyntaxTreeStatementNode {
public:
    ProcedureCallNode(const std::string name, ArgumentsNode* argumentsNode);

    void print() const override;

private:
    const std::string name_;
    const std::unique_ptr<ArgumentsNode> argumentsNode_;
};

#endif // PROCEDURE_CALL_NODE_HPP
