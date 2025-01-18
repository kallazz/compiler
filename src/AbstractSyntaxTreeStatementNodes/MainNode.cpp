#include "MainNode.hpp"
#include <iostream>

MainNode::MainNode(const int lineNumber, DeclarationsNode* declarationsNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), declarationsNode_(declarationsNode), commandsNode_(commandsNode) {}

MainNode::MainNode(const int lineNumber, CommandsNode* commandsNode) : AbstractSyntaxTreeStatementNode(lineNumber), commandsNode_(commandsNode) {}

bool MainNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (declarationsNode_ && !declarationsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    if (commandsNode_ && !commandsNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void MainNode::print() const {
    std::cout << "MainNode\n";
    if (commandsNode_) {
        commandsNode_->print();
        std::cout << '\n';
    }
}