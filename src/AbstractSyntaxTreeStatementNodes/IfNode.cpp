#include "IfNode.hpp"
#include <iostream>

IfNode::IfNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), conditionNode_(conditionNode), thenCommandsNode_(thenCommandsNode), elseCommandsNode_(elseCommandsNode) {}

bool IfNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (thenCommandsNode_ && !thenCommandsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (elseCommandsNode_ && !elseCommandsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void IfNode::print() const {
    std::cout << "IfNode\n{\n";
    std::cout << getLineNumber();
    if (thenCommandsNode_) {
        thenCommandsNode_->print();
    }
    std::cout << "}";
    if (elseCommandsNode_) {
        std::cout << "\n!{\n";
        elseCommandsNode_->print();
        std::cout << "!}";
    }
}