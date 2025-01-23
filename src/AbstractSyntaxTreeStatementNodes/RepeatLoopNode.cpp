#include "RepeatLoopNode.hpp"
#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"
#include <iostream>

RepeatLoopNode::RepeatLoopNode(const int lineNumber, CommandsNode* commandsNode, ConditionNode* conditionNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), commandsNode_(commandsNode), conditionNode_(conditionNode) {}

void RepeatLoopNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitRepeatLoopNode(*this);
}

bool RepeatLoopNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitRepeatLoopNode(*this);
}

void RepeatLoopNode::print() const {
    std::cout << "RepeatLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}

const std::unique_ptr<CommandsNode>& RepeatLoopNode::getCommandsNode() const {
    return commandsNode_;
}

const std::unique_ptr<ConditionNode>& RepeatLoopNode::getConditionNode() const {
    return conditionNode_;
}
