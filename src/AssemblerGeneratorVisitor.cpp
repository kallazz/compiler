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

const std::string MULTIPLICATION_PROCEDURE_NAME = "!MUL";
const std::string DIVISION_PROCEDURE_NAME = "!DIV";
const std::string MODULO_PROCEDURE_NAME = "!MOD";

constexpr long long MULTIPLICATION_PROCEDURE_RETURN_ADDRESS = 10;
constexpr long long DIVISION_PROCEDURE_RETURN_ADDRESS = 11;
constexpr long long MODULO_PROCEDURE_RETURN_ADDRESS = 12;

/*
Registers' purpose:
    p0 - accumulator
    p1, p2, p3, p4, p5, p6, p7, p8, p9 - help during calculations
    p10 - multiplication procedure's return address
    p11 - divison procedure's return address
    p12 - modulo procedure's return address
    p13 - storing 0 as it's faster to load it from here than set the accumulator to 0
    p14 - storing 1 as it's faster to load it from here than set the accumulator to 1
    (0 and 1 will be there only if any of the mathematical procedures is used)
    the rest - storing variables, numbers used by the program and procedures' return addresses
*/

AssemblerGeneratorVisitor::AssemblerGeneratorVisitor(const SymbolTable& symbolTable,
                                                     const bool isMultiplicationProcedureNeeded,
                                                     const bool isDivisionProcedureNeeded,
                                                     const bool isModuloProcedureNeeded)
    : symbolTable_(symbolTable),
      isMultiplicationProcedureNeeded_(isMultiplicationProcedureNeeded),
      isDivisionProcedureNeeded_(isDivisionProcedureNeeded),
      isModuloProcedureNeeded_(isModuloProcedureNeeded),
      outputAssemblerCode_(""),
      forLoopsCounter_(0),
      currentIdentifierAddress_(-1),
      isCurrentIdentifierAddressPointer_(false),
      isCurrentIdentifierAddressCalculatedPointerForArray_(false),
      currentJumpType_(""),
      isCurrentJumpForTrueCondition_(false),
      unresolvedJumpsCounter_(0),
      currentLineNumber_(0),
      currentProcedureName_(std::nullopt) {}

void AssemblerGeneratorVisitor::visitConditionNode(const ConditionNode& conditionNode) {
    addOrSubtract(conditionNode.getValueNode1(), conditionNode.getValueNode2(), MathematicalOperator::SUBTRACT);
    switch (conditionNode.getComparisonOperator()) {
        case ComparisonOperator::EQUAL:
            currentJumpType_ = "JZERO";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparisonOperator::NOT_EQUAL:
            currentJumpType_ = "JZERO";
            isCurrentJumpForTrueCondition_ = false;
            break;
        case ComparisonOperator::GREATER_THAN:
            currentJumpType_ = "JPOS";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparisonOperator::LESS_THAN:
            currentJumpType_ = "JNEG";
            isCurrentJumpForTrueCondition_ = true;
            break;
        case ComparisonOperator::GREATER_THAN_OR_EQUAL:
            currentJumpType_ = "JNEG";
            isCurrentJumpForTrueCondition_ = false;
            break;
        case ComparisonOperator::LESS_THAN_OR_EQUAL:
            currentJumpType_ = "JPOS";
            isCurrentJumpForTrueCondition_ = false;
            break;
    }
}

