#include "ForLoopNode.hpp"
#include <iostream>

ForLoopNode::ForLoopNode(const int lineNumber, const std::string iteratorName, ValueNode* startValueNode, ValueNode* endValueNode, CommandsNode* commandsNode, bool isIteratorIncremented)
    : AbstractSyntaxTreeStatementNode(lineNumber), iteratorName_(iteratorName), startValueNode_(startValueNode), endValueNode_(endValueNode), commandsNode_(commandsNode), isIteratorIncremented_(isIteratorIncremented) {}

bool ForLoopNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    // TODO: check for assignments to iterator in commands

    return true;
}

void ForLoopNode::print() const {
    std::cout << "ForLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}