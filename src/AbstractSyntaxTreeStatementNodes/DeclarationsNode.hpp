#ifndef DECLARATIONS_NODE_HPP
#define DECLARATIONS_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include <optional>
#include <string>
#include <vector>

struct Declaration {
    const int lineNumber;
    const std::string name;
    const std::optional<long long> arrayLowerBound;
    const std::optional<long long> arrayUpperBound;
};

class DeclarationsNode : public AbstractSyntaxTreeStatementNode {
public:
    DeclarationsNode(const int lineNumber);

    void addDeclaration(const int lineNumber, const std::string name);
    void addDeclaration(const int lineNumber, const std::string name, long long arrayLowerBound, long long arrayUpperBound);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::vector<Declaration>& getDeclarations() const;

private:
    std::vector<Declaration> declarations_;
};

#endif // DECLARATIONS_NODE_HPP
