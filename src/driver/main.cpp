#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "antlr4-runtime.h"
#include "AMSLexer.h"
#include "AMSParser.h"   

#include "core/supports/caseCaptilizeInputStream.hpp"
#include "core/builder/AST.hpp"       
#include "core/builder/Builder.hpp"
#include "core/compiler/Generator.hpp" 
#include "core/analysis/SemanticAnalyzer.hpp"


using namespace antlr4;

namespace {
    void printIndent(int level) {
        std::cout << std::string(static_cast<size_t>(level) * 2, ' ');
    }

    const char* scheduleTypeToString(ScheduleType type) {
        switch (type) {
            case ScheduleType::AT: return "AT";
            case ScheduleType::EVERY: return "EVERY";
            case ScheduleType::CRON: return "CRON";
            default: return "UNKNOWN";
        }
    }

    void printAstNode(const std::shared_ptr<ASTNode>& node, int indent = 0) {
        if (!node) {
            printIndent(indent);
            std::cout << "null" << std::endl;
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ProgramNode>(node)) {
            printIndent(indent);
            std::cout << "ProgramNode" << std::endl;
            for (const auto& block : n->programBlocks) {
                printAstNode(block, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<GlobalSectionNode>(node)) {
            printIndent(indent);
            std::cout << "GlobalSectionNode" << std::endl;
            for (const auto& stmt : n->statements) {
                printAstNode(stmt, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ImportNode>(node)) {
            printIndent(indent);
            std::cout << "ImportNode(module=\"" << n->moduleName << "\")" << std::endl;
            return;
        }

        if (auto n = std::dynamic_pointer_cast<MergeNode>(node)) {
            printIndent(indent);
            std::cout << "MergeNode(target=\"" << n->targetName << "\")" << std::endl;
            return;
        }

        if (auto n = std::dynamic_pointer_cast<SourceDefinitionNode>(node)) {
            printIndent(indent);
            std::cout << "SourceDefinitionNode(name=\"" << n->sourceName << "\")" << std::endl;
            if (n->schedule) {
                printIndent(indent + 1);
                std::cout << "schedule:" << std::endl;
                printAstNode(n->schedule, indent + 2);
            }
            for (const auto& stmt : n->statements) {
                printAstNode(stmt, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<EventDefinitionNode>(node)) {
            printIndent(indent);
            std::cout << "EventDefinitionNode(name=\"" << n->eventName << "\")" << std::endl;
            if (n->schedule) {
                printIndent(indent + 1);
                std::cout << "schedule:" << std::endl;
                printAstNode(n->schedule, indent + 2);
            }
            for (const auto& stmt : n->statements) {
                printAstNode(stmt, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ObserverDefinitionNode>(node)) {
            printIndent(indent);
            std::cout << "ObserverDefinitionNode(name=\"" << n->observerName << "\")" << std::endl;
            if (n->schedule) {
                printIndent(indent + 1);
                std::cout << "schedule:" << std::endl;
                printAstNode(n->schedule, indent + 2);
            }
            for (const auto& stmt : n->statements) {
                printAstNode(stmt, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<FunctionDefinitionNode>(node)) {
            printIndent(indent);
            std::cout << "FunctionDefinitionNode(name=\"" << n->functionName << "\", params=[";
            for (size_t i = 0; i < n->parameters.size(); ++i) {
                std::cout << n->parameters[i];
                if (i + 1 < n->parameters.size()) std::cout << ", ";
            }
            std::cout << "])" << std::endl;
            for (const auto& stmt : n->statements) {
                printAstNode(stmt, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<FunctionCallNode>(node)) {
            printIndent(indent);
            std::cout << "FunctionCallNode(name=\"" << n->functionName << "\")" << std::endl;
            for (const auto& arg : n->arguments) {
                printAstNode(arg, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<LiteralNode>(node)) {
            printIndent(indent);
            std::cout << "LiteralNode(value=" << n->value << ")" << std::endl;
            return;
        }

        if (auto n = std::dynamic_pointer_cast<VariableNode>(node)) {
            printIndent(indent);
            std::cout << "VariableNode(name=" << n->name << ")" << std::endl;
            return;
        }

        if (auto n = std::dynamic_pointer_cast<VariableDeclarationNode>(node)) {
            printIndent(indent);
            std::cout << "VariableDeclarationNode(type=" << n->dataType << ", name=" << n->varName << ")" << std::endl;
            if (n->value) {
                printIndent(indent + 1);
                std::cout << "value:" << std::endl;
                printAstNode(n->value, indent + 2);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<AssignmentNode>(node)) {
            printIndent(indent);
            std::cout << "AssignmentNode(name=" << n->varName << ")" << std::endl;
            printAstNode(n->expression, indent + 1);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<UnaryOperatorNode>(node)) {
            printIndent(indent);
            std::cout << "UnaryOperatorNode(op=" << n->op << ")" << std::endl;
            printAstNode(n->right, indent + 1);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<BinaryOperatorNode>(node)) {
            printIndent(indent);
            std::cout << "BinaryOperatorNode(op=" << n->op << ")" << std::endl;
            printIndent(indent + 1);
            std::cout << "left:" << std::endl;
            printAstNode(n->left, indent + 2);
            printIndent(indent + 1);
            std::cout << "right:" << std::endl;
            printAstNode(n->right, indent + 2);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<IfStatementNode>(node)) {
            printIndent(indent);
            std::cout << "IfStatementNode" << std::endl;
            printIndent(indent + 1);
            std::cout << "condition:" << std::endl;
            printAstNode(n->condition, indent + 2);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<WhileStatementNode>(node)) {
            printIndent(indent);
            std::cout << "WhileStatementNode" << std::endl;
            printIndent(indent + 1);
            std::cout << "condition:" << std::endl;
            printAstNode(n->condition, indent + 2);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ForStatementNode>(node)) {
            printIndent(indent);
            std::cout << "ForStatementNode" << std::endl;
            printIndent(indent + 1);
            std::cout << "initialization:" << std::endl;
            printAstNode(n->initialization, indent + 2);
            printIndent(indent + 1);
            std::cout << "condition:" << std::endl;
            printAstNode(n->condition, indent + 2);
            printIndent(indent + 1);
            std::cout << "increment:" << std::endl;
            printAstNode(n->increment, indent + 2);
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ReturnStatementNode>(node)) {
            printIndent(indent);
            std::cout << "ReturnStatementNode" << std::endl;
            if (n->returnValue) {
                printAstNode(n->returnValue, indent + 1);
            }
            return;
        }

        if (auto n = std::dynamic_pointer_cast<ScheduleNode>(node)) {
            printIndent(indent);
            std::cout << "ScheduleNode(type=" << scheduleTypeToString(n->type)
                      << ", value=" << n->value;
            if (!n->unit.empty()) {
                std::cout << ", unit=" << n->unit;
            }
            std::cout << ")" << std::endl;
            return;
        }

        printIndent(indent);
        std::cout << "UnknownNode" << std::endl;
    }
}

int main(int argc, const char* argv[]) {
    // Help Verbose for the dempnstration of Usage Options Available in AMS-Lang Engine 
    if (argc < 3) {
        std::cout << "AutomonScript Language Engine Usage:" << std::endl<<std::endl;
        std::cout << "******************** Executing .ams Programes ***************" << std::endl;
        std::cout << "ams build <file.ams> [-o <output_path>] (Compile the Code to Executable)" << std::endl;
        std::cout << "**************** View AMS Engine Language Working ***********" << std::endl;
        std::cout << "ams lexer <file.ams> (View Tokens At LEXICAL ANALYSIS)" << std::endl;
        std::cout << "ams parser <file.ams> (View ParseTree At SYNTAX ANALYSIS)" << std::endl;
        std::cout << "ams ast <file.ams> (View Abstract Syntax Tree)" << std::endl;
        return 1;
    }

    std::string mode = argv[1];     
    std::string filePath = argv[2]; 
    std::filesystem::path inputPath(filePath);
    std::ifstream stream(filePath);
    if (!stream.good()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    // Using Insted of ANTLRInputStream (Extended Class of ANTLRInputStream)
    // Defined in ./include/supports/caseCaptilizeInputStream.hpp
    // Used to make Language Case Independent (Run, run, RUN, ruN are all same)
    CaseCaptilizeInputStream input(stream);

    AMSLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    AMSParser parser(&tokens);

    // Build The Parse Tree of Source Code Using ANTLR4
    tree::ParseTree* parseTree = parser.program();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        std::cerr << "Syntax Error!" << std::endl;
        return 1;
    }

    // Build The Abstract Syntax Tree of  Code Using IR Builder
    Builder astBuilder;
    auto ast = std::any_cast<std::shared_ptr<ProgramNode>>(astBuilder.visit(parseTree));

    // Run Semantic Analysis
    SemanticAnalyzer semanticAnalyzer;
    if (!semanticAnalyzer.analyze(ast)) {
        std::cerr << "[SEMANTIC ANALYSIS FAILED]" << std::endl;
        printSemanticErrors(semanticAnalyzer);
        return 1;
    }
    std::cout << "[SEMANTIC ANALYSIS PASSED]" << std::endl;
  
    if (mode == "build") {
        //Compiled :  Produces an executable file
        std::string tempCpp = "temp_output.cpp";
        std::filesystem::path exePath = inputPath;
        bool useCustomOutput = false;

        if (argc > 3) {
            if (argc == 5 && std::string(argv[3]) == "-o") {
                exePath = std::filesystem::path(argv[4]);
                useCustomOutput = true;
            } else {
                std::cerr << "Invalid build options. Usage: ams build <file.ams> [-o <output_path>]" << std::endl;
                return 1;
            }
        }
        
        #ifdef _WIN32
            if (!useCustomOutput) {
                exePath.replace_extension(".exe");
            } else if (exePath.extension().empty()) {
                exePath += ".exe";
            }
            std::string flag = " -mconsole";
        #elif __linux__
            if (!useCustomOutput) {
                exePath.replace_extension("");
            }
            std::string flag = "";
        #endif

        Generator generator(tempCpp, filePath);
        generator.generate(ast);
        generator.close();

        // Compile to Executable
        std::string compileCmd = "g++ " + tempCpp + " -o \"" + exePath.string() + "\"" + flag;

        if (std::system(compileCmd.c_str()) == 0) {
            // Clean up intermediate code
            std::filesystem::remove(tempCpp); 
            std::cout << "[SUCCESS] Created: " << exePath.filename().string() << std::endl;
        } else {
            std::cerr << "[ERROR] Compilation failed." << std::endl;
        }
    }
    else if(mode=="lexer" || mode=="parser" || mode=="ast"){
        if(mode == "lexer"){
            tokens.fill();
            for (auto token : tokens.getTokens()) {
                std::cout << token->toString() << std::endl;
            }
        }

        else if(mode == "parser"){
            std::cout << "Program Parse Tree: " << std::endl;
            std::cout << parseTree->toStringTree(&parser) << std::endl;
        }

        else if (mode == "ast") {
            std::cout << "Program AST: " << std::endl;
            printAstNode(ast);
        }
    }
    else {
        //Unknown Execution Mode : ERROR! Verbose
        std::cerr << "Unknown mode: " << mode << ". Use 'build', 'lexer', 'parser', or 'ast'." << std::endl;
        return 1;
    }

    return 0;
}