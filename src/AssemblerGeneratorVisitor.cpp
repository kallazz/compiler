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
    p10 - multiplication procedure's return
    p11 - divison procedure's return
    p12 - modulo procedure's return
    p13 - storing 0 as it's faster to load it from here than set accumulator to 0 (if it's needed by the program)
    p14 - storing 1 as it's faster to load it from here than set accumulator to 1 (if it's needed by the program)
    the rest - storing variables
*/


AssemblerGeneratorVisitor::AssemblerGeneratorVisitor(const SymbolTable& symbolTable)
    : symbolTable_(symbolTable), shouldWriteComments_(false), outputAssemblerCode_(""), forLoopsCounter_(0), currentIdentifierAddress_(-1), isCurrentIdentifierAddressPointer_(false), isCurrentIdentifierAddressCalculatedPointerForArray_(false), currentJumpType_(""), isCurrentJumpForTrueCondition_(false), unresolvedJumpsCounter_(0), currentProcedureName_(std::nullopt), currentLineNumber_(0) {}

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

        if (isCurrentIdentifierAddressPointer_ && !isCurrentIdentifierAddressCalculatedPointerForArray_) {
            writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
        } else {
            writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
        }

        return;
    }

    if (expressionNode.getMathematicalOperator() == MathematicalOperator::ADD || expressionNode.getMathematicalOperator() == MathematicalOperator::SUBTRACT) {
        addOrSubtract(expressionNode.getValueNode1(), expressionNode.getValueNode2(), *expressionNode.getMathematicalOperator());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::MULTIPLY) {
        multiply(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::DIVIDE) {
        divide(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    } else if (expressionNode.getMathematicalOperator() == MathematicalOperator::MODULO) {
        modulo(expressionNode.getValueNode1(), expressionNode.getValueNode2());
    }
}

void AssemblerGeneratorVisitor::visitIdentifierNode(const IdentifierNode& identifierNode) {
    // TODO: Add procedure logic
    const std::string variableName = getVariableNameOrIteratorName(identifierNode.getName());

    isCurrentIdentifierAddressPointer_  = false;
    isCurrentIdentifierAddressCalculatedPointerForArray_ = false;
    if (!identifierNode.getIndexName() && !identifierNode.getIndexValue()) {
        if (currentProcedureName_) {
            const auto [address, isPointer] = symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            currentIdentifierAddress_ = address;
            isCurrentIdentifierAddressPointer_ = isPointer;
        } else {
            currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(variableName);
        }
    } else if (identifierNode.getIndexValue()) {
        if (currentProcedureName_) {
            const auto [arrayAddress, isPointer] = symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            isCurrentIdentifierAddressPointer_ = isPointer;

            if (isCurrentIdentifierAddressPointer_) {
                writeLineToOutputFile("SET " + std::to_string(*identifierNode.getIndexValue()));
                writeLineToOutputFile("ADD " + std::to_string(arrayAddress));

                currentIdentifierAddress_ = 0;
                isCurrentIdentifierAddressPointer_ = true;
                isCurrentIdentifierAddressCalculatedPointerForArray_ = true;
            } else {
                currentIdentifierAddress_ = arrayAddress + *identifierNode.getIndexValue();
            }
        } else {
            currentIdentifierAddress_ = symbolTable_.getVariableAddressInMain(variableName) + *identifierNode.getIndexValue();
        }
    } else if (identifierNode.getIndexName()) {
        const std::string indexName = getVariableNameOrIteratorName(*identifierNode.getIndexName());

        if (currentProcedureName_) {
            const auto [arrayAddress, isArrayPointer] = symbolTable_.getVariableAddressInProcedure(variableName, *currentProcedureName_);
            const auto [arrayIndexNameAddress , isIndexPointer] = symbolTable_.getVariableAddressInProcedure(indexName, *currentProcedureName_);

            const std::string addCommand = isIndexPointer ? "ADDI" : "ADD";

            if (isArrayPointer) {
                writeLineToOutputFile("LOAD " + std::to_string(arrayAddress));
                writeLineToOutputFile(addCommand + " " + std::to_string(arrayIndexNameAddress));
            } else {
                writeLineToOutputFile("SET " + std::to_string(arrayAddress));
                writeLineToOutputFile(addCommand + " " + std::to_string(arrayIndexNameAddress));
            }
        } else {
            const long long arrayAddress = symbolTable_.getVariableAddressInMain(variableName);
            const long long arrayIndexNameAddress = symbolTable_.getVariableAddressInMain(indexName);
            writeLineToOutputFile("SET " + std::to_string(arrayAddress));
            writeLineToOutputFile("ADD " + std::to_string(arrayIndexNameAddress));
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
        currentIdentifierAddress_ = symbolTable_.getGlobalConstantAddress(*valueNode.getNumber());
        isCurrentIdentifierAddressPointer_ = false;
        isCurrentIdentifierAddressCalculatedPointerForArray_ = false;
    }
}

void AssemblerGeneratorVisitor::visitArgumentsDeclarationNode(const ArgumentsDeclarationNode& argumentsDeclarationNode) {
    return;
}

void AssemblerGeneratorVisitor::visitArgumentsNode(const ArgumentsNode& argumentsNode) {
    return;
}

void AssemblerGeneratorVisitor::visitAssignmentNode(const AssignmentNode& assignmentNode) {
    std::string storeCommand = "STORE";

    assignmentNode.getIdentifierNode()->accept(*this);
    long long identifierAddress = currentIdentifierAddress_;
    if (isCurrentIdentifierAddressPointer_) {
        storeCommand += "I";

        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            writeLineToOutputFile("STORE 9");
            identifierAddress = 9;
        }
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
    setGlobalConstantValues();

    mainNode.getCommandsNode()->accept(*this);
    resolveLabels();
    writeLineToOutputFile("HALT");
}

void AssemblerGeneratorVisitor::visitProcedureCallNode(const ProcedureCallNode& procedureCallNode) {
    writeCommentLineToOutputFile("CALL " + procedureCallNode.getName());

    const std::string& procedureName = procedureCallNode.getName();
    const auto& callArguments = procedureCallNode.getArgumentsNode()->getArguments();
    const auto calledProcedureArgumentsAddresses = symbolTable_.getProcedureArgumentsAddresses(procedureName);

    for (int i = 0; i < callArguments.size(); i++) {

        const std::string callArgumentName = getVariableNameOrIteratorName(callArguments[i].name);
        long long callArgumentAddress;
        bool isCallArgumentPointer;
        if (currentProcedureName_) {
            const auto [address, isPointer] = symbolTable_.getVariableAddressInProcedure(callArgumentName, *currentProcedureName_);
            callArgumentAddress = address;
            isCallArgumentPointer = isPointer;
        } else {
            callArgumentAddress = symbolTable_.getVariableAddressInMain(callArgumentName);
            isCallArgumentPointer = false;
        }
        const long long procedureArgumentAddress = calledProcedureArgumentsAddresses[i];

        const std::string command = isCallArgumentPointer ? "LOAD" : "SET";
        writeLineToOutputFile(command + " " + std::to_string(callArgumentAddress));
        writeLineToOutputFile("STORE " + std::to_string(procedureArgumentAddress));
    }

    writeLineToOutputFile("SET " + std::to_string(currentLineNumber_ + 3));
    writeLineToOutputFile("STORE " + std::to_string(symbolTable_.getProcedureReturnAddress(procedureName)));
    writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(procedureName)) + FILL_LABEL);
    
    writeCommentLineToOutputFile("ENDCALL " + procedureCallNode.getName());
}

