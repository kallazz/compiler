#ifndef VALUE_NODE_HPP
#define VALUE_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"
#include "IdentifierNode.hpp"

#include <memory>
#include <optional>

class ValueNode : public AbstractSyntaxTreeExpressionNode {
public:
    ValueNode(const int lineNumber, const long long number);
    ValueNode(const int lineNumber, IdentifierNode* identifierNode);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::optional<long long>& getNumber() const;
    const std::unique_ptr<IdentifierNode>& getIdentifierNode() const;

private:
    const std::optional<long long> number_;
    const std::unique_ptr<IdentifierNode> identifierNode_;
};

#endif // VALUE_NODE_HPP