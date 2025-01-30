#ifndef WRITE_NODE_HPP
#define WRITE_NODE_HPP

#include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"
#include "AbstractSyntaxTreeStatementNode.hpp"

#include <memory>

class WriteNode : public AbstractSyntaxTreeStatementNode {
public:
    WriteNode(const int lineNumber, ValueNode* valueNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::unique_ptr<ValueNode>& getValueNode() const;

private:
    const std::unique_ptr<ValueNode> valueNode_;
};

#endif  // WRITE_NODE_HPP
