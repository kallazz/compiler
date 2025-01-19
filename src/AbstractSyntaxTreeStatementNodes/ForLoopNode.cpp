#include "ForLoopNode.hpp"
#include <iostream>

ForLoopNode::ForLoopNode(const int lineNumber, const std::string iteratorName, ValueNode* startValueNode, ValueNode* endValueNode, CommandsNode* commandsNode, bool isIteratorIncremented)
    : AbstractSyntaxTreeStatementNode(lineNumber), iteratorName_(iteratorName), startValueNode_(startValueNode), endValueNode_(endValueNode), commandsNode_(commandsNode), isIteratorIncremented_(isIteratorIncremented) {}

bool ForLoopNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitForLoopNode(*this);
}

void ForLoopNode::print() const {
    std::cout << "ForLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}

const std::string& ForLoopNode::getIteratorName() const {
    return iteratorName_;
}

const std::unique_ptr<ValueNode>& ForLoopNode::getStartValueNode() const {
    return startValueNode_;
}

const std::unique_ptr<ValueNode>& ForLoopNode::getEndValueNode() const {
    return endValueNode_;
}

const std::unique_ptr<CommandsNode>& ForLoopNode::getCommandsNode() const {
    return commandsNode_;
}

bool ForLoopNode::isIteratorIncremented() const {
    return isIteratorIncremented_;
}
