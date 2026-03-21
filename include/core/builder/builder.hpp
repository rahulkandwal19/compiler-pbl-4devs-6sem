#pragma once
#include "AMSBaseVisitor.h"
#include "AST.hpp"
#include <iostream>
#include <memory>
#include <any>

//:::::::::::::::::::::::::::::::::::::::::: Helpers :::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
std::shared_ptr<T> get_node(antlrcpp::Any res) {
    if (!res.has_value()) return nullptr;

    try {
        return std::any_cast<std::shared_ptr<T>>(res);
    } 
    catch (const std::bad_any_cast&) {
        try {
            auto basePtr = std::any_cast<std::shared_ptr<ASTNode>>(res);
            return std::dynamic_pointer_cast<T>(basePtr);
        } 
        catch (const std::bad_any_cast&) {
            return nullptr;
        }
    }
}

//:::::::::::::::::::::::::::::::::::::::::: Builder ::::::::::::::::::::::::::::::::::::::::::::::::::

class Builder : public AMSBaseVisitor {
public:
    //################################### Program Visitor #############################################
    virtual antlrcpp::Any visitProgram(AMSParser::ProgramContext *ctx) override {
        auto program = std::make_shared<ProgramNode>();
        
        if (ctx->globalSection()) {
            if (auto nodePtr = get_node<ASTNode>(visit(ctx->globalSection())))
                program->programBlocks.push_back(nodePtr);
        }
        
        if (ctx->sourcesSection()) {
            for (auto* d : ctx->sourcesSection()->sourceDefinition())
                if (auto nodePtr = get_node<ASTNode>(visit(d))) program->programBlocks.push_back(nodePtr);
        }

        if (ctx->eventsSection()) {
            for (auto* d : ctx->eventsSection()->eventDefinition())
                if (auto nodePtr = get_node<ASTNode>(visit(d))) program->programBlocks.push_back(nodePtr);
        }

        if (ctx->observersSection()) {
            for (auto* d : ctx->observersSection()->observerDefinition()) // Fixed to observerDefinition
                if (auto nodePtr = get_node<ASTNode>(visit(d))) program->programBlocks.push_back(nodePtr);
        }
        
        if (ctx->functionsSection()) {
            for (auto* d : ctx->functionsSection()->functionDefinition()) // Fixed to functionDefinition
                if (auto nodePtr = get_node<ASTNode>(visit(d))) program->programBlocks.push_back(nodePtr);
        }

        return program;
    }

