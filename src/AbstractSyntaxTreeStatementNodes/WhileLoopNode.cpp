#include "WhileLoopNode.hpp"
#include <iostream>

WhileLoopNode::WhileLoopNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* commandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), conditionNode_(conditionNode), commandsNode_(commandsNode) {}

bool WhileLoopNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitWhileLoopNode(*this);
}

void WhileLoopNode::print() const {
    std::cout << "WhileLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}

const std::unique_ptr<ConditionNode>& WhileLoopNode::getConditionNode() const {
    return conditionNode_;
}

const std::unique_ptr<CommandsNode>& WhileLoopNode::getCommandsNode() const {
    return commandsNode_;
}
