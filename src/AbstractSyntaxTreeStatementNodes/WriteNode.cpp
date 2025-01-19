#include "WriteNode.hpp"
#include <iostream>

WriteNode::WriteNode(const int lineNumber, ValueNode* valueNode) : AbstractSyntaxTreeStatementNode(lineNumber), valueNode_(valueNode) {}

bool WriteNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitWriteNode(*this);
}

void WriteNode::print() const {
    std::cout << "WriteNode";
}

const std::unique_ptr<ValueNode>& WriteNode::getValueNode() const {
    return valueNode_;
}
