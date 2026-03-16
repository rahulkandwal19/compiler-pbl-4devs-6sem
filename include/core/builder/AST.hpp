#pragma once
#include <string>
#include <vector>
#include <memory>

// Forward declarations for the Visitor
class ASTNode;
class ProgramNode;
class WatchNode;
class PrintNode;
class EventNode;

// The pure virtual Visitor interface for your backends
class ASTVisitor {
public:
    virtual void visit(ProgramNode* node) = 0;
    virtual void visit(WatchNode* node) = 0;
    virtual void visit(PrintNode* node) = 0;
    virtual void visit(EventNode* node) = 0;
};

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    // Every node must accept a visitor
    virtual void accept(ASTVisitor* visitor) = 0; 
};


//Node Implementations

class WatchNode : public ASTNode {
public:
    std::string resourceType;
    std::string variableName;
    std::string url;

    WatchNode(std::string type, std::string name, std::string u) 
        : resourceType(type), variableName(name), url(u) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class EventNode : public ASTNode {
public:
    std::string eventName;
    std::vector<std::shared_ptr<ASTNode>> statements;

    EventNode(std::string name) : eventName(name) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class PrintNode : public ASTNode {
public:
    std::string text;
    PrintNode(std::string t) : text(t) {}

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};