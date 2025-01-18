#ifndef ARGUMENTS_DECLARATION_NODE_HPP
#define ARGUMENTS_DECLARATION_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include <memory>
#include <vector>
#include <string>

struct ArgumentDeclaration {
    const int lineNumber;
    const std::string name;
    const bool isArray;
};

class ArgumentsDeclarationNode : public AbstractSyntaxTreeStatementNode {
public:
    ArgumentsDeclarationNode(const int lineNumber);

    void addArgumentDeclaration(const int lineNumber, const std::string name, const bool isArray);
    bool evaluateBySymbolTable(SymbolTable& symbolTable) const override;
    void print() const override;

private:
    std::vector<ArgumentDeclaration> argumentDeclarations_;
};

#endif // ARGUMENTS_DECLARATION_NODE_HPP