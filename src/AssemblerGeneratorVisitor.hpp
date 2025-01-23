#ifndef ASSEMBLER_GENERATOR_VISITOR_HPP
#define ASSEMBLER_GENERATOR_VISITOR_HPP

#include <fstream>

class SymbolTable;

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

class AssemblerGeneratorVisitor {
public:
    AssemblerGeneratorVisitor(const SymbolTable& symbolTable, std::ofstream& outputFile);

    void visitConditionNode(const ConditionNode& conditionNode);
    void visitExpressionNode(const ExpressionNode& expressionNode);
    void visitIdentifierNode(const IdentifierNode& identifierNode);
    void visitValueNode(const ValueNode& valueNode);

    void visitArgumentsDeclarationNode(const ArgumentsDeclarationNode&);
    void visitArgumentsNode(const ArgumentsNode&);
    void visitAssignmentNode(const AssignmentNode& assignmentNode);
    void visitCommandsNode(const CommandsNode& commandsNode);
    void visitDeclarationsNode(const DeclarationsNode& declarationsNode);
    void visitForLoopNode(const ForLoopNode& forLoopNode);
    void visitIfNode(const IfNode& ifNode);
    void visitMainNode(const MainNode& mainNode);
    void visitProcedureCallNode(const ProcedureCallNode& procedureCallNode);
    void visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode);
    void visitProceduresNode(const ProceduresNode& proceduresNode);
    void visitReadNode(const ReadNode& readNode);
    void visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode);
    void visitWhileLoopNode(const WhileLoopNode& whileLoopNode);
    void visitWriteNode(const WriteNode& writeNode);

private:
    void writeLineToOutputFile(const std::string& text);

    std::ofstream& outputFile_;

};

#endif // ASSEMBLER_GENERATOR_VISITOR_HPP
