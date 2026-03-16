#pragma once
#include "AMSBaseVisitor.h"
#include "AST.hpp"

class Builder : public AMSBaseVisitor {
public:
    virtual antlrcpp::Any visitProgram(AMSParser::ProgramContext *ctx) override {
        auto program = std::make_shared<ProgramNode>();
        
        for (auto child : ctx->children) {
            auto result = visit(child);
            
            if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                program->statements.push_back(*nodePtr);
            }
        }
        return program;
    }

    virtual antlrcpp::Any visitWatchItem(AMSParser::WatchItemContext *ctx) override {
        std::string type = ctx->ID(0)->getText();
        std::string name = ctx->ID(1)->getText();
        std::string url = ctx->STRING()->getText();
        
        std::shared_ptr<ASTNode> node = std::make_shared<WatchNode>(type, name, url);
        return node;
    }

    virtual antlrcpp::Any visitStatement(AMSParser::StatementContext *ctx) override {
        std::string text = ctx->STRING()->getText();
        // Remove quotes if present
        if (text.length() >= 2 && text.front() == '"' && text.back() == '"') {
            text = text.substr(1, text.length() - 2);
        }
        
        std::shared_ptr<ASTNode> node = std::make_shared<PrintNode>(text);
        return node;
    }

    virtual antlrcpp::Any visitEventDef(AMSParser::EventDefContext *ctx) override {
        // 1. Get the event name
        std::string name = ctx->ID()->getText();
        auto eventNode = std::make_shared<EventNode>(name);

        // 2. Loop through all statements INSIDE the event
        for (auto stmtCtx : ctx->statement()) {
            auto result = visit(stmtCtx);
            if (auto* nodePtr = std::any_cast<std::shared_ptr<ASTNode>>(&result)) {
                eventNode->statements.push_back(*nodePtr);
            }
        }

        // 3. Return the fully built event node!
        return std::shared_ptr<ASTNode>(eventNode);
    }
};