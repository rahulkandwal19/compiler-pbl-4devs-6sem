#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
//############################# AST Node Declaration  ##################################
class ProgramNode;

class GlobalSectionNode;
class ImportNode;
class MergeNode;

class EventDefinitionNode;

class SourceDefinitionNode; 

class ObserverDefinitionNode;

class FunctionDefinitionNode;
class FunctionCallNode;

class LiteralNode;
class VariableNode;

class VariableDeclarationNode;
class AssignmentNode;

class UnaryOperatorNode;
class BinaryOperatorNode;

class IfStatementNode;
class WhileStatementNode;
class ForStatementNode;
class ReturnStatementNode;
class ScheduleNode;
//############################## Visitor Interfacs #####################################
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ProgramNode* node) = 0;
    
    virtual void visit(GlobalSectionNode* node) = 0;
    virtual void visit(ImportNode* node) = 0;
    virtual void visit(MergeNode* node) = 0;

    virtual void visit(EventDefinitionNode* node) = 0;

    virtual void visit(SourceDefinitionNode* node) = 0;

    virtual void visit(ObserverDefinitionNode* node) = 0;

    virtual void visit(FunctionDefinitionNode* node) = 0;
    virtual void visit(FunctionCallNode* node) = 0;

    virtual void visit(LiteralNode* node) = 0;
    virtual void visit(VariableNode* node) = 0;

    virtual void visit(VariableDeclarationNode* node) = 0;
    virtual void visit(AssignmentNode* node) = 0;

    virtual void visit(UnaryOperatorNode* node) = 0;
    virtual void visit(BinaryOperatorNode* node) = 0;

    virtual void visit(IfStatementNode* node) = 0;
    virtual void visit(WhileStatementNode* node) = 0;
    virtual void visit(ForStatementNode* node) = 0;
    virtual void visit(ReturnStatementNode* node) = 0;
    virtual void visit(ScheduleNode* node) = 0;
};
//############################# AST Node Base Class ####################################
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0; 
};

//############################# Node Implementation ####################################