void AssemblerGeneratorVisitor::visitExpressionNode(const ExpressionNode& expressionNode) {
    if (!expressionNode.getValueNode2()) {
        expressionNode.getValueNode1()->accept(*this);

        if (isCurrentIdentifierAddressPointer_) {
            appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
        } else {
            appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
        }

        return;
    }

    if (expressionNode.getMathematicalOperator() == MathematicalOperator::ADD ||
        expressionNode.getMathematicalOperator() == MathematicalOperator::SUBTRACT) {
        addOrSubtract(expressionNode.getValueNode1(), expressionNode.getValueNode2(),
                      *expressionNode.getMathematicalOperator());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::MULTIPLY) {
        multiply(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::DIVIDE) {
        divide(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::MODULO) {
        modulo(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    }
}

void AssemblerGeneratorVisitor::visitIdentifierNode(const IdentifierNode& identifierNode) {
    const std::string variableName = getVariableNameOrIteratorName(identifierNode.getName());

    isCurrentIdentifierAddressPointer_ = false;
    isCurrentIdentifierAddressCalculatedPointerForArray_ = false;
    if (!identifierNode.getIndexName() && !identifierNode.getIndexValue()) {
        if (currentProcedureName_) {
            const auto [address, isPointer] =
                symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            currentIdentifierAddress_ = address;
            isCurrentIdentifierAddressPointer_ = isPointer;
        } else {
            currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(variableName);
        }
    } else if (identifierNode.getIndexValue()) {
        if (currentProcedureName_) {
            const auto [arrayAddress, isPointer] =
                symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            isCurrentIdentifierAddressPointer_ = isPointer;

            if (isCurrentIdentifierAddressPointer_) {
                appendLineToOutputCode("SET " + std::to_string(*identifierNode.getIndexValue()));
                appendLineToOutputCode("ADD " + std::to_string(arrayAddress));

                currentIdentifierAddress_ = 0;
                isCurrentIdentifierAddressPointer_ = true;
                isCurrentIdentifierAddressCalculatedPointerForArray_ = true;
            } else {
                currentIdentifierAddress_ = arrayAddress + *identifierNode.getIndexValue();
            }
        } else {
            currentIdentifierAddress_ =
                symbolTable_.getVariableAddressInMain(variableName) + *identifierNode.getIndexValue();
        }
    } else if (identifierNode.getIndexName()) {
        const std::string indexName = getVariableNameOrIteratorName(*identifierNode.getIndexName());

        if (currentProcedureName_) {
            const auto [arrayAddress, isArrayPointer] =
                symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            const auto [arrayIndexNameAddress, isIndexPointer] =
                symbolTable_.getVariableAddressInProcedure(indexName, *currentProcedureName_);

            const std::string addCommand = isIndexPointer ? "ADDI" : "ADD";

            if (isArrayPointer) {
                appendLineToOutputCode("LOAD " + std::to_string(arrayAddress));
                appendLineToOutputCode(addCommand + " " + std::to_string(arrayIndexNameAddress));
            } else {
                appendLineToOutputCode("SET " + std::to_string(arrayAddress));
                appendLineToOutputCode(addCommand + " " + std::to_string(arrayIndexNameAddress));
            }
        } else {
            const long long arrayAddress = symbolTable_.getVariableAddressInMain(variableName);
            const long long arrayIndexNameAddress = symbolTable_.getVariableAddressInMain(indexName);
            appendLineToOutputCode("SET " + std::to_string(arrayAddress));
            appendLineToOutputCode("ADD " + std::to_string(arrayIndexNameAddress));
        }

        currentIdentifierAddress_ = 0;
        isCurrentIdentifierAddressPointer_ = true;
        isCurrentIdentifierAddressCalculatedPointerForArray_ = true;
    }
}

void AssemblerGeneratorVisitor::visitValueNode(const ValueNode& valueNode) {
    if (valueNode.getIdentifierNode()) {
        valueNode.getIdentifierNode()->accept(*this);
    } else {
        currentIdentifierAddress_ = *symbolTable_.getGlobalConstantAddress(*valueNode.getNumber());
        isCurrentIdentifierAddressPointer_ = false;
        isCurrentIdentifierAddressCalculatedPointerForArray_ = false;
    }
}

void AssemblerGeneratorVisitor::visitArgumentsDeclarationNode(const ArgumentsDeclarationNode&) {
    return;
}

void AssemblerGeneratorVisitor::visitArgumentsNode(const ArgumentsNode&) {
    return;
}

void AssemblerGeneratorVisitor::visitAssignmentNode(const AssignmentNode& assignmentNode) {
    std::string storeCommand = "STORE";

    assignmentNode.getIdentifierNode()->accept(*this);
    long long identifierAddress = currentIdentifierAddress_;
    if (isCurrentIdentifierAddressPointer_) {
        storeCommand += "I";

        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            appendLineToOutputCode("STORE 9");
            identifierAddress = 9;
        }
    }

    assignmentNode.getExpressionNode()->accept(*this);

    appendLineToOutputCode(storeCommand + " " + std::to_string(identifierAddress));
}

void AssemblerGeneratorVisitor::visitCommandsNode(const CommandsNode& commandsNode) {
    for (const auto& command : commandsNode.getCommandNodes()) {
        command->accept(*this);
    }
}

void AssemblerGeneratorVisitor::visitDeclarationsNode(const DeclarationsNode&) {
    return;
}

ValueNode* copyValueNode(const std::unique_ptr<ValueNode>& valueNode) {
    const int lineNumber = valueNode->getLineNumber();
    if (valueNode->getIdentifierNode()) {
        const int identifierNodeLineNumber = valueNode->getIdentifierNode()->getLineNumber();
        const std::string identifierNodeName = valueNode->getIdentifierNode()->getName();

        if (valueNode->getIdentifierNode()->getIndexName()) {
            const std::string identifierNodeIndexName = *valueNode->getIdentifierNode()->getIndexName();
            return new ValueNode(
                lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName, identifierNodeIndexName));
        }
        if (valueNode->getIdentifierNode()->getIndexValue()) {
            const long long identifierNodeIndexValue = *valueNode->getIdentifierNode()->getIndexValue();
            return new ValueNode(
                lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName, identifierNodeIndexValue));
        }
        return new ValueNode(lineNumber, new IdentifierNode(identifierNodeLineNumber, identifierNodeName));
    }

    const long long number = *valueNode->getNumber();

    return new ValueNode(lineNumber, number);
}

void AssemblerGeneratorVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    const int currentForLoopIndex = forLoopsCounter_;
    forLoopsCounter_++;

    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    const int afterLoopJumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    const std::string iteratorName = "!IT" + std::to_string(currentForLoopIndex) + forLoopNode.getIteratorName();
    const std::string iteratorBoundName =
        "!ITBOUND" + std::to_string(currentForLoopIndex) + forLoopNode.getIteratorName();

    iteratorProgramNameToIteratorInternalName_.insert({forLoopNode.getIteratorName(), iteratorName});

    const AssignmentNode iteratorAssignmentNode(0, new IdentifierNode(0, iteratorName),
                                                new ExpressionNode(0, copyValueNode(forLoopNode.getStartValueNode())));
    const AssignmentNode iteratorBoundAssignmentNode(
        0, new IdentifierNode(0, iteratorBoundName),
        new ExpressionNode(0, copyValueNode(forLoopNode.getEndValueNode())));
    iteratorAssignmentNode.accept(*this);
    iteratorBoundAssignmentNode.accept(*this);

    appendToOutputCode(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL);

    const ComparisonOperator comparisonOperator = forLoopNode.isIteratorIncremented()
                                                      ? ComparisonOperator::LESS_THAN_OR_EQUAL
                                                      : ComparisonOperator::GREATER_THAN_OR_EQUAL;
    const ConditionNode conditionNode(0, new ValueNode(0, new IdentifierNode(0, iteratorName)),
                                      new ValueNode(0, new IdentifierNode(0, iteratorBoundName)), comparisonOperator);
    conditionNode.accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        appendLineToOutputCode(currentJumpType_ + " 2");
        appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    } else {
        appendLineToOutputCode(currentJumpType_ + " " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    }

    forLoopNode.getCommandsNode()->accept(*this);

    const MathematicalOperator mathematicalOperator =
        forLoopNode.isIteratorIncremented() ? MathematicalOperator::ADD : MathematicalOperator::SUBTRACT;
    const AssignmentNode iteratorIncrementAssignmentNode(
        0, new IdentifierNode(0, iteratorName),
        new ExpressionNode(0, new ValueNode(0, new IdentifierNode(0, iteratorName)), new ValueNode(0, 1),
                           mathematicalOperator));
    iteratorIncrementAssignmentNode.accept(*this);

    appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    appendToOutputCode(RESULT_LABEL + std::to_string(afterLoopJumpIndex) + RESULT_LABEL);

    iteratorProgramNameToIteratorInternalName_.erase(forLoopNode.getIteratorName());
}

