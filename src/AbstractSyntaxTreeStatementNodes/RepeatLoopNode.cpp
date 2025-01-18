#include "RepeatLoopNode.hpp"
#include <iostream>

RepeatLoopNode::RepeatLoopNode(const int lineNumber, CommandsNode* commandsNode, ConditionNode* conditionNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), commandsNode_(commandsNode), conditionNode_(conditionNode) {}

bool RepeatLoopNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (commandsNode_ && !commandsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (conditionNode_ && !conditionNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void RepeatLoopNode::print() const {
    std::cout << "RepeatLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}