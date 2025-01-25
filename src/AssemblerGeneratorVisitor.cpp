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
#include "SymbolTable.hpp"
#include <sstream>

const std::string FILL_LABEL = "__";
const std::string RESULT_LABEL = "!!";

/*
Registers' purpose:
    p0 - accumulator
    p1, p2, p3 - help during calculations
    the rest - storing variables
*/

AssemblerGeneratorVisitor::AssemblerGeneratorVisitor(const SymbolTable& symbolTable)
    : symbolTable_(symbolTable), outputAssemblerCode_(""), currentIdentifierAddress_(-1), isIdentifierAddressPointer_(false), currentValue_(std::nullopt), currentConditionJumpType_(""), unresolvedJumpsCounter_(0) {}

void AssemblerGeneratorVisitor::visitConditionNode(const ConditionNode& conditionNode) {
    // TODO: Repair
    addOrSubtract(conditionNode.getValueNode1(), conditionNode.getValueNode2(), MathematicalOperator::SUBTRACT);
    switch (conditionNode.getComparisonOperator()) {
        case ComparsionOperator::EQUAL:
            currentConditionJumpType_= "JZERO";
            break;
        case ComparsionOperator::NOT_EQUAL:
            currentConditionJumpType_= "JZERO";
            break;
        case ComparsionOperator::GREATER_THAN:
            currentConditionJumpType_= "JPOS";
            break;
        case ComparsionOperator::LESS_THAN:
            currentConditionJumpType_= "JNEG";
            break;
        case ComparsionOperator::GREATER_THAN_OR_EQUAL:
            currentConditionJumpType_= "JNEG";
            break;
        case ComparsionOperator::LESS_THAN_OR_EQUAL:
            currentConditionJumpType_= "JPOS";
            break;
    }
}

void AssemblerGeneratorVisitor::visitExpressionNode(const ExpressionNode& expressionNode) {
    if (!expressionNode.getValueNode2()) {
        expressionNode.getValueNode1()->accept(*this);
        if (currentValue_) {
            writeLineToOutputFile("SET " + std::to_string(*currentValue_));
        } else {
            writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
        }

        return;
    }

    if (expressionNode.getMathematicalOperator() == MathematicalOperator::ADD || expressionNode.getMathematicalOperator() == MathematicalOperator::SUBTRACT) {
        addOrSubtract(expressionNode.getValueNode1(), expressionNode.getValueNode2(), *expressionNode.getMathematicalOperator());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::MULTIPLY) {
        // TODO: Add logic
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::DIVIDE) {
        // TODO: Add logic
    }
}

void AssemblerGeneratorVisitor::visitIdentifierNode(const IdentifierNode& identifierNode) {
    // TODO: Add procedure logic
    if (!identifierNode.getIndexName() && !identifierNode.getIndexValue()) {
        currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(identifierNode.getName());
        isIdentifierAddressPointer_ = false;
    } else if (identifierNode.getIndexValue()) {
        currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(identifierNode.getName()) + *identifierNode.getIndexValue();
        isIdentifierAddressPointer_ = false;
    } else if (identifierNode.getIndexName()) {
        const long long arrayAddress = symbolTable_.getVariableAddressInMain(identifierNode.getName());
        const long long arrayIndexNameAddress = symbolTable_.getVariableAddressInMain(*identifierNode.getIndexName());
        writeLineToOutputFile("SET " + std::to_string(arrayAddress));
        writeLineToOutputFile("ADD " + std::to_string(arrayIndexNameAddress));
        currentIdentifierAddress_ = 0;
        isIdentifierAddressPointer_ = true;
    }
}

void AssemblerGeneratorVisitor::visitValueNode(const ValueNode& valueNode) {
    currentValue_ = valueNode.getNumber();
    if (valueNode.getIdentifierNode()) {
        valueNode.getIdentifierNode()->accept(*this);
    }
}

void AssemblerGeneratorVisitor::visitArgumentsDeclarationNode(const ArgumentsDeclarationNode& argumentsDeclarationNode) {
    return;
}

