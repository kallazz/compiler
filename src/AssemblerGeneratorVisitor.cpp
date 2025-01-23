#include "AssemblerGeneratorVisitor.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ConditionNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ExpressionNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/IdentifierNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ArgumentsDeclarationNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ArgumentsNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/AssignmentNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/CommandsNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/DeclarationsNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ForLoopNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/IfNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/MainNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ProcedureCallNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ProcedureHeadNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ProceduresNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/ReadNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/RepeatLoopNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/WhileLoopNode.hpp"
#include "AbstractSyntaxTreeStatementNodes/WriteNode.hpp"

AssemblerGeneratorVisitor::AssemblerGeneratorVisitor(const SymbolTable& symbolTable, std::ofstream& outputFile) : outputFile_(outputFile) {}

void AssemblerGeneratorVisitor::visitConditionNode(const ConditionNode& conditionNode) {
    return;
}

void AssemblerGeneratorVisitor::visitExpressionNode(const ExpressionNode& expressionNode) {
    return;
}

void AssemblerGeneratorVisitor::visitIdentifierNode(const IdentifierNode& identifierNode) {
    return;
}

void AssemblerGeneratorVisitor::visitValueNode(const ValueNode& valueNode) {
    return;
}

void AssemblerGeneratorVisitor::visitArgumentsDeclarationNode(const ArgumentsDeclarationNode& argumentsDeclarationNode) {
    return;
}

void AssemblerGeneratorVisitor::visitArgumentsNode(const ArgumentsNode& argumentsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitAssignmentNode(const AssignmentNode& assignmentNode) {
    return;
}

void AssemblerGeneratorVisitor::visitCommandsNode(const CommandsNode& commandsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitDeclarationsNode(const DeclarationsNode& declarationsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitIfNode(const IfNode& ifNode) {
    return;
}

void AssemblerGeneratorVisitor::visitMainNode(const MainNode& mainNode) {
    writeLineToOutputFile("HALT");
}

void AssemblerGeneratorVisitor::visitProcedureCallNode(const ProcedureCallNode& procedureCallNode) {
    return;
}

void AssemblerGeneratorVisitor::visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode) {
    return;
}

void AssemblerGeneratorVisitor::visitProceduresNode(const ProceduresNode& proceduresNode) {
    return;
}

void AssemblerGeneratorVisitor::visitReadNode(const ReadNode& readNode) {
    return;
}

void AssemblerGeneratorVisitor::visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitWhileLoopNode(const WhileLoopNode& whileLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitWriteNode(const WriteNode& writeNode) {
    return;
}

void AssemblerGeneratorVisitor::writeLineToOutputFile(const std::string& text) {
    outputFile_ << text << '\n';
}
