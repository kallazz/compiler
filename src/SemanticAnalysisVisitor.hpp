#ifndef SEMANTIC_ANALYSIS_VISITOR_HPP
#define SEMANTIC_ANALYSIS_VISITOR_HPP

#include "SymbolTable.hpp"
#include <optional>
#include <string>

class ConditionNode;
class ExpressionNode;
class IdentifierNode;
class ValueNode;
class ArgumentsDeclarationNode;
class ArgumentsNode;
class AssignmentNode;
class CommandsNode;
class DeclarationsNode;
class ForLoopNode;
class IfNode;
class MainNode;
class ProcedureCallNode;
class ProcedureHeadNode;
class ProceduresNode;
class ReadNode;
class RepeatLoopNode;
class WhileLoopNode;
class WriteNode;

class SemanticAnalysisVisitor {
public:
    SemanticAnalysisVisitor(SymbolTable& symbolTable);

    bool visitConditionNode(const ConditionNode& conditionNode);
    bool visitExpressionNode(const ExpressionNode& expressionNode);
    bool visitIdentifierNode(const IdentifierNode& identifierNode);
    bool visitValueNode(const ValueNode& valueNode);

    bool visitArgumentsDeclarationNode(const ArgumentsDeclarationNode&);
    bool visitArgumentsNode(const ArgumentsNode&);
    bool visitAssignmentNode(const AssignmentNode& assignmentNode);
    bool visitCommandsNode(const CommandsNode& commandsNode);
    bool visitDeclarationsNode(const DeclarationsNode& declarationsNode);
    bool visitForLoopNode(const ForLoopNode& forLoopNode);
    bool visitIfNode(const IfNode& ifNode);
    bool visitMainNode(const MainNode& mainNode);
    bool visitProcedureCallNode(const ProcedureCallNode& procedureCallNode);
    bool visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode);
    bool visitProceduresNode(const ProceduresNode& proceduresNode);
    bool visitReadNode(const ReadNode& readNode);
    bool visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode);
    bool visitWhileLoopNode(const WhileLoopNode& whileLoopNode);
    bool visitWriteNode(const WriteNode& writeNode);

private:
    bool isProcedureBeingProcessed();

    SymbolTable& symbolTable_;
    std::optional<std::string> currentProcedureName_;
};

#endif // SEMANTIC_ANALYSIS_VISITOR_HPP