#include "ForLoopNode.hpp"
#include <iostream>

ForLoopNode::ForLoopNode(const std::string iteratorName, ValueNode* startValueNode, ValueNode* endValueNode, CommandsNode* commandsNode, bool isIteratorIncremented)
    : iteratorName_(iteratorName), startValueNode_(startValueNode), endValueNode_(endValueNode), commandsNode_(commandsNode), isIteratorIncremented_(isIteratorIncremented) {}

void ForLoopNode::print() const {
    std::cout << "ForLoopNode\n{\n";
    if (commandsNode_) {
        commandsNode_->print();
    }
    std::cout << "}";
}