#ifndef PROCEDURE_HEAD_NODE_HPP
#define PROCEDURE_HEAD_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "ArgumentsDeclarationNode.hpp"

#include <memory>
#include <string>

class ProcedureHeadNode : public AbstractSyntaxTreeStatementNode {
public:
    ProcedureHeadNode(const int lineNumber, const std::string name, ArgumentsDeclarationNode* argumentsDeclarationNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::string& getName() const;
    const std::unique_ptr<ArgumentsDeclarationNode>& getArgumentsDeclarationNode() const;

private:
    const std::string name_;
    const std::unique_ptr<ArgumentsDeclarationNode> argumentsDeclarationNode_;
};

#endif  // PROCEDURE_HEAD_NODE_HPP
