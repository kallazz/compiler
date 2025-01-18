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
    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    std::vector<Declaration> declarations_;
};

#endif // DECLARATIONS_NODE_HPP
