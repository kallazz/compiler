#ifndef IDENTIFIER_NODE_HPP
#define IDENTIFIER_NODE_HPP

#include "AbstractSyntaxTreeExpressionNode.hpp"

#include <string>
#include <optional>

class IdentifierNode : public AbstractSyntaxTreeExpressionNode {
public:
    IdentifierNode(const int lineNumber, const std::string name);
    IdentifierNode(const int lineNumber, const std::string name, const std::string indexName);
    IdentifierNode(const int lineNumber, const std::string name, const long long indexValue);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;

    const std::string& getName() const;
    const std::optional<std::string>& getIndexName() const;
    const std::optional<long long>& getIndexValue() const;

private:
    const std::string name_;
    const std::optional<std::string> indexName_;
    const std::optional<long long> indexValue_;
};

#endif  // IDENTIFIER_NODE_HPP
