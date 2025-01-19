#include "SemanticAnalysisVisitor.hpp"
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

SemanticAnalysisVisitor::SemanticAnalysisVisitor(SymbolTable& symbolTable) : symbolTable_(symbolTable), currentProcedureIndex_(std::nullopt) {}

bool SemanticAnalysisVisitor::visitConditionNode(const ConditionNode& conditionNode) {
    if (conditionNode.getValueNode1() && !conditionNode.getValueNode1()->accept(*this)) {
        return false;
    }

    if (conditionNode.getValueNode2() && !conditionNode.getValueNode2()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitExpressionNode(const ExpressionNode& expressionNode) {
    if (expressionNode.getValueNode1() && !expressionNode.getValueNode1()->accept(*this)) {
        return false;
    }

    if (expressionNode.getValueNode2() && !expressionNode.getValueNode2()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitIdentifierNode(const IdentifierNode& identifierNode) {
    if (!identifierNode.getIndexName() && !identifierNode.getIndexValue()) {
        if (isProcedureBeingProcessed()) {
            return symbolTable_.checkIfVariableExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), *currentProcedureIndex_);
        }
        return symbolTable_.checkIfVariableExists(identifierNode.getLineNumber(), identifierNode.getName());
    }

    if (!identifierNode.getIndexName()) {
        if (isProcedureBeingProcessed()) {
            return symbolTable_.checkIfArrayExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), identifierNode.getIndexValue(), *currentProcedureIndex_);
        }
        return symbolTable_.checkIfArrayExists(identifierNode.getLineNumber(), identifierNode.getName(), identifierNode.getIndexValue());
    }

    if (isProcedureBeingProcessed()) {
        return symbolTable_.checkIfArrayExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), std::nullopt, *currentProcedureIndex_);
    }
    return symbolTable_.checkIfArrayExists(identifierNode.getLineNumber(), identifierNode.getName(), std::nullopt);
}

bool SemanticAnalysisVisitor::visitValueNode(const ValueNode& valueNode) {
    if (valueNode.getNumber()) {
        return true;
    }

    return valueNode.getIdentifierNode() && valueNode.getIdentifierNode()->accept(*this);
}


bool SemanticAnalysisVisitor::visitArgumentsDeclarationNode(const ArgumentsDeclarationNode&) {
    return true;
}

bool SemanticAnalysisVisitor::visitArgumentsNode(const ArgumentsNode&) {
    return true;
}

bool SemanticAnalysisVisitor::visitAssignmentNode(const AssignmentNode& assignmentNode) {
    if (assignmentNode.getIdentifierNode() && !assignmentNode.getIdentifierNode()->accept(*this)) {
        return false;
    }

    if (assignmentNode.getExpressionNode() && !assignmentNode.getExpressionNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitCommandsNode(const CommandsNode& commandsNode) {
    for (const auto& command : commandsNode.getCommandNodes()) {
        if (!command->accept(*this)) {
            return false;
        }
    }

    return true;
}

bool SemanticAnalysisVisitor::visitDeclarationsNode(const DeclarationsNode& declarationsNode) {
    for (const auto& declaration : declarationsNode.getDeclarations()) {
        bool wasEvaluationSuccessful = false;

        if (declaration.arrayLowerBound && declaration.arrayUpperBound) {
            if (isProcedureBeingProcessed()) {
                wasEvaluationSuccessful = symbolTable_.declareArrayInProcedure(declaration.lineNumber, declaration.name, *declaration.arrayLowerBound, *declaration.arrayUpperBound, *currentProcedureIndex_);
            } else {
                wasEvaluationSuccessful = symbolTable_.declareArray(declaration.lineNumber, declaration.name, *declaration.arrayLowerBound, *declaration.arrayUpperBound);
            }
        } else {
            if (isProcedureBeingProcessed()) {
                wasEvaluationSuccessful = symbolTable_.declareVariableInProcedure(declaration.lineNumber, declaration.name, *currentProcedureIndex_);
            } else {
                wasEvaluationSuccessful = symbolTable_.declareVariable(declaration.lineNumber, declaration.name);
            }
        }

        if (!wasEvaluationSuccessful) {
            return false;
        }
    }

    return true;
}

bool SemanticAnalysisVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    // TODO: check for assignments to iterator in commands
    return true;
}

bool SemanticAnalysisVisitor::visitIfNode(const IfNode& ifNode) {
    if (ifNode.getThenCommandsNode() && !ifNode.getThenCommandsNode()->accept(*this)) {
        return false;
    }

    if (ifNode.getElseCommandsNode() && !ifNode.getElseCommandsNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitMainNode(const MainNode& mainNode) {
    if (mainNode.getDeclarationsNode() && !mainNode.getDeclarationsNode()->accept(*this)) {
        return false;
    }

    if (mainNode.getCommandsNode() && !mainNode.getCommandsNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitProcedureCallNode(const ProcedureCallNode& procedureCallNode) {
    const auto& arguments = procedureCallNode.getArgumentsNode()->getArguments();
    return symbolTable_.checkIfProcedureExists(procedureCallNode.getLineNumber(), procedureCallNode.getName(), arguments);
}

bool SemanticAnalysisVisitor::visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode) {
    const auto& argumentsDeclarations = procedureHeadNode.getArgumentsDeclarationNode()->getArgumentDeclarations();
    return symbolTable_.declareProcedure(procedureHeadNode.getLineNumber(), procedureHeadNode.getName(), argumentsDeclarations, *currentProcedureIndex_);
}

bool SemanticAnalysisVisitor::visitProceduresNode(const ProceduresNode& proceduresNode) {
    for (const auto& procedure : proceduresNode.getProcedures()) {
        currentProcedureIndex_ = procedure->procedureIndex;

        if (procedure->procedureHeadNode && !procedure->procedureHeadNode->accept(*this)) {
            return false;
        }

        if (procedure->declarationsNode && !procedure->declarationsNode->accept(*this)) {
            return false;
        }

        if (procedure->commandsNode && !procedure->commandsNode->accept(*this)) {
            return false;
        }
    }

    currentProcedureIndex_ = std::nullopt;
    return true;
}

bool SemanticAnalysisVisitor::visitReadNode(const ReadNode& readNode) {
    if (readNode.getIdentifierNode() && !readNode.getIdentifierNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitRepeatLoopNode(const RepeatLoopNode& repeatLoopNode) {
    if (repeatLoopNode.getCommandsNode() && !repeatLoopNode.getCommandsNode()->accept(*this)) {
        return false;
    }

    if (repeatLoopNode.getConditionNode() && !repeatLoopNode.getConditionNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitWhileLoopNode(const WhileLoopNode& whileLoopNode) {
    if (whileLoopNode.getConditionNode() && !whileLoopNode.getConditionNode()->accept(*this)) {
        return false;
    }

    if (whileLoopNode.getCommandsNode() && !whileLoopNode.getCommandsNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::visitWriteNode(const WriteNode& writeNode) {
    if (writeNode.getValueNode() && !writeNode.getValueNode()->accept(*this)) {
        return false;
    }

    return true;
}

bool SemanticAnalysisVisitor::isProcedureBeingProcessed() {
    return currentProcedureIndex_ != std::nullopt;
}