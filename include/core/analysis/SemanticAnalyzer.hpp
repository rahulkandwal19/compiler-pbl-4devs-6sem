#pragma once
#include "core/builder/AST.hpp"
#include "core/analysis/SymbolTable.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <memory>

class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer();
    bool analyze(std::shared_ptr<ProgramNode> program);
    
    void visit(ProgramNode* node) override;
    void visit(GlobalSectionNode* node) override;
    void visit(ImportNode* node) override;
    void visit(MergeNode* node) override;
    void visit(SourceDefinitionNode* node) override;
    void visit(EventDefinitionNode* node) override;
    void visit(ObserverDefinitionNode* node) override;
    void visit(FunctionDefinitionNode* node) override;
    void visit(FunctionCallNode* node) override;
    void visit(LiteralNode* node) override;
    void visit(VariableNode* node) override;
    void visit(VariableDeclarationNode* node) override;
    void visit(AssignmentNode* node) override;
    void visit(UnaryOperatorNode* node) override;
    void visit(BinaryOperatorNode* node) override;
    void visit(IfStatementNode* node) override;
    void visit(WhileStatementNode* node) override;
    void visit(ForStatementNode* node) override;
    void visit(ReturnStatementNode* node) override;

    // Schedule nodes don't need semantic analysis
    // void visit(ScheduleNode* node) override;

    bool hasErrors() const { return errorCount > 0; }
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    SymbolTable symbolTable;
    int errorCount;
    std::vector<std::string> errors;
    std::string currentFunctionReturnType;
    
    void reportError(const std::string& msg);
    void reportWarning(const std::string& msg);
    
    SymbolType getExpressionType(std::shared_ptr<ASTNode> node);
    bool checkAssignment(const std::string& varName, std::shared_ptr<ASTNode> value);
};

void printSemanticErrors(const SemanticAnalyzer& analyzer);