void AssemblerGeneratorVisitor::visitArgumentsNode(const ArgumentsNode& argumentsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitAssignmentNode(const AssignmentNode& assignmentNode) {
    // TODO: Add procedure logic
    std::string storeCommand = "STORE";

    assignmentNode.getIdentifierNode()->accept(*this);
    long long identifierAddress = currentIdentifierAddress_;
    if (isIdentifierAddressPointer_) {
        storeCommand += "I";
        writeLineToOutputFile("STORE 3");
        identifierAddress = 3;
    }

    assignmentNode.getExpressionNode()->accept(*this);

    writeLineToOutputFile(storeCommand + " " + std::to_string(identifierAddress));
}

void AssemblerGeneratorVisitor::visitCommandsNode(const CommandsNode& commandsNode) {
    for (const auto& command : commandsNode.getCommandNodes()) {
        command->accept(*this);
    }
}

void AssemblerGeneratorVisitor::visitDeclarationsNode(const DeclarationsNode& declarationsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitIfNode(const IfNode& ifNode) {
    ifNode.getConditionNode()->accept(*this);

    const int jumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    if (!ifNode.getElseCommandsNode()) {
        writeLineToOutputFile(currentConditionJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        ifNode.getThenCommandsNode()->accept(*this);
        writeToOutputFile(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);
    } else {
        writeLineToOutputFile(currentConditionJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        ifNode.getThenCommandsNode()->accept(*this);
        const int jumpOverElseIndex = unresolvedJumpsCounter_;
        unresolvedJumpsCounter_++;
        writeLineToOutputFile("JUMP __" + std::to_string(jumpOverElseIndex) + FILL_LABEL);
        writeToOutputFile(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);
        ifNode.getElseCommandsNode()->accept(*this);
        writeToOutputFile(RESULT_LABEL + std::to_string(jumpOverElseIndex) + RESULT_LABEL);
    }
}

void AssemblerGeneratorVisitor::visitMainNode(const MainNode& mainNode) {
    mainNode.getCommandsNode()->accept(*this);
    resolveLabels();
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
    readNode.getIdentifierNode()->accept(*this);
    if (isIdentifierAddressPointer_) {
        writeLineToOutputFile("STORE 3");
        writeLineToOutputFile("GET 0");
        writeLineToOutputFile("STOREI 3");
    } else {
        writeLineToOutputFile("GET " + std::to_string(currentIdentifierAddress_));
    }
}

void AssemblerGeneratorVisitor::visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitWhileLoopNode(const WhileLoopNode& whileLoopNode) {
    return;
}

void AssemblerGeneratorVisitor::visitWriteNode(const WriteNode& writeNode) {
    writeNode.getValueNode()->accept(*this);
    if (currentValue_) {
        writeLineToOutputFile("SET " + std::to_string(*currentValue_));
        writeLineToOutputFile("PUT 0");
    } else {
        if (isIdentifierAddressPointer_) {
            writeLineToOutputFile("LOADI 0");
            writeLineToOutputFile("PUT 0");
        } else {
            writeLineToOutputFile("PUT " + std::to_string(currentIdentifierAddress_));
        }
    }
}

void AssemblerGeneratorVisitor::addOrSubtract(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2, const MathematicalOperator mathematicalOperator) {
    const bool isSubtraction = (mathematicalOperator == MathematicalOperator::SUBTRACT);
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode1->accept(*this);
    const std::optional<long long> firstValue = currentValue_;
    long long firstAddress = currentIdentifierAddress_;
    if (!firstValue && isIdentifierAddressPointer_) {
        isFirstAddressPointer = true;
        writeLineToOutputFile("STORE 3");
        firstAddress = 3;
    }

    valueNode2->accept(*this);
    std::optional<long long> secondValue = currentValue_;
    long long secondAddress = currentIdentifierAddress_;
    if (!secondValue && isIdentifierAddressPointer_) {
        isSecondAddressPointer = true;
        writeLineToOutputFile("STORE 2");
        secondAddress = 2;
    }

    if (firstValue && secondValue) {
        writeLineToOutputFile("SET " + std::to_string(*firstValue));
        writeLineToOutputFile("STORE 1");

        if (isSubtraction) {
            secondValue = *secondValue * (-1);
        }

        writeLineToOutputFile("SET " + std::to_string(*secondValue));
        writeLineToOutputFile("ADD 1");
    } else if (!firstValue && !secondValue) {
        const std::string loadCommand = isFirstAddressPointer ? "LOADI" : "LOAD";
        writeLineToOutputFile(loadCommand + " " + std::to_string(firstAddress));

        std::string mathematicalOperator = isSubtraction ? "SUB" : "ADD";
        if (isSecondAddressPointer) {
            mathematicalOperator += "I";
        }

        writeLineToOutputFile(mathematicalOperator + " " + std::to_string(secondAddress));
    } else {
        if (firstValue) {
            const long long value = *firstValue;
            const long long address = secondAddress;

            writeLineToOutputFile("SET " + std::to_string(value));

            std::string mathematicalOperator = isSubtraction ? "SUB" : "ADD";
            if (isSecondAddressPointer) {
                mathematicalOperator += "I";
            }

            writeLineToOutputFile(mathematicalOperator + " " + std::to_string(address));
        } else {
            const long long value = isSubtraction ? (*secondValue * (-1)) : *secondValue;
            const long long address = firstAddress;

            writeLineToOutputFile("SET " + std::to_string(value));

            const std::string mathematicalOperator = isFirstAddressPointer ? "ADDI" : "ADD";

            writeLineToOutputFile(mathematicalOperator + " " + std::to_string(address));
        }
    }
}

void AssemblerGeneratorVisitor::writeToOutputFile(const std::string& text) {
    outputAssemblerCode_ += text;
}

void AssemblerGeneratorVisitor::writeLineToOutputFile(const std::string& text) {
    outputAssemblerCode_ += text + '\n';
}

std::string resolveResultLabels(const std::string& code, std::unordered_map<int, int>& jumpIndexToResultLabelLineNumber) {
    std::string codeWithoutResultLabels = "";
    std::istringstream codeStream(code);
    std::string line;
    int lineNumber = 0;

    while (std::getline(codeStream, line)) {
        size_t resultLabelPosition = 0;
        while ((resultLabelPosition = line.find(RESULT_LABEL)) != std::string::npos) {
            size_t endResultLabelPosition = line.find(RESULT_LABEL, resultLabelPosition + 2);
            if (endResultLabelPosition != std::string::npos) {
                const int jumpIndex = std::stoi(line.substr(resultLabelPosition + 2, endResultLabelPosition - resultLabelPosition - 2));
                jumpIndexToResultLabelLineNumber[jumpIndex] = lineNumber;
                line.erase(resultLabelPosition, endResultLabelPosition + 2 - resultLabelPosition);
            }
        }

        codeWithoutResultLabels += line + '\n';
        lineNumber++;
    }

    return codeWithoutResultLabels;
}

std::string resolveFillLabels(const std::string& code, const std::unordered_map<int, int>& jumpIndexToResultLabelLineNumber) {
    std::string codeWithFilledLabels = "";
    std::istringstream codeStream(code);
    std::string line;
    int lineNumber = 0;

    while (std::getline(codeStream, line)) {
        size_t fillLabelPosition = 0;
        while ((fillLabelPosition = line.find(FILL_LABEL)) != std::string::npos) {
            size_t endFillLabelPosition = line.find(FILL_LABEL, fillLabelPosition + 2);
            if (endFillLabelPosition != std::string::npos) {
                const int jumpIndex = std::stoi(line.substr(fillLabelPosition + 2, endFillLabelPosition - fillLabelPosition - 2));
                const int lineDifference = jumpIndexToResultLabelLineNumber.at(jumpIndex) - lineNumber;
                line.replace(fillLabelPosition, endFillLabelPosition + 2 - fillLabelPosition, std::to_string(lineDifference));
            }
        }

        codeWithFilledLabels += line + '\n';
        lineNumber++;
    }

    return codeWithFilledLabels;
}

void AssemblerGeneratorVisitor::resolveLabels() {
    std::unordered_map<int, int> jumpIndexToResultLabelLineNumber;
    
    const std::string codeWithoutResultLabels = resolveResultLabels(outputAssemblerCode_, jumpIndexToResultLabelLineNumber);
    const std::string codeWithFilledLabels = resolveFillLabels(codeWithoutResultLabels, jumpIndexToResultLabelLineNumber);

    outputAssemblerCode_ = codeWithFilledLabels;
}

std::string AssemblerGeneratorVisitor::getGeneratedAssemblerCode() const {
    return outputAssemblerCode_;
}