void AssemblerGeneratorVisitor::visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode) {
    return;
}

void AssemblerGeneratorVisitor::visitProceduresNode(const ProceduresNode& proceduresNode) {
    // if (!proceduresNode.getProcedures().empty()) {
        const int jumpOverAllProceduresIndex = unresolvedJumpsCounter_;
        unresolvedJumpsCounter_++;

        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(jumpOverAllProceduresIndex) + FILL_LABEL);

        generateMultiplicationProcedure();
        generateDivisionProcedure();

        for (const auto& procedure : proceduresNode.getProcedures()) {
            currentProcedureName_ = procedure->procedureHeadNode->getName();

            const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
            unresolvedJumpsCounter_++;
            procedureNameToJumpIndex.insert({*currentProcedureName_, jumpToCurrentProcedureIndex});

            writeCommentLineToOutputFile("PROCEDURE " + *currentProcedureName_);

            writeToOutputFile(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);
            procedure->commandsNode->accept(*this);
            writeLineToOutputFile("RTRN " + std::to_string(symbolTable_.getProcedureReturnAddress(*currentProcedureName_)));

            writeCommentLineToOutputFile("ENDPROCEDURE " + *currentProcedureName_);
        }

        currentProcedureName_ = std::nullopt;

        writeToOutputFile(RESULT_LABEL + std::to_string(jumpOverAllProceduresIndex) + RESULT_LABEL);
    // }
}

