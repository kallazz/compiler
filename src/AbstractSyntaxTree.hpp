#ifndef ABSTRACT_SYNTAX_TREE
#define ABSTRACT_SYNTAX_TREE

#include "AbstractSyntaxTreeStatementNodes/MainNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ProceduresNode.hpp"
#include "SymbolTable.hpp"

#include <memory>

class AbstractSyntaxTree {
public:
    AbstractSyntaxTree();

    void setProceduresNode(ProceduresNode* proceduresNode);
    void setMainNode(MainNode* mainNode);
    void setIsMultiplicationProcedureNeeded(const bool value);
    void setIsDivisionProcedureNeeded(const bool value);
    void setIsModuloProcedureNeeded(const bool value);
    bool fillSymbolTable();
    std::string generateAssembler() const;
    CompilationError getCompilationError() const;

private:
    std::unique_ptr<ProceduresNode> proceduresNode_;
    std::unique_ptr<MainNode> mainNode_;
    SymbolTable symbolTable_;
    CompilationError compilationError_;
    bool isMultiplicationProcedureNeeded_;
    bool isDivisionProcedureNeeded_;
    bool isModuloProcedureNeeded_;
};

#endif  // ABSTRACT_SYNTAX_TREE