void AssemblerGeneratorVisitor::visitIfNode(const IfNode& ifNode) {
    ifNode.getConditionNode()->accept(*this);

    const int jumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    const bool isCurrentJumpForTrueCondition = isCurrentJumpForTrueCondition_;

    if (!ifNode.getElseCommandsNode()) {
        if (isCurrentJumpForTrueCondition) {
            appendLineToOutputCode(currentJumpType_ + " 2");
            appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        } else {
            appendLineToOutputCode(currentJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        }

        ifNode.getThenCommandsNode()->accept(*this);
        appendToOutputCode(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);
    } else {
        appendLineToOutputCode(currentJumpType_ + " " + FILL_LABEL + std::to_string(jumpIndex) + FILL_LABEL);
        if (isCurrentJumpForTrueCondition) {
            ifNode.getElseCommandsNode()->accept(*this);
        } else {
            ifNode.getThenCommandsNode()->accept(*this);
        }

        const int jumpOverSecondPartIndex = unresolvedJumpsCounter_;
        unresolvedJumpsCounter_++;

        appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(jumpOverSecondPartIndex) + FILL_LABEL);
        appendToOutputCode(RESULT_LABEL + std::to_string(jumpIndex) + RESULT_LABEL);

        if (isCurrentJumpForTrueCondition) {
            ifNode.getThenCommandsNode()->accept(*this);
        } else {
            ifNode.getElseCommandsNode()->accept(*this);
        }

        appendToOutputCode(RESULT_LABEL + std::to_string(jumpOverSecondPartIndex) + RESULT_LABEL);
    }
}

void AssemblerGeneratorVisitor::visitMainNode(const MainNode& mainNode) {
    setGlobalConstantValues();

    mainNode.getCommandsNode()->accept(*this);
    resolveLabels();
    appendLineToOutputCode("HALT");
}

void AssemblerGeneratorVisitor::visitProcedureCallNode(const ProcedureCallNode& procedureCallNode) {
    const std::string& procedureName = procedureCallNode.getName();
    const auto& callArguments = procedureCallNode.getArgumentsNode()->getArguments();
    const auto calledProcedureArgumentsAddresses = symbolTable_.getProcedureArgumentsAddresses(procedureName);

    for (size_t i = 0; i < callArguments.size(); i++) {
        const std::string callArgumentName = getVariableNameOrIteratorName(callArguments[i].name);
        long long callArgumentAddress;
        bool isCallArgumentPointer;
        if (currentProcedureName_) {
            const auto [address, isPointer] =
                symbolTable_.getVariableAddressInProcedure(callArgumentName, *currentProcedureName_);
            callArgumentAddress = address;
            isCallArgumentPointer = isPointer;
        } else {
            callArgumentAddress = symbolTable_.getVariableAddressInMain(callArgumentName);
            isCallArgumentPointer = false;
        }
        const long long procedureArgumentAddress = calledProcedureArgumentsAddresses[i];

        const std::string command = isCallArgumentPointer ? "LOAD" : "SET";
        appendLineToOutputCode(command + " " + std::to_string(callArgumentAddress));
        appendLineToOutputCode("STORE " + std::to_string(procedureArgumentAddress));
    }

    appendLineToOutputCode("SET " + std::to_string(currentLineNumber_ + 3));
    appendLineToOutputCode("STORE " + std::to_string(symbolTable_.getProcedureReturnAddress(procedureName)));
    appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(procedureName)) +
                           FILL_LABEL);
}

void AssemblerGeneratorVisitor::visitProcedureHeadNode(const ProcedureHeadNode&) {
    return;
}

void AssemblerGeneratorVisitor::visitProceduresNode(const ProceduresNode& proceduresNode) {
    if (!proceduresNode.getProcedures().empty() || isMultiplicationProcedureNeeded_ || isDivisionProcedureNeeded_ ||
        isModuloProcedureNeeded_) {
        const int jumpOverAllProceduresIndex = unresolvedJumpsCounter_;
        unresolvedJumpsCounter_++;

        appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(jumpOverAllProceduresIndex) + FILL_LABEL);

        if (isMultiplicationProcedureNeeded_) {
            generateMultiplicationProcedure();
        }
        if (isDivisionProcedureNeeded_) {
            generateDivisionProcedure();
        }
        if (isModuloProcedureNeeded_) {
            generateModuloProcedure();
        }

        for (const auto& procedure : proceduresNode.getProcedures()) {
            currentProcedureName_ = procedure->procedureHeadNode->getName();

            const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
            unresolvedJumpsCounter_++;
            procedureNameToJumpIndex.insert({*currentProcedureName_, jumpToCurrentProcedureIndex});

            appendToOutputCode(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);
            procedure->commandsNode->accept(*this);
            appendLineToOutputCode("RTRN " +
                                   std::to_string(symbolTable_.getProcedureReturnAddress(*currentProcedureName_)));
        }

        currentProcedureName_ = std::nullopt;

        appendToOutputCode(RESULT_LABEL + std::to_string(jumpOverAllProceduresIndex) + RESULT_LABEL);
    }
}

