#ifndef FOR_LOOP_NODE_HPP
#define FOR_LOOP_NODE_HPP

#include "AbstractSyntaxTreeStatementNode.hpp"
#include "CommandsNode.hpp"
#include "AbstractSyntaxTreeExpressionNodes/ValueNode.hpp"
#include <memory>
#include <string>

class ForLoopNode : public AbstractSyntaxTreeStatementNode {
public:
    ForLoopNode(const std::string iteratorName, ValueNode* startValueNode, ValueNode* endValueNode, CommandsNode* commandsNode, bool isIteratorIncremented);

    void print() const override;

private:
    const std::string iteratorName_;
    const std::unique_ptr<ValueNode> startValueNode_;
    const std::unique_ptr<ValueNode> endValueNode_;
    const std::unique_ptr<CommandsNode> commandsNode_;
    const bool isIteratorIncremented_;
};

#endif // FOR_LOOP_NODE_HPP