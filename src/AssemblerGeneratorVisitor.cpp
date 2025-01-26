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
    : symbolTable_(symbolTable), shouldWriteComments_(true), outputAssemblerCode_(""), forLoopsCounter_(0), currentIdentifierAddress_(-1), isCurrentIdentifierAddressPointer_(false), currentValue_(std::nullopt), currentJumpType_(""), isCurrentJumpForTrueCondition_(false), unresolvedJumpsCounter_(0) {}

void AssemblerGeneratorVisitor::visitConditionNode(const ConditionNode& conditionNode) {
    addOrSubtract(conditionNode.getValueNode1(), conditionNode.getValueNode2(), MathematicalOperator::SUBTRACT);
    switch (conditionNode.getComparisonOperator()) {
        case ComparsionOperator::EQUAL:
            currentJumpType_= "JZERO";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparsionOperator::NOT_EQUAL:
            currentJumpType_= "JZERO";
            isCurrentJumpForTrueCondition_ = false;
            break;
        case ComparsionOperator::GREATER_THAN:
            currentJumpType_= "JPOS";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparsionOperator::LESS_THAN:
            currentJumpType_= "JNEG";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparsionOperator::GREATER_THAN_OR_EQUAL:
            currentJumpType_= "JNEG";
            isCurrentJumpForTrueCondition_ = false;
            break;
        case ComparsionOperator::LESS_THAN_OR_EQUAL:
            currentJumpType_= "JPOS";
            isCurrentJumpForTrueCondition_ = false;
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
    std::string variableName = identifierNode.getName();

    const auto it = iteratorProgramNameToIteratorInternalName_.find(variableName);
    if (it != iteratorProgramNameToIteratorInternalName_.end()) {
        variableName = it->second;
    }

    if (!identifierNode.getIndexName() && !identifierNode.getIndexValue()) {
        currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(variableName);
        isCurrentIdentifierAddressPointer_ = false;
    } else if (identifierNode.getIndexValue()) {
        currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(variableName) + *identifierNode.getIndexValue();
        isCurrentIdentifierAddressPointer_ = false;
    } else if (identifierNode.getIndexName()) {
        std::string indexName = *identifierNode.getIndexName();

        const auto it = iteratorProgramNameToIteratorInternalName_.find(indexName);
        if (it != iteratorProgramNameToIteratorInternalName_.end()) {
            indexName = it->second;
        }

        const long long arrayAddress = symbolTable_.getVariableAddressInMain(variableName);
        const long long arrayIndexNameAddress = symbolTable_.getVariableAddressInMain(indexName);
        writeLineToOutputFile("SET " + std::to_string(arrayAddress));
        writeLineToOutputFile("ADD " + std::to_string(arrayIndexNameAddress));
        currentIdentifierAddress_ = 0;
        isCurrentIdentifierAddressPointer_ = true;
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
    if (isCurrentIdentifierAddressPointer_) {
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

ValueNode* copyValueNode(const std::unique_ptr<ValueNode>& valueNode) {
    const int lineNumber = valueNode->getLineNumber();
    if (valueNode->getIdentifierNode()) {
        const int identifierNodeLineNumber = valueNode->getIdentifierNode()->getLineNumber();
        const std::string identifierNodeName = valueNode->getIdentifierNode()->getName();

        if (valueNode->getIdentifierNode()->getIndexName()) {
            const std::string identifierNodeIndexName = *valueNode->getIdentifierNode()->getIndexName();
            return new ValueNode(lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName, identifierNodeIndexName));
        }
        if (valueNode->getIdentifierNode()->getIndexValue()) {
            const long long identifierNodeIndexValue = *valueNode->getIdentifierNode()->getIndexValue();
            return new ValueNode(lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName, identifierNodeIndexValue));
        }
        return new ValueNode(lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName));
    }

    const long long number = *valueNode->getNumber(); 

    return new ValueNode(lineNumber, number);
}

void AssemblerGeneratorVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    writeCommentLineToOutputFile("FOR");

    const int currentForLoopIndex = forLoopsCounter_;
    forLoopsCounter_++;

    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    const int afterLoopJumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    const std::string iteratorName = "!IT" + std::to_string(currentForLoopIndex) + forLoopNode.getIteratorName();
    const std::string iteratorBoundName = "!ITBOUND" + std::to_string(currentForLoopIndex) + forLoopNode.getIteratorName();

    iteratorProgramNameToIteratorInternalName_.insert({forLoopNode.getIteratorName(), iteratorName});

    const AssignmentNode iteratorAssignmentNode(0, new IdentifierNode(0, iteratorName), new ExpressionNode(0, copyValueNode(forLoopNode.getStartValueNode())));
    const AssignmentNode iteratorBoundAssignmentNode(0, new IdentifierNode(0, iteratorBoundName), new ExpressionNode(0, copyValueNode(forLoopNode.getEndValueNode())));
    iteratorAssignmentNode.accept(*this);
    iteratorBoundAssignmentNode.accept(*this);

    writeToOutputFile(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL); 

    const ComparsionOperator comparsionOperator = forLoopNode.isIteratorIncremented() ? ComparsionOperator::LESS_THAN_OR_EQUAL : ComparsionOperator::GREATER_THAN_OR_EQUAL;
    const ConditionNode conditionNode(0, new ValueNode(0, new IdentifierNode(0, iteratorName)), new ValueNode(0, new IdentifierNode(0, iteratorBoundName)), comparsionOperator);
    conditionNode.accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        writeLineToOutputFile(currentJumpType_ + " 2");
        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    } else {
        writeLineToOutputFile(currentJumpType_ + " " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    }

    forLoopNode.getCommandsNode()->accept(*this);

    const MathematicalOperator mathematicalOperator = forLoopNode.isIteratorIncremented() ? MathematicalOperator::ADD : MathematicalOperator::SUBTRACT;
    const AssignmentNode iteratorIncrementAssignmentNode(0, new IdentifierNode(0, iteratorName), new ExpressionNode(0, new ValueNode(0, new IdentifierNode(0, iteratorName)), new ValueNode(0, 1), mathematicalOperator));
    iteratorIncrementAssignmentNode.accept(*this);

    writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    writeToOutputFile(RESULT_LABEL + std::to_string(afterLoopJumpIndex) + RESULT_LABEL);

    iteratorProgramNameToIteratorInternalName_.erase(forLoopNode.getIteratorName());

    writeCommentLineToOutputFile("ENDFOR");
}

