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

    void visit(SourceDefNode* node) override {
        std::cout << "[System] Processing Source Block: " << node->sourceName << std::endl;

        for (const auto& item : node->items) {
            item->accept(this);
        }
    }

    void visit(SourceItemNode* node) override {
        std::cout << "[System] Initializing " << node->resourceType 
                  << " connection for: " << node->variableName 
                  << " at URL: " << node->url << std::endl;
    }

    void visit(PrintNode* node) override {
        std::cout << node->text << std::endl;
    }

    void visit(EventNode* node) override {
        std::cout << "[System] Registering Event: " << node->eventName << std::endl;
        for (const auto& stmt : node->statements) {
            stmt->accept(this);
        }
    }
};