class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> programBlocks;
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//--------------------------------------------------------------------------------------
class LiteralNode : public ASTNode {
public:
    std::string value;
    LiteralNode(std::string v) : value(std::move(v)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class VariableNode : public ASTNode {
public:
    std::string name;
    VariableNode(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class VariableDeclarationNode : public ASTNode {
public:
    std::string dataType;
    std::string varName;
    std::shared_ptr<ASTNode> value; 

    VariableDeclarationNode(std::string type, std::string name, std::shared_ptr<ASTNode> val = nullptr) 
        : dataType(std::move(type)), varName(std::move(name)), value(std::move(val)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class AssignmentNode : public ASTNode {
public:
    std::string varName;
    std::shared_ptr<ASTNode> expression;

    AssignmentNode(std::string name, std::shared_ptr<ASTNode> expr) 
        : varName(std::move(name)), expression(std::move(expr)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};
//------------------------------- GLOBAL SECTION ---------------------------------------
class GlobalSectionNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ImportNode : public ASTNode {
public:
    std::string moduleName;
    ImportNode(std::string name) : moduleName(std::move(name)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class MergeNode : public ASTNode {
public:
    std::string targetName;
    MergeNode(std::string name) : targetName(std::move(name)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//------------------------------ SOURCES SECTION ---------------------------------------
class SourceDefinitionNode : public ASTNode {
public:
    std::string sourceName;
    std::shared_ptr<ASTNode> schedule;
    std::vector<std::shared_ptr<ASTNode>> statements;
    SourceDefinitionNode(std::string name, std::shared_ptr<ASTNode> sched = nullptr) 
        : sourceName(std::move(name)), schedule(std::move(sched)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//------------------------------ EVENTS SECTION ---------------------------------------
class EventDefinitionNode : public ASTNode {
public:
    std::string eventName;
    std::shared_ptr<ASTNode> schedule;
    std::vector<std::shared_ptr<ASTNode>> statements;
    EventDefinitionNode(std::string name, std::shared_ptr<ASTNode> sched = nullptr) 
        : eventName(std::move(name)), schedule(std::move(sched)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//----------------------------- OBSERVERS SECTION -------------------------------------
class ObserverDefinitionNode : public ASTNode {
public:
    std::string observerName;
    std::shared_ptr<ASTNode> schedule;
    std::vector<std::shared_ptr<ASTNode>> statements;
    ObserverDefinitionNode(std::string name, std::shared_ptr<ASTNode> sched = nullptr) 
        : observerName(std::move(name)), schedule(std::move(sched)) {}
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//----------------------------- FUNCTIONS SECTION -------------------------------------
class FunctionDefinitionNode : public ASTNode {
public:
    std::string functionName;
    
    std::vector<std::string> parameters; 
    std::vector<std::shared_ptr<ASTNode>> statements;

    FunctionDefinitionNode(std::string name, std::vector<std::string> params = {}) 
        : functionName(std::move(name)), parameters(std::move(params)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    // A list of expressions passed to the function
    std::vector<std::shared_ptr<ASTNode>> arguments;

    // Updated Constructor
    FunctionCallNode(std::string name, std::vector<std::shared_ptr<ASTNode>> args = {}) 
        : functionName(std::move(name)), arguments(std::move(args)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//--------------------------- OPERATOR NODES -----------------------------------

class UnaryOperatorNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> right;

    UnaryOperatorNode(std::string o, std::shared_ptr<ASTNode> r) 
        : op(std::move(o)), right(std::move(r)) {}

    void accept(ASTVisitor* visitor) override { 
        visitor->visit(this); 
    }
};

class BinaryOperatorNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;
    std::string op;
    std::shared_ptr<ASTNode> right;

    BinaryOperatorNode(std::shared_ptr<ASTNode> l, std::string o, std::shared_ptr<ASTNode> r) 
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}

    void accept(ASTVisitor* visitor) override { 
        visitor->visit(this); 
    }
};

//------------------------------- CONTROL FLOW NODES -----------------------------------

class IfStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> ifBody;
    std::vector<std::shared_ptr<ASTNode>> elseBody;

    IfStatementNode(std::shared_ptr<ASTNode> cond = nullptr, 
                     std::vector<std::shared_ptr<ASTNode>> ifBlock = {},
                     std::vector<std::shared_ptr<ASTNode>> elseBlock = {})
        : condition(std::move(cond)), ifBody(std::move(ifBlock)), elseBody(std::move(elseBlock)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class WhileStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> body;

    WhileStatementNode(std::shared_ptr<ASTNode> cond = nullptr, std::vector<std::shared_ptr<ASTNode>> b = {})
        : condition(std::move(cond)), body(std::move(b)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ForStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> initialization;
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> increment;
    std::vector<std::shared_ptr<ASTNode>> body;

    ForStatementNode(std::shared_ptr<ASTNode> init = nullptr,
                      std::shared_ptr<ASTNode> cond = nullptr,
                      std::shared_ptr<ASTNode> inc = nullptr,
                      std::vector<std::shared_ptr<ASTNode>> b = {})
        : initialization(std::move(init)), condition(std::move(cond)), 
          increment(std::move(inc)), body(std::move(b)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ReturnStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> returnValue;

    ReturnStatementNode(std::shared_ptr<ASTNode> val = nullptr) : returnValue(std::move(val)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//------------------------------- SCHEDULE NODE -----------------------------------
enum class ScheduleType { AT, EVERY, CRON };

class ScheduleNode : public ASTNode {
public:
    ScheduleType type;
    std::string value;
    std::string unit;

    ScheduleNode(ScheduleType t, std::string val, std::string u = "") 
        : type(t), value(std::move(val)), unit(std::move(u)) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

//--------------------------------------------------------------------------------------