void AssemblerGeneratorVisitor::visitReadNode(const ReadNode& readNode) {
    readNode.getIdentifierNode()->accept(*this);
    if (isCurrentIdentifierAddressPointer_) {
        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            appendLineToOutputCode("STORE 3");
            appendLineToOutputCode("GET 0");
            appendLineToOutputCode("STOREI 3");
        } else {
            appendLineToOutputCode("GET 0");
            appendLineToOutputCode("STOREI " + std::to_string(currentIdentifierAddress_));
        }
    } else {
        appendLineToOutputCode("GET " + std::to_string(currentIdentifierAddress_));
    }
}

void AssemblerGeneratorVisitor::visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode) {
    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    appendToOutputCode(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL);

    repeatLoopNode.getCommandsNode()->accept(*this);
    repeatLoopNode.getConditionNode()->accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        appendLineToOutputCode(currentJumpType_ + " 2");
        appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    } else {
        appendLineToOutputCode(currentJumpType_ + " " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    }
}

void AssemblerGeneratorVisitor::visitWhileLoopNode(const WhileLoopNode& whileLoopNode) {
    const int backToStartjumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    const int afterLoopJumpIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;

    appendToOutputCode(RESULT_LABEL + std::to_string(backToStartjumpIndex) + RESULT_LABEL);

    whileLoopNode.getConditionNode()->accept(*this);

    if (isCurrentJumpForTrueCondition_) {
        appendLineToOutputCode(currentJumpType_ + " 2");
        appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    } else {
        appendLineToOutputCode(currentJumpType_ + " " + FILL_LABEL + std::to_string(afterLoopJumpIndex) + FILL_LABEL);
    }

    whileLoopNode.getCommandsNode()->accept(*this);

    appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(backToStartjumpIndex) + FILL_LABEL);
    appendToOutputCode(RESULT_LABEL + std::to_string(afterLoopJumpIndex) + RESULT_LABEL);
}

void AssemblerGeneratorVisitor::visitWriteNode(const WriteNode& writeNode) {
    writeNode.getValueNode()->accept(*this);

    if (isCurrentIdentifierAddressPointer_) {
        appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
        appendLineToOutputCode("PUT 0");
    } else {
        appendLineToOutputCode("PUT " + std::to_string(currentIdentifierAddress_));
    }
}

void AssemblerGeneratorVisitor::setGlobalConstantValues() {
    const auto constantInfos = symbolTable_.getGlobalConstantInfos();
    for (const auto [address, value] : constantInfos) {
        appendLineToOutputCode("SET " + std::to_string(value));
        appendLineToOutputCode("STORE " + std::to_string(address));
    }
}

void AssemblerGeneratorVisitor::addOrSubtract(const std::unique_ptr<ValueNode>& valueNode1,
                                              const std::unique_ptr<ValueNode>& valueNode2,
                                              const MathematicalOperator mathematicalOperator) {
    const bool isSubtraction = (mathematicalOperator == MathematicalOperator::SUBTRACT);
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode2->accept(*this);
    long long secondAddress = currentIdentifierAddress_;

    if (isCurrentIdentifierAddressPointer_) {
        isSecondAddressPointer = true;

        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            appendLineToOutputCode("STORE 1");
            secondAddress = 1;
        }
    }

    valueNode1->accept(*this);
    long long firstAddress = currentIdentifierAddress_;

    if (isCurrentIdentifierAddressPointer_) {
        isFirstAddressPointer = true;
    }

    const std::string loadCommand = isFirstAddressPointer ? "LOADI" : "LOAD";
    appendLineToOutputCode(loadCommand + " " + std::to_string(firstAddress));

    const std::optional<long long> zeroAddress = symbolTable_.getGlobalConstantAddress(0);
    if (!(zeroAddress && zeroAddress == secondAddress)) {
        std::string mathematicalCommand = isSubtraction ? "SUB" : "ADD";
        if (isSecondAddressPointer) {
            mathematicalCommand += "I";
        }

        appendLineToOutputCode(mathematicalCommand + " " + std::to_string(secondAddress));
    }
}