void AssemblerGeneratorVisitor::visitReadNode(const ReadNode& readNode) {
    readNode.getIdentifierNode()->accept(*this);
    if (isCurrentIdentifierAddressPointer_) {
        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            writeLineToOutputFile("STORE 3");
            writeLineToOutputFile("GET 0");
            writeLineToOutputFile("STOREI 3");
        } else {
            writeLineToOutputFile("GET 0");
            writeLineToOutputFile("STOREI " + std::to_string(currentIdentifierAddress_));
        }
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

    if (isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
        writeLineToOutputFile("PUT 0");
    } else {
        writeLineToOutputFile("PUT " + std::to_string(currentIdentifierAddress_));
    }
}

void AssemblerGeneratorVisitor::setGlobalConstantValues() {
    const auto constantInfos =  symbolTable_.getGlobalConstantInfos();
    for (const auto [address, value] : constantInfos) {
        writeLineToOutputFile("SET " + std::to_string(value));
        writeLineToOutputFile("STORE " + std::to_string(address));
    }
}

void AssemblerGeneratorVisitor::addOrSubtract(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2, const MathematicalOperator mathematicalOperator) {
    const bool isSubtraction = (mathematicalOperator == MathematicalOperator::SUBTRACT);
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode1->accept(*this);
    long long firstAddress = currentIdentifierAddress_;

    if (isCurrentIdentifierAddressPointer_) {
        isFirstAddressPointer = true;

        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            writeLineToOutputFile("STORE 2");
            firstAddress = 2;
        }
    }

    valueNode2->accept(*this);
    long long secondAddress = currentIdentifierAddress_;

    if (isCurrentIdentifierAddressPointer_) {
        isSecondAddressPointer = true;

        if (isCurrentIdentifierAddressCalculatedPointerForArray_) {
            writeLineToOutputFile("STORE 1");
            secondAddress = 1;
        }
    }

    const std::string loadCommand = isFirstAddressPointer ? "LOADI" : "LOAD";
    writeLineToOutputFile(loadCommand + " " + std::to_string(firstAddress));

    std::string mathematicalCommand = isSubtraction ? "SUB" : "ADD";
    if (isSecondAddressPointer) {
        mathematicalCommand += "I";
    }

    writeLineToOutputFile(mathematicalCommand + " " + std::to_string(secondAddress));
}


void AssemblerGeneratorVisitor::multiply(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2) {
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode1->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 1");

    valueNode2->accept(*this);

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 2");

    writeLineToOutputFile("SET " + std::to_string(currentLineNumber_ + 3));
    writeLineToOutputFile("STORE " + std::to_string(MULTIPLICATION_PROCEDURE_RETURN_ADDRESS));
    writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(MULTIPLICATION_PROCEDURE_NAME)) + FILL_LABEL);    
}

void AssemblerGeneratorVisitor::divide(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2) {
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode1->accept(*this);
    const long long firstAddress = currentIdentifierAddress_;

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 1");

    valueNode2->accept(*this);
    const long long secondAddress = currentIdentifierAddress_;

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 2");

    if (symbolTable_.checkIfGlobalConstantExists(2) && symbolTable_.getGlobalConstantAddress(2) == secondAddress) {
        const std::string loadCommand = isFirstAddressPointer ? "LOADI" : "LOAD";
        writeLineToOutputFile(loadCommand + " " + std::to_string(firstAddress));
        writeLineToOutputFile("HALF");
    } else {
        writeLineToOutputFile("SET " + std::to_string(currentLineNumber_ + 3));
        writeLineToOutputFile("STORE " + std::to_string(DIVISION_PROCEDURE_RETURN_ADDRESS));
        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(DIVISION_PROCEDURE_NAME)) + FILL_LABEL);    
        writeLineToOutputFile("LOAD 3");
    }
}