void AssemblerGeneratorVisitor::visitIfNode(const IfNode& ifNode) {
    writeCommentLineToOutputFile("IF");

    ifNode.getConditionNode()->accept(*this);

    const int jumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    if (!ifNode.getElseCommandsNode()) {
        if (isCurrentJumpForTrueCondition_) {
            writeLineToOutputFile(currentJumpType_ + " 2");
            writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        } else {
            writeLineToOutputFile(currentJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        }

        ifNode.getThenCommandsNode()->accept(*this);
        writeToOutputFile(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);
    } else {
        writeLineToOutputFile(currentJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        if (isCurrentJumpForTrueCondition_) {
            ifNode.getElseCommandsNode()->accept(*this);
        } else {
            ifNode.getThenCommandsNode()->accept(*this);
        }

        const int jumpOverSecondPartIndex = unresolvedJumpsCounter_;
        unresolvedJumpsCounter_++;

        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(jumpOverSecondPartIndex) + FILL_LABEL);
        writeToOutputFile(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);

        if (isCurrentJumpForTrueCondition_) {
            ifNode.getThenCommandsNode()->accept(*this);
        } else {
            ifNode.getElseCommandsNode()->accept(*this);
        }

        writeToOutputFile(RESULT_LABEL + std::to_string(jumpOverSecondPartIndex) + RESULT_LABEL);
    }

    writeCommentLineToOutputFile("ENDIF");
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
    if (isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("STORE 3");
        writeLineToOutputFile("GET 0");
        writeLineToOutputFile("STOREI 3");
    } else {
        writeLineToOutputFile("GET " + std::to_string(currentIdentifierAddress_));
    }
}

void AssemblerGeneratorVisitor::visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode) {
    writeCommentLineToOutputFile("REPEAT");

    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    writeToOutputFile(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL); 

    repeatLoopNode.getCommandsNode()->accept(*this);
    repeatLoopNode.getConditionNode()->accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        writeLineToOutputFile(currentJumpType_ + " 2");
        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    } else {
        writeLineToOutputFile(currentJumpType_ + " " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    }

    writeCommentLineToOutputFile("ENDREPEAT");
}

void AssemblerGeneratorVisitor::visitWhileLoopNode(const WhileLoopNode& whileLoopNode) {
    writeCommentLineToOutputFile("WHILE");

    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    const int afterLoopJumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    writeToOutputFile(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL); 

    whileLoopNode.getConditionNode()->accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        writeLineToOutputFile(currentJumpType_ + " 2");
        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    } else {
        writeLineToOutputFile(currentJumpType_ + " " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    }

    whileLoopNode.getCommandsNode()->accept(*this);

    writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    writeToOutputFile(RESULT_LABEL + std::to_string(afterLoopJumpIndex) + RESULT_LABEL);

    writeCommentLineToOutputFile("ENDWHILE");
}

void AssemblerGeneratorVisitor::visitWriteNode(const WriteNode& writeNode) {
    writeNode.getValueNode()->accept(*this);
    if (currentValue_) {
        writeLineToOutputFile("SET " + std::to_string(*currentValue_));
        writeLineToOutputFile("PUT 0");
    } else {
        if (isCurrentIdentifierAddressPointer_) {
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
    if (!firstValue && isCurrentIdentifierAddressPointer_) {
        isFirstAddressPointer = true;
        writeLineToOutputFile("STORE 3");
        firstAddress = 3;
    }

    valueNode2->accept(*this);
    std::optional<long long> secondValue = currentValue_;
    long long secondAddress = currentIdentifierAddress_;
    if (!secondValue && isCurrentIdentifierAddressPointer_) {
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

void AssemblerGeneratorVisitor::writeCommentLineToOutputFile(const std::string& text) {
    if (shouldWriteComments_) {
        outputAssemblerCode_ += "# " + text + '\n';
    }
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
