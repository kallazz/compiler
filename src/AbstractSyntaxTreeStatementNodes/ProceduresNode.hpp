#ifndef PROCEDURES_NODE_HPP
#define PROCEDURES_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "DeclarationsNode.hpp"
#include "ProcedureHeadNode.hpp"
#include <memory>
#include <vector>

struct Procedure {
    const std::unique_ptr<ProcedureHeadNode> procedureHeadNode;
    const std::unique_ptr<DeclarationsNode> declarationsNode;
    const std::unique_ptr<CommandsNode> commandsNode;

    Procedure(ProcedureHeadNode* procedureHeadNode, DeclarationsNode* declarationsNode, CommandsNode* commandsNode)
        : procedureHeadNode(std::move(procedureHeadNode)),
          declarationsNode(std::move(declarationsNode)),
          commandsNode(std::move(commandsNode)) {}
};

class ProceduresNode : public AbstractSyntaxTreeStatementNode {
public:
    ProceduresNode(const int lineNumber);

    void addProcedure(ProcedureHeadNode* procedureHeadNode, DeclarationsNode* declarationsNode, CommandsNode* commandsNode);
    void addProcedure(ProcedureHeadNode* procedureHeadNode, CommandsNode* commandsNode);

    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::vector<std::unique_ptr<Procedure>>& getProcedures() const;

private:
    std::vector<std::unique_ptr<Procedure>> procedures_;
};

#endif // PROCEDURES_NODE_HPP
