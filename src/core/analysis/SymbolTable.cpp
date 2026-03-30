#include "core/analysis/SymbolTable.hpp"
#include <algorithm>

void SymbolTable::enterScope(const std::string& scopeName) {
    scopeStack.push_back(scopeName);
    currentScope = scopeName;
}

void SymbolTable::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop_back();
        currentScope = scopeStack.back();
    }
}

bool SymbolTable::addSymbol(const std::string& name, SymbolType type) {
    return addSymbol(name, type, currentScope);
}

bool SymbolTable::addSymbol(const std::string& name, SymbolType type, const std::string& scope) {
    std::string key = name + ":" + scope;
    if (symbols.find(key) != symbols.end()) {
        return false;
    }
    symbols[key].push_back(Symbol(name, type, scope));
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        std::string key = name + ":" + *it;
        auto found = symbols.find(key);
        if (found != symbols.end() && !found->second.empty()) {
            return &found->second[0];
        }
    }
    return nullptr;
}

Symbol* SymbolTable::lookupCurrentScope(const std::string& name) {
    std::string key = name + ":" + currentScope;
    auto found = symbols.find(key);
    if (found != symbols.end() && !found->second.empty()) {
        return &found->second[0];
    }
    return nullptr;
}

bool SymbolTable::exists(const std::string& name) {
    return lookup(name) != nullptr;
}

bool SymbolTable::existsInCurrentScope(const std::string& name) {
    return lookupCurrentScope(name) != nullptr;
}

void SymbolTable::setInitialized(const std::string& name) {
    Symbol* sym = lookup(name);
    if (sym) {
        sym->isInitialized = true;
    }
}

SymbolType SymbolTable::getType(const std::string& name) {
    Symbol* sym = lookup(name);
    if (sym) {
        return sym->type;
    }
    return SymbolType::UNKNOWN;
}

void SymbolTable::clear() {
    symbols.clear();
    scopeStack = {"global"};
    currentScope = "global";
}

std::vector<std::string> SymbolTable::getAllSymbols() {
    std::vector<std::string> result;
    for (const auto& pair : symbols) {
        if (!pair.second.empty()) {
            result.push_back(pair.second[0].name + " (" + symbolTypeToString(pair.second[0].type) + ")");
        }
    }
    return result;
}

SymbolType stringToSymbolType(const std::string& typeStr) {
    std::string upperType = typeStr;
    std::transform(upperType.begin(), upperType.end(), upperType.begin(), ::toupper);
    
    if (upperType == "INT") return SymbolType::INT;
    if (upperType == "FLOAT") return SymbolType::FLOAT;
    if (upperType == "STRING") return SymbolType::STRING;
    if (upperType == "BOOL") return SymbolType::BOOL;
    if (upperType == "VOID") return SymbolType::VOID;
    if (upperType == "FUNCTION") return SymbolType::FUNCTION;
    if (upperType == "ARRAY") return SymbolType::ARRAY;
    if (upperType == "STRUCT") return SymbolType::STRUCT;
    
    return SymbolType::UNKNOWN;
}

std::string symbolTypeToString(SymbolType type) {
    switch (type) {
        case SymbolType::INT: return "INT";
        case SymbolType::FLOAT: return "FLOAT";
        case SymbolType::STRING: return "STRING";
        case SymbolType::BOOL: return "BOOL";
        case SymbolType::VOID: return "VOID";
        case SymbolType::FUNCTION: return "FUNCTION";
        case SymbolType::ARRAY: return "ARRAY";
        case SymbolType::STRUCT: return "STRUCT";
        default: return "UNKNOWN";
    }
}

bool areTypesCompatible(SymbolType left, SymbolType right, bool allowImplicit) {
    if (left == right) return true;
    
    if (allowImplicit) {
        if ((left == SymbolType::INT && right == SymbolType::FLOAT) ||
            (left == SymbolType::FLOAT && right == SymbolType::INT)) {
            return true;
        }
    }
    
    return false;
}