void AssemblerGeneratorVisitor::multiply(const std::unique_ptr<ValueNode>& valueNode1,
                                         const std::unique_ptr<ValueNode>& valueNode2) {
    valueNode1->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    appendLineToOutputCode("STORE 1");

    valueNode2->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    appendLineToOutputCode("STORE 2");

    appendLineToOutputCode("SET " + std::to_string(currentLineNumber_ + 3));
    appendLineToOutputCode("STORE " + std::to_string(MULTIPLICATION_PROCEDURE_RETURN_ADDRESS));
    appendLineToOutputCode("JUMP " + FILL_LABEL +
                           std::to_string(procedureNameToJumpIndex.at(MULTIPLICATION_PROCEDURE_NAME)) + FILL_LABEL);
}

void AssemblerGeneratorVisitor::divide(const std::unique_ptr<ValueNode>& valueNode1,
                                       const std::unique_ptr<ValueNode>& valueNode2) {
    valueNode2->accept(*this);

    const std::optional<long long> twoAddress = symbolTable_.getGlobalConstantAddress(2);
    if (twoAddress && *twoAddress == currentIdentifierAddress_) {
        valueNode1->accept(*this);
        const std::string loadCommand = isCurrentIdentifierAddressPointer_ ? "LOADI" : "LOAD";
        appendLineToOutputCode(loadCommand + " " + std::to_string(currentIdentifierAddress_));
        appendLineToOutputCode("HALF");
    } else {
        if (!isCurrentIdentifierAddressPointer_) {
            appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
        } else {
            appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
        }
        appendLineToOutputCode("STORE 2");

        valueNode1->accept(*this);

        if (!isCurrentIdentifierAddressPointer_) {
            appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
        } else {
            appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
        }
        appendLineToOutputCode("STORE 1");

        appendLineToOutputCode("SET " + std::to_string(currentLineNumber_ + 3));
        appendLineToOutputCode("STORE " + std::to_string(DIVISION_PROCEDURE_RETURN_ADDRESS));
        appendLineToOutputCode("JUMP " + FILL_LABEL +
                               std::to_string(procedureNameToJumpIndex.at(DIVISION_PROCEDURE_NAME)) + FILL_LABEL);
    }
}

