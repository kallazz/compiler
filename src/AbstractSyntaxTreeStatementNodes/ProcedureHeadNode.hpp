#ifndef PROCEDURE_HEAD_NODE_HPP
#define PROCEDURE_HEAD_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "ArgumentsDeclarationNode.hpp"
#include <memory>
#include <string>

class ProcedureHeadNode : public AbstractSyntaxTreeStatementNode {
public:
    ProcedureHeadNode(const int lineNumber, const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode);

    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    const std::string name_;
    const std::unique_ptr<ArgumentsDeclarationNode> argumentsDeclarationNode_;
};

#endif // PROCEDURE_HEAD_NODE_HPP