void AssemblerGeneratorVisitor::modulo(const std::unique_ptr<ValueNode>& valueNode1, const std::unique_ptr<ValueNode>& valueNode2) {
    bool isFirstAddressPointer = false;
    bool isSecondAddressPointer = false;

    valueNode1->accept(*this);
    const long long firstAddress = currentIdentifierAddress_;

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 1");

    valueNode2->accept(*this);
    const long long secondAddress = currentIdentifierAddress_;

    if (!isCurrentIdentifierAddressPointer_) {
        writeLineToOutputFile("LOAD " + std::to_string(currentIdentifierAddress_));
    } else {
        writeLineToOutputFile("LOADI " + std::to_string(currentIdentifierAddress_));
    }
    writeLineToOutputFile("STORE 2");

    // if (symbolTable_.checkIfGlobalConstantExists(2) && symbolTable_.getGlobalConstantAddress(2) == secondAddress) {
    //     const std::string loadCommand = isFirstAddressPointer ? "LOADI" : "LOAD";
    //     writeLineToOutputFile(loadCommand + " " + std::to_string(firstAddress));
    //     writeLineToOutputFile("HALF");
    // } else {
        writeLineToOutputFile("SET " + std::to_string(currentLineNumber_ + 3));
        writeLineToOutputFile("STORE " + std::to_string(DIVISION_PROCEDURE_RETURN_ADDRESS));
        writeLineToOutputFile("JUMP " + FILL_LABEL + std::to_string(procedureNameToJumpIndex.at(DIVISION_PROCEDURE_NAME)) + FILL_LABEL);    
        writeLineToOutputFile("LOAD 1");
    // }
}

std::string AssemblerGeneratorVisitor::getVariableNameOrIteratorName(const std::string& variableName) {
    const auto it = iteratorProgramNameToIteratorInternalName_.find(variableName);
    if (it != iteratorProgramNameToIteratorInternalName_.end()) {
        return it->second;
    }

    return variableName;
}

void AssemblerGeneratorVisitor::writeToOutputFile(const std::string& text) {
    outputAssemblerCode_ += text;
}

