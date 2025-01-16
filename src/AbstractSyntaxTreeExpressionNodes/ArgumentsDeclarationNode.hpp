#ifndef ARGUMENTS_DECLARATION_NODE_HPP
#define ARGUMENTS_DECLARATION_NODE_HPP

#include "AbstractSyntaxTree.hpp"
#include <memory>
#include <vector>
#include <string>

struct ArgumentDeclaration {
    const std::string name;
    const bool isArray;
};

class ArgumentsDeclarationNode : public AbstractSyntaxTreeExpressionNode {
public:
    ArgumentsDeclarationNode() = default;

    void addArgumentDeclaration(const std::string name, const bool isArray);
    void print() const override;

private:
    std::vector<ArgumentDeclaration> argumentDeclarations_;
};

#endif // ARGUMENTS_DECLARATION_NODE_HPP