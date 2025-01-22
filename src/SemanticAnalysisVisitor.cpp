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

SemanticAnalysisVisitor::SemanticAnalysisVisitor(SymbolTable& symbolTable) : symbolTable_(symbolTable), currentProcedureName_(std::nullopt), willNumberVariableBeModified_(false) {}

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
            return symbolTable_.checkIfNumberVariableExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), willNumberVariableBeModified_, *currentProcedureName_);
        }
        return symbolTable_.checkIfNumberVariableExistsInMain(identifierNode.getLineNumber(), identifierNode.getName(), willNumberVariableBeModified_);
    }

    if (!identifierNode.getIndexName()) {
        if (isProcedureBeingProcessed()) {
            return symbolTable_.checkIfArrayVariableExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), identifierNode.getIndexValue(), *currentProcedureName_);
        }
        return symbolTable_.checkIfArrayVariableExistsInMain(identifierNode.getLineNumber(), identifierNode.getName(), identifierNode.getIndexValue());
    }

    if (isProcedureBeingProcessed()) {
        return symbolTable_.checkIfArrayVariableExistsInProcedure(identifierNode.getLineNumber(), identifierNode.getName(), std::nullopt, *currentProcedureName_) && symbolTable_.checkIfNumberVariableExistsInProcedure(identifierNode.getLineNumber(), *(identifierNode.getIndexName()), false, *currentProcedureName_);
    }
    return symbolTable_.checkIfArrayVariableExistsInMain(identifierNode.getLineNumber(), identifierNode.getName(), std::nullopt) && symbolTable_.checkIfNumberVariableExistsInMain(identifierNode.getLineNumber(), *(identifierNode.getIndexName()), false);
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
    willNumberVariableBeModified_ = true;

    // This if is for handling x := x case
    if (assignmentNode.getIdentifierNode() && assignmentNode.getExpressionNode() && !assignmentNode.getExpressionNode()->getValueNode2()
        && assignmentNode.getExpressionNode()->getValueNode1() && assignmentNode.getExpressionNode()->getValueNode1()->getIdentifierNode()
        && assignmentNode.getExpressionNode()->getValueNode1()->getIdentifierNode()->getName() == assignmentNode.getIdentifierNode()->getName()) {
        willNumberVariableBeModified_ = false;
    }

    if (assignmentNode.getIdentifierNode() && !assignmentNode.getIdentifierNode()->accept(*this)) {
        willNumberVariableBeModified_ = false;
        return false;
    }

    willNumberVariableBeModified_ = false;

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
                wasEvaluationSuccessful = symbolTable_.declareArrayVariableInProcedure(declaration.lineNumber, declaration.name, *declaration.arrayLowerBound, *declaration.arrayUpperBound, *currentProcedureName_);
            } else {
                wasEvaluationSuccessful = symbolTable_.declareArrayVariableInMain(declaration.lineNumber, declaration.name, *declaration.arrayLowerBound, *declaration.arrayUpperBound);
            }
        } else {
            if (isProcedureBeingProcessed()) {
                wasEvaluationSuccessful = symbolTable_.declareNumberVariableInProcedure(declaration.lineNumber, declaration.name, *currentProcedureName_);
            } else {
                wasEvaluationSuccessful = symbolTable_.declareNumberVariableInMain(declaration.lineNumber, declaration.name);
            }
        }

        if (!wasEvaluationSuccessful) {
            return false;
        }
    }

    return true;
}

bool SemanticAnalysisVisitor::visitForLoopNode(const ForLoopNode& forLoopNode) {
    const std::string& iteratorName = forLoopNode.getIteratorName();

    if (isProcedureBeingProcessed()) {
        if (!symbolTable_.declareNumberVariableInProcedure(forLoopNode.getLineNumber(), iteratorName, *currentProcedureName_, true, true)) {
            return false;
        }
    } else {
        if (!symbolTable_.declareNumberVariableInMain(forLoopNode.getLineNumber(), iteratorName, true, true)) {
            return false;
        }
    }

    if (forLoopNode.getStartValueNode() && !forLoopNode.getStartValueNode()->accept(*this)) {
        return false;
    }

    if (forLoopNode.getEndValueNode() && !forLoopNode.getEndValueNode()->accept(*this)) {
        return false;
    }

    if (forLoopNode.getCommandsNode() && !forLoopNode.getCommandsNode()->accept(*this)) {
        return false;
    }

    if (isProcedureBeingProcessed()) {
        symbolTable_.removeVariableFromProcedure(iteratorName, *currentProcedureName_);
    } else {
        symbolTable_.removeVariableFromMain(iteratorName);
    }

    return true;
}

bool SemanticAnalysisVisitor::visitIfNode(const IfNode& ifNode) {
    if (ifNode.getConditionNode() && !ifNode.getConditionNode()->accept(*this)) {
        return false;
    }

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
    return symbolTable_.verifyProcedureCall(procedureCallNode.getLineNumber(), procedureCallNode.getName(), arguments, currentProcedureName_);
}

bool SemanticAnalysisVisitor::visitProcedureHeadNode(const ProcedureHeadNode& procedureHeadNode) {
    const auto& argumentsDeclarations = procedureHeadNode.getArgumentsDeclarationNode()->getArgumentDeclarations();
    return symbolTable_.declareProcedure(procedureHeadNode.getLineNumber(), procedureHeadNode.getName(), argumentsDeclarations);
}

bool SemanticAnalysisVisitor::visitProceduresNode(const ProceduresNode& proceduresNode) {
    for (const auto& procedure : proceduresNode.getProcedures()) {
        currentProcedureName_ = procedure->procedureHeadNode->getName();

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

    currentProcedureName_ = std::nullopt;
    return true;
}

bool SemanticAnalysisVisitor::visitReadNode(const ReadNode& readNode) {
    willNumberVariableBeModified_ = true;

    if (readNode.getIdentifierNode() && !readNode.getIdentifierNode()->accept(*this)) {
        willNumberVariableBeModified_ = false;
        return false;
    }

    willNumberVariableBeModified_ = false;
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
    return currentProcedureName_ != std::nullopt;
}