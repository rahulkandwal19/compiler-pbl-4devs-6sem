#pragma once
#include "core/builder/AST.hpp"
#include <iostream>

class Evaluator : public ASTVisitor {
public:
    void evaluate(std::shared_ptr<ProgramNode> program) {
        program->accept(this); // Start the tree walk
    }

    void visit(ProgramNode* node) override {
        for (const auto& stmt : node->statements) {
            stmt->accept(this);
        }
    }

    void visit(WatchNode* node) override {
        std::cout << "[System] Initializing " << node->resourceType 
                  << " Watcher for: " << node->variableName << std::endl;
    }

    void visit(PrintNode* node) override {
        std::cout << node->text << std::endl;
    }

    void visit(EventNode* node) override {
        std::cout << "[System] Registering Event: " << node->eventName << std::endl;
        // For now, let's just execute the statements inside it immediately to test
        for (const auto& stmt : node->statements) {
            stmt->accept(this);
        }
    }
};