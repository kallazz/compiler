#include "WriteNode.hpp"
#include <iostream>

WriteNode::WriteNode(const int lineNumber, ValueNode* valueNode) : AbstractSyntaxTreeStatementNode(lineNumber), valueNode_(valueNode) {}

bool WriteNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    if (valueNode_ && !valueNode_->evaluateBySymbolTable(symbolTable)) {
        return false;
    }

    return true;
}

void WriteNode::print() const {
    std::cout << "WriteNode";
}