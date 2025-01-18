#include "WhileLoopNode.hpp"
#include <iostream>

WhileLoopNode::WhileLoopNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), conditionNode_(conditionNode), commandsNode_(commandsNode) {}

bool WhileLoopNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (conditionNode_ && !conditionNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (commandsNode_ && !commandsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void WhileLoopNode::print() const {
    std::cout << "WhileLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}