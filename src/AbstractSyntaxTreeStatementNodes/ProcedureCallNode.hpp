#ifndef PROCEDURE_CALL_NODE_HPP
#define PROCEDURE_CALL_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "ArgumentsNode.hpp"

#include <string>
#include <memory>

class ProcedureCallNode : public AbstractSyntaxTreeStatementNode {
public:
    ProcedureCallNode(const int lineNumber, const std::string name, ArgumentsNode* argumentsNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::string& getName() const;
    const std::unique_ptr<ArgumentsNode>& getArgumentsNode() const;

private:
    const std::string name_;
    const std::unique_ptr<ArgumentsNode> argumentsNode_;
};

#endif  // PROCEDURE_CALL_NODE_HPP
