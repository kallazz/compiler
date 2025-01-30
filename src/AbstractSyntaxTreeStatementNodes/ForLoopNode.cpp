#include "ForLoopNode.hpp"

#include "AssemblerGeneratorVisitor.hpp"
#include "SemanticAnalysisVisitor.hpp"

ForLoopNode::ForLoopNode(const int lineNumber, const std::string iteratorName, ValueNode* startValueNode,
                         ValueNode* endValueNode, CommandsNode* commandsNode, bool isIteratorIncremented)
    : AbstractSyntaxTreeStatementNode(lineNumber),
      iteratorName_(iteratorName),
      startValueNode_(startValueNode),
      endValueNode_(endValueNode),
      commandsNode_(commandsNode),
      isIteratorIncremented_(isIteratorIncremented) {}

void ForLoopNode::accept(AssemblerGeneratorVisitor& assemblerGeneratorVisitor) const {
    return assemblerGeneratorVisitor.visitForLoopNode(*this);
}

bool ForLoopNode::accept(SemanticAnalysisVisitor& semanticAnalysisVisitor) const {
    return semanticAnalysisVisitor.visitForLoopNode(*this);
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
