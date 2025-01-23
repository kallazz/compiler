#ifndef ARGUMENTS_DECLARATION_NODE_HPP
#define ARGUMENTS_DECLARATION_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "enums/ArgumentType.hpp"
#include "structs/ArgumentDeclaration.hpp"
#include <memory>
#include <vector>
#include <string>


class ArgumentsDeclarationNode : public AbstractSyntaxTreeStatementNode {
public:
    ArgumentsDeclarationNode(const int lineNumber);

    void addArgumentDeclaration(const int lineNumber, const std::string name, const bool isArray);

    void accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const override;
    bool accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const override;
    void print() const override;

    const std::vector<ArgumentDeclaration>& getArgumentDeclarations() const;

private:
    std::vector<ArgumentDeclaration> argumentDeclarations_;
};

#endif // ARGUMENTS_DECLARATION_NODE_HPP