void AssemblerGeneratorVisitor::modulo(const std::unique_ptr<ValueNode>& valueNode1,
                                       const std::unique_ptr<ValueNode>& valueNode2) {
    valueNode1->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    appendLineToOutputCode("STORE 1");

    valueNode2->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        appendLineToOutputCode("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        appendLineToOutputCode("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    appendLineToOutputCode("STORE 2");

    appendLineToOutputCode("SET " + std::to_string(currentLineNumber_ + 3));
    appendLineToOutputCode("STORE " + std::to_string(MODULO_PROCEDURE_RETURN_ADDRESS));
    appendLineToOutputCode("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(MODULO_PROCEDURE_NAME)) +
                           FILL_LABEL);
}

std::string AssemblerGeneratorVisitor::getVariableNameOrIteratorName(const std::string& variableName) {
    const auto it = iteratorProgramNameToIteratorInternalName_.find(variableName);
    if (it != iteratorProgramNameToIteratorInternalName_.end()) {
        return it->second;
    }

    return variableName;
}

void AssemblerGeneratorVisitor::appendToOutputCode(const std::string& text) {
    outputAssemblerCode_ += text;
}

void AssemblerGeneratorVisitor::appendLineToOutputCode(const std::string& text) {
    outputAssemblerCode_ += text + '\n';
    currentLineNumber_++;
}

std::string resolveResultLabels(const std::string& code,
                                std::unordered_map<int, int>& jumpIndexToResultLabelLineNumber) {
    std::string codeWithoutResultLabels = "";
    std::istringstream codeStream(code);
    std::string line;
    int lineNumber = 0;

    while (std::getline(codeStream, line)) {
        size_t resultLabelPosition = 0;
        while ((resultLabelPosition = line.find(RESULT_LABEL)) != std::string::npos) {
            size_t endResultLabelPosition = line.find(RESULT_LABEL, resultLabelPosition + 2);
            if (endResultLabelPosition != std::string::npos) {
                const int jumpIndex =
                    std::stoi(line.substr(resultLabelPosition + 2, endResultLabelPosition - resultLabelPosition - 2));
                jumpIndexToResultLabelLineNumber[jumpIndex] = lineNumber;
                line.erase(resultLabelPosition, endResultLabelPosition + 2 - resultLabelPosition);
            }
        }

        codeWithoutResultLabels += line + '\n';
        lineNumber++;
    }

    return codeWithoutResultLabels;
}

std::string resolveFillLabels(const std::string& code,
                              const std::unordered_map<int, int>& jumpIndexToResultLabelLineNumber) {
    std::string codeWithFilledLabels = "";
    std::istringstream codeStream(code);
    std::string line;
    int lineNumber = 0;

    while (std::getline(codeStream, line)) {
        size_t fillLabelPosition = 0;
        while ((fillLabelPosition = line.find(FILL_LABEL)) != std::string::npos) {
            size_t endFillLabelPosition = line.find(FILL_LABEL, fillLabelPosition + 2);
            if (endFillLabelPosition != std::string::npos) {
                const int jumpIndex =
                    std::stoi(line.substr(fillLabelPosition + 2, endFillLabelPosition - fillLabelPosition - 2));
                const int lineDifference = jumpIndexToResultLabelLineNumber.at(jumpIndex) - lineNumber;
                line.replace(fillLabelPosition, endFillLabelPosition + 2 - fillLabelPosition,
                             std::to_string(lineDifference));
            }
        }

        codeWithFilledLabels += line + '\n';
        lineNumber++;
    }

    return codeWithFilledLabels;
}

void AssemblerGeneratorVisitor::resolveLabels() {
    std::unordered_map<int, int> jumpIndexToResultLabelLineNumber;

    const std::string codeWithoutResultLabels =
        resolveResultLabels(outputAssemblerCode_, jumpIndexToResultLabelLineNumber);
    const std::string codeWithFilledLabels =
        resolveFillLabels(codeWithoutResultLabels, jumpIndexToResultLabelLineNumber);

    outputAssemblerCode_ = codeWithFilledLabels;
}

/*
The below assembler code was based on this procedure:

PROCEDURE multiply(a, b, result) IS
    temp, is_a_negative, is_b_negative
BEGIN
    result := 0;

    IF a < 0 THEN
        is_a_negative := 1;
        a := 0 - a;
    ELSE
        is_a_negative := 0;
    ENDIF

    IF b < 0 THEN
        is_b_negative := 1;
        b := 0 - b;
    ELSE
        is_b_negative := 0;
    ENDIF

    WHILE b > 0 DO
        temp := b / 2;
        temp := temp + temp;

        IF temp != b THEN
            result := result + a;
        ENDIF

        a := a + a;
        b := b / 2;
    ENDWHILE

    temp := is_a_negative + is_b_negative;

    IF temp = 1 THEN
        result := 0 - result;
    ENDIF
END
*/
void AssemblerGeneratorVisitor::generateMultiplicationProcedure() {
    const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    procedureNameToJumpIndex.insert({MULTIPLICATION_PROCEDURE_NAME, jumpToCurrentProcedureIndex});

    appendToOutputCode(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);

    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 2");
    appendLineToOutputCode("STORE 2");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JPOS 2");
    appendLineToOutputCode("JUMP 17");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("ADD 0");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("SUB 2");
    appendLineToOutputCode("JZERO 4");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("ADD 1");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("ADD 1");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("STORE 2");
    appendLineToOutputCode("JUMP -19");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("ADD 6");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JZERO 2");
    appendLineToOutputCode("JUMP 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 3");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("RTRN " + std::to_string(MULTIPLICATION_PROCEDURE_RETURN_ADDRESS));
}

/*
The below assembler code was based on this procedure:

PROCEDURE divide(a, b, result) IS
a, b, result, power, temp_divisor, is_a_negative, is_b_negative
BEGIN
    result := 0;

    IF b != 0 THEN
        IF a < 0 THEN
            is_a_negative := 1;
            a := 0 - a;
        ELSE
            is_a_negative := 0;
        ENDIF

        IF b < 0 THEN
            is_b_negative := 1;
            b := 0 - b;
        ELSE
            is_b_negative := 0;
        ENDIF

        power := 1;
        temp_divisor := b;

        WHILE temp_divisor <= a DO
            temp_divisor := temp_divisor + temp_divisor;
            power := power + power;
        ENDWHILE

        WHILE power > 1 DO
            temp_divisor := temp_divisor / 2;
            power := power / 2;

            IF a >= temp_divisor THEN
                a := a - temp_divisor;
                result := result + power;
            ENDIF
        ENDWHILE

        temp_divisor := is_a_negative + is_b_negative;

        IF temp_divisor = 1 THEN
            result := 0 - result;
            IF a != 0 THEN
                result := result - 1;
            ENDIF
        ENDIF
    ENDIF
END
*/
void AssemblerGeneratorVisitor::generateDivisionProcedure() {
    const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    procedureNameToJumpIndex.insert({DIVISION_PROCEDURE_NAME, jumpToCurrentProcedureIndex});

    appendToOutputCode(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);

    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JZERO 70");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 7");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 7");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 2");
    appendLineToOutputCode("STORE 2");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("JPOS 8");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("ADD 5");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("ADD 4");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("JUMP -9");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JPOS 2");
    appendLineToOutputCode("JUMP 17");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("SUB 5");
    appendLineToOutputCode("JNEG 7");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("SUB 5");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("ADD 4");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("JUMP -19");
    appendLineToOutputCode("LOAD 6");
    appendLineToOutputCode("ADD 7");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JZERO 2");
    appendLineToOutputCode("JUMP 9");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 3");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("JZERO 4");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("RTRN " + std::to_string(DIVISION_PROCEDURE_RETURN_ADDRESS));
}

