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
        std::shared_ptr<ASTNode> sched = nullptr;
        if (ctx->sourceScheduleStatement()) {
            sched = get_node<ASTNode>(visit(ctx->sourceScheduleStatement()));
        }
        auto node = std::make_shared<SourceDefinitionNode>(ctx->ID()->getText(), sched);
        for (auto* item : ctx->sourceItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    virtual antlrcpp::Any visitSourceScheduleStatement(AMSParser::SourceScheduleStatementContext *ctx) override {
        if (ctx->AT()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::AT, val, ""));
        }
        if (ctx->EVERY()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::EVERY, val, ""));
        }
        if (ctx->CRON()) {
            std::string cron = ctx->STRING_L()->getText();
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::CRON, cron.substr(1, cron.length()-2), ""));
        }
        return antlrcpp::Any();
    }

    //##################################### Events Definitions  #######################################
    virtual antlrcpp::Any visitEventDefinition(AMSParser::EventDefinitionContext *ctx) override {
        std::shared_ptr<ASTNode> sched = nullptr;
        if (ctx->eventScheduleStatement()) {
            sched = get_node<ASTNode>(visit(ctx->eventScheduleStatement()));
        }
        auto node = std::make_shared<EventDefinitionNode>(ctx->ID()->getText(), sched);
        for (auto* item : ctx->eventItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    virtual antlrcpp::Any visitEventScheduleStatement(AMSParser::EventScheduleStatementContext *ctx) override {
        if (ctx->AT()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::AT, val, ""));
        }
        if (ctx->EVERY()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::EVERY, val, ""));
        }
        if (ctx->CRON()) {
            std::string cron = ctx->STRING_L()->getText();
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::CRON, cron.substr(1, cron.length()-2), ""));
        }
        return antlrcpp::Any();
    }

    //#################################### Observers Definitions  #####################################
    virtual antlrcpp::Any visitObserverDefinition(AMSParser::ObserverDefinitionContext *ctx) override {
        std::shared_ptr<ASTNode> sched = nullptr;
        if (ctx->observerScheduleStatement()) {
            sched = get_node<ASTNode>(visit(ctx->observerScheduleStatement()));
        }
        auto node = std::make_shared<ObserverDefinitionNode>(ctx->ID()->getText(), sched);
        for (auto* item : ctx->observerItem()) {
            if (auto ptr = get_node<ASTNode>(visit(item))) node->statements.push_back(ptr);
        }
        return std::static_pointer_cast<ASTNode>(node);
    }

    virtual antlrcpp::Any visitObserverScheduleStatement(AMSParser::ObserverScheduleStatementContext *ctx) override {
        if (ctx->AT()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::AT, val, ""));
        }
        if (ctx->EVERY()) {
            auto expr = get_node<LiteralNode>(visit(ctx->scheduleExpr()));
            std::string val = expr ? expr->value : "";
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::EVERY, val, ""));
        }
        if (ctx->CRON()) {
            std::string cron = ctx->STRING_L()->getText();
            return std::static_pointer_cast<ASTNode>(std::make_shared<ScheduleNode>(ScheduleType::CRON, cron.substr(1, cron.length()-2), ""));
        }
        return antlrcpp::Any();
    }

    virtual antlrcpp::Any visitScheduleExpr(AMSParser::ScheduleExprContext *ctx) override {
        if (ctx->INT_L()) {
            std::string unit = "";
            if (ctx->MINUTES()) unit = "MINUTES";
            else if (ctx->HOURS()) unit = "HOURS";
            else if (ctx->DAYS()) unit = "DAYS";
            else if (ctx->WEEKS()) unit = "WEEKS";
            else if (ctx->SECONDS()) unit = "SECONDS";
            return std::static_pointer_cast<ASTNode>(std::make_shared<LiteralNode>(ctx->INT_L()->getText() + " " + unit));
        }
        if (ctx->STRING_L()) {
            return std::static_pointer_cast<ASTNode>(std::make_shared<LiteralNode>(ctx->STRING_L()->getText()));
        }
        return antlrcpp::Any();
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
            for (auto* exprCtx : ctx->arguments()->expression()) {
                if (auto argNode = get_node<ASTNode>(visit(exprCtx))) {
                    args.push_back(argNode);
                }
            }
        }
        return std::static_pointer_cast<ASTNode>(std::make_shared<FunctionCallNode>(name, args));
    }

    virtual antlrcpp::Any visitExpression(AMSParser::ExpressionContext *ctx) override {
        // 1. Parentheses
        if (ctx->LPAREN()) {
            return visit(ctx->expression(0));
        }

        // 2. Function Call
        if (ctx->functionCall()) {
            return visit(ctx->functionCall());
        }

        // 3. Unary Operator
        if (ctx->expression().size() == 1 && ctx->op) {
            std::string op = ctx->op->getText();
            auto right = get_node<ASTNode>(visit(ctx->expression(0)));
            return std::static_pointer_cast<ASTNode>(std::make_shared<UnaryOperatorNode>(op, right));
        }

        // 4. Binary Operator
        if (ctx->expression().size() == 2) {
            auto left = get_node<ASTNode>(visit(ctx->expression(0)));
            std::string op = ctx->op->getText();
            auto right = get_node<ASTNode>(visit(ctx->expression(1)));
            return std::static_pointer_cast<ASTNode>(std::make_shared<BinaryOperatorNode>(left, op, right));
        }

        // 5. Variables - FIXED CASTING
        if (ctx->ID()) {
            auto node = std::make_shared<VariableNode>(ctx->getText());
            return std::static_pointer_cast<ASTNode>(node); // <--- Use static_pointer_cast
        }

        // 6. Literals - FIXED CASTING
        auto litNode = std::make_shared<LiteralNode>(ctx->getText());
        return std::static_pointer_cast<ASTNode>(litNode); // <--- Use static_pointer_cast
    }
    //########################################## Statements & Variables #################################
    virtual antlrcpp::Any visitStatement(AMSParser::StatementContext *ctx) override {
        if (ctx->functionCall()) return visit(ctx->functionCall());
        if (ctx->variableDeclaration()) return visit(ctx->variableDeclaration());
        if (ctx->assignment()) return visit(ctx->assignment());
        if (ctx->ifStatement()) return visit(ctx->ifStatement());
        if (ctx->whileStatement()) return visit(ctx->whileStatement());
        if (ctx->forStatement()) return visit(ctx->forStatement());
        if (ctx->returnStatement()) return visit(ctx->returnStatement());
        return antlrcpp::Any();
    }

    virtual antlrcpp::Any visitVariableDeclaration(AMSParser::VariableDeclarationContext *ctx) override {
        std::string type = ctx->dataType()->getText();
        std::string name = ctx->ID()->getText();
        std::shared_ptr<ASTNode> val = nullptr;

        if (ctx->EQUAL()) {
            val = get_node<ASTNode>(visit(ctx->expression()));
        }
        return std::static_pointer_cast<ASTNode>(std::make_shared<VariableDeclarationNode>(type, name, val));
    }

    virtual antlrcpp::Any visitAssignment(AMSParser::AssignmentContext *ctx) override {
        std::string name = ctx->ID()->getText();
        auto val = get_node<ASTNode>(visit(ctx->expression()));
        return std::static_pointer_cast<ASTNode>(std::make_shared<AssignmentNode>(name, val));
    }

    //##################################### Control Flow Statements  #################################
    virtual antlrcpp::Any visitIfStatement(AMSParser::IfStatementContext *ctx) override {
        auto condition = get_node<ASTNode>(visit(ctx->expression()));
        return std::static_pointer_cast<ASTNode>(std::make_shared<IfStatementNode>(condition, std::vector<std::shared_ptr<ASTNode>>(), std::vector<std::shared_ptr<ASTNode>>()));
    }

    virtual antlrcpp::Any visitWhileStatement(AMSParser::WhileStatementContext *ctx) override {
        auto condition = get_node<ASTNode>(visit(ctx->expression()));
        return std::static_pointer_cast<ASTNode>(std::make_shared<WhileStatementNode>(condition, std::vector<std::shared_ptr<ASTNode>>()));
    }

    virtual antlrcpp::Any visitForStatement(AMSParser::ForStatementContext *ctx) override {
        auto init = get_node<ASTNode>(visit(ctx->variableDeclaration()));
        auto cond = get_node<ASTNode>(visit(ctx->expression()));
        auto inc = get_node<ASTNode>(visit(ctx->assignment()));
        return std::static_pointer_cast<ASTNode>(std::make_shared<ForStatementNode>(init, cond, inc, std::vector<std::shared_ptr<ASTNode>>()));
    }

    virtual antlrcpp::Any visitReturnStatement(AMSParser::ReturnStatementContext *ctx) override {
        std::shared_ptr<ASTNode> retVal = nullptr;
        if (ctx->expression()) {
            retVal = get_node<ASTNode>(visit(ctx->expression()));
        }
        return std::static_pointer_cast<ASTNode>(std::make_shared<ReturnStatementNode>(retVal));
    }

    //##################################### Pass Through Visitors  ####################################
    virtual antlrcpp::Any visitSourceItem(AMSParser::SourceItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitEventItem(AMSParser::EventItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitObserverItem(AMSParser::ObserverItemContext *ctx) override { return visit(ctx->statement()); }
    virtual antlrcpp::Any visitFunctionItem(AMSParser::FunctionItemContext *ctx) override { return visit(ctx->statement()); }
};