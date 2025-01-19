#include "IfNode.hpp"
#include <iostream>

IfNode::IfNode(const int lineNumber, ConditionNode* conditionNode, CommandsNode* thenCommandsNode, CommandsNode* elseCommandsNode)
    : AbstractSyntaxTreeStatementNode(lineNumber), conditionNode_(conditionNode), thenCommandsNode_(thenCommandsNode), elseCommandsNode_(elseCommandsNode) {}

bool IfNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitIfNode(*this);
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

const std::unique_ptr<ConditionNode>& IfNode::getConditionNode() const {
    return conditionNode_;
}

const std::unique_ptr<CommandsNode>& IfNode::getThenCommandsNode() const {
    return thenCommandsNode_;
}

const std::unique_ptr<CommandsNode>& IfNode::getElseCommandsNode() const {
    return elseCommandsNode_;
}