/*
The below assembler code was based on this procedure:

PROCEDURE modulo(a, b) IS # a is the result
power, temp_divisor, initial_b, is_a_negative, is_b_negative
BEGIN
    IF b != 0 THEN
        IF a < 0 THEN
            is_a_negative := 1;
            a := 0 - a;
        ELSE
            is_a_negative := 0;
        ENDIF

        IF b < 0 THEN
            is_b_negative := 1;
            b := 0 - b;
        ELSE
            is_b_negative := 0;
        ENDIF

        initial_b := b;

        power := 1;
        temp_divisor := b;

        WHILE temp_divisor <= a DO
            temp_divisor := temp_divisor + temp_divisor;
            power := power + power;
        ENDWHILE

        WHILE power > 1 DO
            temp_divisor := temp_divisor / 2;
            power := power / 2;

            IF a >= temp_divisor THEN
                a := a - temp_divisor;
            ENDIF
        ENDWHILE

        IF is_a_negative = 1 THEN
            a := initial_b - a;
        ENDIF

        IF is_b_negative = 1 THEN
            a := a - initial_b;
        ENDIF
    ELSE
        a := 0;
    ENDIF
END
*/
void AssemblerGeneratorVisitor::generateModuloProcedure() {
    const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    procedureNameToJumpIndex.insert({MODULO_PROCEDURE_NAME, jumpToCurrentProcedureIndex});

    appendToOutputCode(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);

    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JZERO 70");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 6");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 7");
    appendLineToOutputCode("JUMP 6");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 7");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("SUB 2");
    appendLineToOutputCode("STORE 2");
    appendLineToOutputCode("LOAD 2");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("STORE 5");
    appendLineToOutputCode("LOAD 14");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("JPOS 8");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("ADD 4");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("ADD 3");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("JUMP -9");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JPOS 2");
    appendLineToOutputCode("JUMP 14");
    appendLineToOutputCode("LOAD 4");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("STORE 4");
    appendLineToOutputCode("LOAD 3");
    appendLineToOutputCode("HALF");
    appendLineToOutputCode("STORE 3");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("SUB 4");
    appendLineToOutputCode("JNEG 4");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("SUB 4");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("JUMP -16");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("JZERO 15");
    appendLineToOutputCode("LOAD 6");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JZERO 2");
    appendLineToOutputCode("JUMP 4");
    appendLineToOutputCode("LOAD 5");
    appendLineToOutputCode("SUB 1");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 7");
    appendLineToOutputCode("SUB 14");
    appendLineToOutputCode("JZERO 2");
    appendLineToOutputCode("JUMP 4");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("SUB 5");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("JUMP 3");
    appendLineToOutputCode("LOAD 13");
    appendLineToOutputCode("STORE 1");
    appendLineToOutputCode("LOAD 1");
    appendLineToOutputCode("RTRN " + std::to_string(MODULO_PROCEDURE_RETURN_ADDRESS));
}

std::string AssemblerGeneratorVisitor::getGeneratedAssemblerCode() const {
    return outputAssemblerCode_;
}
