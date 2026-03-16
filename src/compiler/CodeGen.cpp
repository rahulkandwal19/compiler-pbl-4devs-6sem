#include "core/compiler/Generator.hpp"

Generator::Generator(const std::string& outputFile) : out(outputFile) {}

Generator::~Generator() {
    if (out.is_open()) out.close();
}

void Generator::generate(std::shared_ptr<ProgramNode> program) {
    program->accept(this);
}

void Generator::visit(ProgramNode* node) {
    out << "#include <iostream>\n";
    out << "#include <string>\n\n";
    out << "int main() {\n";
    
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    
    out << "    return 0;\n";
    out << "}\n";
}

void Generator::visit(WatchNode* node) {
    out << " std::cout << \"[System] Initializing " << node->resourceType 
        << " Watcher for: " << node->variableName << "\" << std::endl;\n";
}

void Generator::visit(PrintNode* node) {
    out << "    std::cout << \"" << node->text << "\" << std::endl;\n";
}