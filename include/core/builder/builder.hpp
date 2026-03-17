#pragma once
#include "AMSBaseVisitor.h"
#include "AST.hpp"

class Builder : public AMSBaseVisitor {
public:
    virtual antlrcpp::Any visitProgram(AMSParser::ProgramContext *ctx) override {
        auto program = std::make_shared<ProgramNode>();
        
        // 1. Parse Sources Block (if it exists)
        if (ctx->sourcesBlock()) {
            for (auto defCtx : ctx->sourcesBlock()->sourceDefination()) {
                auto result = visit(defCtx);
                if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                    program->statements.push_back(*nodePtr);
                }
            }
        }

        // 2. Parse Events Block (if it exists)
        if (ctx->eventsBlock()) {
            for (auto defCtx : ctx->eventsBlock()->eventDefination()) {
                auto result = visit(defCtx);
                if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                    program->statements.push_back(*nodePtr);
                }
            }
        }
        
        return program;
    }

    // SOURCES BLOCK LOGIC

    virtual antlrcpp::Any visitSourceDefination(AMSParser::SourceDefinationContext *ctx) override {
        std::string name = ctx->ID()->getText();
       
        auto sourceNode = std::make_shared<SourceDefNode>(name); 

        for (auto itemCtx : ctx->sourceItem()) {
            auto result = visit(itemCtx);
            if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                sourceNode->items.push_back(*nodePtr);
            }
        }
        return std::shared_ptr<ASTNode>(sourceNode);
    }

    virtual antlrcpp::Any visitSourceItem(AMSParser::SourceItemContext *ctx) override {
        std::string type = ctx->ID(0)->getText();
        std::string name = ctx->ID(1)->getText();
        std::string url = ctx->STRING()->getText();

        if (url.length() >= 2 && url.front() == '"' && url.back() == '"') {
            url = url.substr(1, url.length() - 2);
        }

        std::shared_ptr<ASTNode> node = std::make_shared<SourceItemNode>(type, name, url);
        return node;
    }

    // EVENTS BLOCK LOGIC

    virtual antlrcpp::Any visitEventDefination(AMSParser::EventDefinationContext *ctx) override {
        
        std::string name = ctx->ID()->getText();
        auto eventNode = std::make_shared<EventNode>(name);

        // Loop through all eventItems (CONSOLE prints) inside the event
        for (auto stmtCtx : ctx->eventItem()) {
            auto result = visit(stmtCtx);
            if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                eventNode->statements.push_back(*nodePtr);
            }
        }

        return std::shared_ptr<ASTNode>(eventNode);
    }

    virtual antlrcpp::Any visitEventItem(AMSParser::EventItemContext *ctx) override {

        std::string text = ctx->STRING()->getText();
        
        if (text.length() >= 2 && text.front() == '"' && text.back() == '"') {
            text = text.substr(1, text.length() - 2);
        }
        
        std::shared_ptr<ASTNode> node = std::make_shared<PrintNode>(text);
        return node;
    }
};