void AssemblerGeneratorVisitor::writeLineToOutputFile(const std::string& text) {
    outputAssemblerCode_ += text + '\n';
    currentLineNumber_++;
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
        if (!line.empty() && line[0] == '#') {
            codeWithoutResultLabels += line + '\n';
            continue;
        }

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
        if (!line.empty() && line[0] == '#') {
            codeWithFilledLabels += line + '\n';
            continue;
        }

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

void AssemblerGeneratorVisitor::generateMultiplicationProcedure() {
    const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    procedureNameToJumpIndex.insert({MULTIPLICATION_PROCEDURE_NAME, jumpToCurrentProcedureIndex});

    writeToOutputFile(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);

    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("ADD 1");
    writeLineToOutputFile("JNEG 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 5");
    writeLineToOutputFile("JUMP 6");
    writeLineToOutputFile("LOAD 14");
    writeLineToOutputFile("STORE 5");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 1");
    writeLineToOutputFile("STORE 1");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("ADD 2");
    writeLineToOutputFile("JNEG 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 6");
    writeLineToOutputFile("JUMP 6");
    writeLineToOutputFile("LOAD 14");
    writeLineToOutputFile("STORE 6");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 2");
    writeLineToOutputFile("STORE 2");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("ADD 2");
    writeLineToOutputFile("JPOS 2");
    writeLineToOutputFile("JUMP 17");
    writeLineToOutputFile("LOAD 2");
    writeLineToOutputFile("HALF");
    writeLineToOutputFile("ADD 0");
    writeLineToOutputFile("STORE 4");
    writeLineToOutputFile("SUB 2");
    writeLineToOutputFile("JZERO 4");
    writeLineToOutputFile("LOAD 3");
    writeLineToOutputFile("ADD 1");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("LOAD 1");
    writeLineToOutputFile("ADD 1");
    writeLineToOutputFile("STORE 1");
    writeLineToOutputFile("LOAD 2");
    writeLineToOutputFile("HALF");
    writeLineToOutputFile("STORE 2");
    writeLineToOutputFile("JUMP -19");
    writeLineToOutputFile("LOAD 5");
    writeLineToOutputFile("ADD 6");
    writeLineToOutputFile("STORE 4");
    writeLineToOutputFile("SET -1");
    writeLineToOutputFile("ADD 4");
    writeLineToOutputFile("JZERO 2");
    writeLineToOutputFile("JUMP 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 3");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("LOAD 3");
    writeLineToOutputFile("RTRN " + std::to_string(MULTIPLICATION_PROCEDURE_RETURN_ADDRESS));
}

void AssemblerGeneratorVisitor::generateDivisionProcedure() {
    const int jumpToCurrentProcedureIndex = unresolvedJumpsCounter_;
    unresolvedJumpsCounter_++;
    procedureNameToJumpIndex.insert({DIVISION_PROCEDURE_NAME, jumpToCurrentProcedureIndex});

    writeToOutputFile(RESULT_LABEL + std::to_string(jumpToCurrentProcedureIndex) + RESULT_LABEL);

    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("LOAD 2");
    writeLineToOutputFile("SUB 13");
    writeLineToOutputFile("JZERO 63");
    writeLineToOutputFile("LOAD 1");
    writeLineToOutputFile("SUB 13");
    writeLineToOutputFile("JNEG 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 7");
    writeLineToOutputFile("JUMP 6");
    writeLineToOutputFile("LOAD 14");
    writeLineToOutputFile("STORE 7");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 1");
    writeLineToOutputFile("STORE 1");
    writeLineToOutputFile("LOAD 2");
    writeLineToOutputFile("SUB 13");
    writeLineToOutputFile("JNEG 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("STORE 8");
    writeLineToOutputFile("JUMP 6");
    writeLineToOutputFile("LOAD 14");
    writeLineToOutputFile("STORE 8");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 2");
    writeLineToOutputFile("STORE 2");
    writeLineToOutputFile("LOAD 1");
    writeLineToOutputFile("SUB 2");
    writeLineToOutputFile("JNEG 28");
    writeLineToOutputFile("LOAD 2");
    writeLineToOutputFile("STORE 4");
    writeLineToOutputFile("LOAD 14");
    writeLineToOutputFile("STORE 6");
    writeLineToOutputFile("LOAD 4");
    writeLineToOutputFile("ADD 4");
    writeLineToOutputFile("STORE 5");
    writeLineToOutputFile("LOAD 1");
    writeLineToOutputFile("SUB 5");
    writeLineToOutputFile("JNEG 11");
    writeLineToOutputFile("LOAD 4");
    writeLineToOutputFile("ADD 4");
    writeLineToOutputFile("STORE 4");
    writeLineToOutputFile("LOAD 6");
    writeLineToOutputFile("ADD 6");
    writeLineToOutputFile("STORE 6");
    writeLineToOutputFile("LOAD 4");
    writeLineToOutputFile("ADD 4");
    writeLineToOutputFile("STORE 5");
    writeLineToOutputFile("JUMP -12");
    writeLineToOutputFile("LOAD 1");
    writeLineToOutputFile("SUB 4");
    writeLineToOutputFile("STORE 1");
    writeLineToOutputFile("LOAD 3");
    writeLineToOutputFile("ADD 6");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("JUMP -29");
    writeLineToOutputFile("LOAD 7");
    writeLineToOutputFile("ADD 8");
    writeLineToOutputFile("STORE 4");
    writeLineToOutputFile("LOAD 4");
    writeLineToOutputFile("SUB 14");
    writeLineToOutputFile("JZERO 2");
    writeLineToOutputFile("JUMP 4");
    writeLineToOutputFile("LOAD 13");
    writeLineToOutputFile("SUB 3");
    writeLineToOutputFile("STORE 3");
    writeLineToOutputFile("RTRN " + std::to_string(DIVISION_PROCEDURE_RETURN_ADDRESS));
}

std::string AssemblerGeneratorVisitor::getGeneratedAssemblerCode() const {
    return outputAssemblerCode_;
}
