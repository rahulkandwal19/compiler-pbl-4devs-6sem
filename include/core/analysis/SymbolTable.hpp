#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

enum class SymbolType {
    INT,
    FLOAT,
    STRING,
    BOOL,
    VOID,
    FUNCTION,
    ARRAY,
    STRUCT,
    UNKNOWN
};

struct Symbol {
    std::string name;
    SymbolType type;
    std::string scope;
    bool isInitialized;

    Symbol(std::string n, SymbolType t, std::string s = "global")
        : name(std::move(n)), type(t), scope(std::move(s)), isInitialized(false) {}
};

class SymbolTable {
public:
    void enterScope(const std::string& scopeName);
    void exitScope();
    
    bool addSymbol(const std::string& name, SymbolType type);
    bool addSymbol(const std::string& name, SymbolType type, const std::string& scope);
    
    Symbol* lookup(const std::string& name);
    Symbol* lookupCurrentScope(const std::string& name);
    
    bool exists(const std::string& name);
    bool existsInCurrentScope(const std::string& name);
    
    void setInitialized(const std::string& name);
    SymbolType getType(const std::string& name);
    
    std::string getCurrentScope() const { return currentScope; }
    
    void clear();
    
    std::vector<std::string> getAllSymbols();

private:
    std::unordered_map<std::string, std::vector<Symbol>> symbols;
    std::string currentScope = "global";
    std::vector<std::string> scopeStack = {"global"};
};

SymbolType stringToSymbolType(const std::string& typeStr);
std::string symbolTypeToString(SymbolType type);
bool areTypesCompatible(SymbolType left, SymbolType right, bool allowImplicit = true);