    //#################################### Global Section  ###########################################
    virtual antlrcpp::Any visitGlobalSection(AMSParser::GlobalSectionContext *ctx) override {
        auto node = std::make_shared<GlobalSectionNode>();
        for (auto* item : ctx->globalItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    virtual antlrcpp::Any visitGlobalItem(AMSParser::GlobalItemContext *ctx) override {
        if (ctx->importStatement()) return visit(ctx->importStatement());
        if (ctx->mergeStatement())  return visit(ctx->mergeStatement());
        return visit(ctx->statement());
    }

    virtual antlrcpp::Any visitImportStatement(AMSParser::ImportStatementContext *ctx) override {
        std::string s = ctx->STRING_L()->getText();
        return std::static_pointer_cast<ASTNode>(std::make_shared<ImportNode>(s.substr(1, s.length()-2)));
    }

    virtual antlrcpp::Any visitMergeStatement(AMSParser::MergeStatementContext *ctx) override {
        std::string s = ctx->STRING_L()->getText();
        return std::static_pointer_cast<ASTNode>(std::make_shared<MergeNode>(s.substr(1, s.length()-2)));
    }

    //##################################### Source Definitions  #######################################
    virtual antlrcpp::Any visitSourceDefinition(AMSParser::SourceDefinitionContext *ctx) override {
        auto node = std::make_shared<SourceDefinitionNode>(ctx->ID()->getText());
        for (auto* item : ctx->sourceItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    //##################################### Events Definitions  #######################################
    virtual antlrcpp::Any visitEventDefinition(AMSParser::EventDefinitionContext *ctx) override {
        auto node = std::make_shared<EventDefinitionNode>(ctx->ID()->getText());
        for (auto* item : ctx->eventItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    //#################################### Observers Definitions  #####################################
    virtual antlrcpp::Any visitObserverDefinition(AMSParser::ObserverDefinitionContext *ctx) override {
        auto node = std::make_shared<ObserverDefinitionNode>(ctx->ID()->getText());
        for (auto* item : ctx->observerItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    //##################################### Function Definitions  #####################################
    virtual antlrcpp::Any visitFunctionDefinition(AMSParser::FunctionDefinitionContext *ctx) override {
        std::string name = ctx->ID()->getText();
        std::vector<std::string> params;

        if (ctx->parameters()) {
            for (auto* paramCtx : ctx->parameters()->parameter()) {
                params.push_back(paramCtx->ID()->getText());
            }
        }

        auto node = std::make_shared<FunctionDefinitionNode>(name, params);

        for (auto* item : ctx->functionItem()) {
            if (auto nodePtr = get_node<ASTNode>(visit(item))) {
                node->statements.push_back(nodePtr);
            }
        }

        return std::static_pointer_cast<ASTNode>(node);
    }

    virtual antlrcpp::Any visitFunctionCall(AMSParser::FunctionCallContext *ctx) override {
        std::string name = ctx->ID()->getText();
        std::vector<std::shared_ptr<ASTNode>> args;
        if (ctx->arguments()) {
            for (auto* argCtx : ctx->arguments()->argument()) {
                // Safe extraction using get_node!
                if (auto argNode = get_node<ASTNode>(visit(argCtx))) {
                    args.push_back(argNode);
                }
            }
        }
        return std::static_pointer_cast<ASTNode>(std::make_shared<FunctionCallNode>(name, args));
    }

    //########################################## Statements & Variables #################################
    virtual antlrcpp::Any visitStatement(AMSParser::StatementContext *ctx) override {
        if (ctx->functionCall()) return visit(ctx->functionCall());
        if (ctx->variableDeclaration()) return visit(ctx->variableDeclaration());
        if (ctx->assignment()) return visit(ctx->assignment());
        return antlrcpp::Any();
    }

    virtual antlrcpp::Any visitVariableDeclaration(AMSParser::VariableDeclarationContext *ctx) override {
        std::string type = ctx->dataType()->getText();
        std::string name = ctx->ID()->getText();
        std::shared_ptr<ASTNode> val = nullptr;

        if (ctx->argument()) { 
            val = get_node<ASTNode>(visit(ctx->argument()));
        }
        return std::static_pointer_cast<ASTNode>(std::make_shared<VariableDeclarationNode>(type, name, val));
    }

    virtual antlrcpp::Any visitAssignment(AMSParser::AssignmentContext *ctx) override {
        std::string name = ctx->ID()->getText();
        auto val = get_node<ASTNode>(visit(ctx->argument()));
        return std::static_pointer_cast<ASTNode>(std::make_shared<AssignmentNode>(name, val));
    }

    virtual antlrcpp::Any visitArgument(AMSParser::ArgumentContext *ctx) override {
        std::string text = ctx->getText();
        // Route to VariableNode or LiteralNode
        if (ctx->ID()) {
            return std::static_pointer_cast<ASTNode>(std::make_shared<VariableNode>(text));
        } else {
            return std::static_pointer_cast<ASTNode>(std::make_shared<LiteralNode>(text));
        }
    }

    //##################################### Pass Through Visitors  ####################################
    virtual antlrcpp::Any visitSourceItem(AMSParser::SourceItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitEventItem(AMSParser::EventItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitObserverItem(AMSParser::ObserverItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitFunctionItem(AMSParser::FunctionItemContext *ctx) override { return visit(ctx->statement()); }
};