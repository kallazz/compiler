#ifndef ASSEMBLER_GENERATOR_VISITOR_HPP
#define ASSEMBLER_GENERATOR_VISITOR_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

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
    AssemblerGeneratorVisitor(const SymbolTable& symbolTable, const bool isMultiplicationProcedureNeeded,
                              const bool isDivisionProcedureNeeded, const bool isModuloProcedureNeeded);

    void visitConditionNode(const ConditionNode& conditionNode);
    void visitExpressionNode(const ExpressionNode& expressionNode);
    void visitIdentifierNode(const IdentifierNode& identifierNode);
    void visitValueNode(const ValueNode& valueNode);

    void visitArgumentsDeclarationNode(const ArgumentsDeclarationNode&);
    void visitArgumentsNode(const ArgumentsNode&);
    void visitAssignmentNode(const AssignmentNode& assignmentNode);
    void visitCommandsNode(const CommandsNode& commandsNode);
    void visitDeclarationsNode(const DeclarationsNode&);
    void visitForLoopNode(const ForLoopNode& forLoopNode);
    void visitIfNode(const IfNode& ifNode);
    void visitMainNode(const MainNode& mainNode);
    void visitProcedureCallNode(const ProcedureCallNode& procedureCallNode);
    void visitProcedureHeadNode(const ProcedureHeadNode&);
    void visitProceduresNode(const ProceduresNode& proceduresNode);
    void visitReadNode(const ReadNode& readNode);
    void visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode);
    void visitWhileLoopNode(const WhileLoopNode& whileLoopNode);
    void visitWriteNode(const WriteNode& writeNode);

    std::string getGeneratedAssemblerCode() const;

private:
    void setGlobalConstantValues();
    void addOrSubtract(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2,
                       const MathematicalOperator mathematicalOperator);
    void multiply(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2);
    void divide(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2);
    void modulo(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2);
    std::string getVariableNameOrIteratorName(const std::string& variableName);
    void appendToOutputCode(const std::string& text);
    void appendLineToOutputCode(const std::string& text);
    void resolveLabels();
    void generateMultiplicationProcedure();
    void generateDivisionProcedure();
    void generateModuloProcedure();

    const SymbolTable& symbolTable_;
    const bool isMultiplicationProcedureNeeded_;
    const bool isDivisionProcedureNeeded_;
    const bool isModuloProcedureNeeded_;
    std::string outputAssemblerCode_;
    int forLoopsCounter_;
    long long currentIdentifierAddress_;
    bool isCurrentIdentifierAddressPointer_;
    bool isCurrentIdentifierAddressCalculatedPointerForArray_;
    std::string currentJumpType_;
    bool isCurrentJumpForTrueCondition_;
    int unresolvedJumpsCounter_;
    int currentLineNumber_;
    std::optional<std::string> currentProcedureName_;
    std::unordered_map<std::string, std::string> iteratorProgramNameToIteratorInternalName_;
    std::unordered_map<std::string, int> procedureNameToJumpIndex;
};

#endif  // ASSEMBLER_GENERATOR_VISITOR_HPP
