#include "ArgumentsNode.hpp"
#include "SymbolTable.hpp"
#include <iostream>

ArgumentsNode::ArgumentsNode(const int lineNumber) : AbstractSyntaxTreeStatementNode(lineNumber) {}

void ArgumentsNode::addArgument(const int lineNumber, const std::string name) {
    arguments_.push_back({lineNumber, name});
}

bool ArgumentsNode::evaluateBySymbolTable(SymbolTable& symbolTable) const {
    for (const auto& argument : arguments_) {
        if (!symbolTable.checkIfVariableOrArrayExists(argument.lineNumber, argument.name)) {
            return false;
        }
    }

    // TODO: Think how to make it work with procedure signatures

    return true;
}

void ArgumentsNode::print() const {
    std::cout << "ArgumentNode: ";

    std::cout << '\n';
}
