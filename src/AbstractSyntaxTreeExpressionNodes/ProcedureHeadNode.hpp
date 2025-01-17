#ifndef PROCEDURE_HEAD_NODE_HPP
#define PROCEDURE_HEAD_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "ArgumentsDeclarationNode.hpp"
#include <memory>
#include <string>

class ProcedureHeadNode : public AbstractSyntaxTreeExpressionNode {
public:
    ProcedureHeadNode(const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode);

    void print() const override;

private:
    const std::string name_;
    const std::unique_ptr<ArgumentsDeclarationNode> argumentsDeclarationNode_;
};

#endif // PROCEDURE_HEAD_NODE_HPP
