#ifndef ASSEMBLER_GENERATOR_VISITOR_HPP
#define ASSEMBLER_GENERATOR_VISITOR_HPP

#include <memory>
#include <optional>
#include <string>

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

enum class MathematicalOperator;

struct ConditionJumpInfo {
    std::string jumpType;
    bool isConditionTrueAfterJump;
};

class AssemblerGeneratorVisitor {
public:
    AssemblerGeneratorVisitor(const SymbolTable& symbolTable);

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

    std::string getGeneratedAssemblerCode() const;

private:
    void addOrSubtract(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2, const MathematicalOperator mathematicalOperator);
    void writeToOutputFile(const std::string& text);
    void writeLineToOutputFile(const std::string& text);
    void resolveLabels();

    const SymbolTable& symbolTable_;
    std::string outputAssemblerCode_;
    long long currentIdentifierAddress_;
    bool isIdentifierAddressPointer_;
    std::optional<long long> currentValue_;
    std::string currentConditionJumpType_;
    int unresolvedJumpsCounter_;
};

#endif // ASSEMBLER_GENERATOR_VISITOR_HPP
