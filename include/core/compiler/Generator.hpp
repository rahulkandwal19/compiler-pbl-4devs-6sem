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
    void visit(WatchNode* node) override;
    void visit(PrintNode* node) override;

    void visit(EventNode* node) override { // (or Generator::visit in the .cpp)
        out << "    // Event: " << node->eventName << "\n";
        out << "    auto " << node->eventName << " = []() {\n";
        
        // Generate the C++ code for everything inside the event
        for (const auto& stmt : node->statements) {
            stmt->accept(this); 
        }
        
        out << "    };\n";
        // Optionally call it immediately for testing:
        out << "    " << node->eventName << "();\n\n";
    }

private:
    std::ofstream out;
};