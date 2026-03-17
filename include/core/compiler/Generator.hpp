#pragma once
#include "core/builder/AST.hpp"
#include <fstream>
#include <string>

class Generator : public ASTVisitor {
public:
    Generator(const std::string& outputFile);
    ~Generator();

    void generate(std::shared_ptr<ProgramNode> program);
    
    void close() { if (out.is_open()) out.close(); }

    void visit(ProgramNode* node) override;
    void visit(PrintNode* node) override;

    void visit(EventNode* node) override {
        out << "    // Event: " << node->eventName << "\n";
        out << "    auto " << node->eventName << " = []() {\n";
        

        for (const auto& stmt : node->statements) {
            stmt->accept(this); 
        }
        
        out << "    };\n";

        out << "    " << node->eventName << "();\n\n";
    }

    void visit(SourceDefNode* node) override {
        out << "    // Source Block: " << node->sourceName << "\n";
        
        for (const auto& item : node->items) {
            item->accept(this); 
        }
    }

    void visit(SourceItemNode* node) override {
        out << "    // Auto-generated Source Item: " << node->variableName << "\n";
        out << "    std::string " << node->variableName << "_type = \"" << node->resourceType << "\";\n";
        out << "    std::string " << node->variableName << "_url = \"" << node->url << "\";\n";
    }

private:
    std::ofstream out;
};