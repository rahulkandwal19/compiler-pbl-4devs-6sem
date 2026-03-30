#include "core/analysis/SemanticAnalyzer.hpp"

static const std::vector<std::string> BUILTIN_FUNCTIONS = {
    "PRINT", "PRINTLN", "READ", "READLN",
    "LENGTH", "SUBSTRING", "CONCAT", "TOSTRING",
    "TOINT", "TOFLOAT", "TOSTRING_FLOAT",
    "RAND", "SQRT", "ABS", "MIN", "MAX",
    "ROUND", "FLOOR", "CEIL", "POW", "LOG", "LOG10",
    "SIN", "COS", "TAN",
    "STRLOWER", "STRUPPER", "TRIM", "REPLACE", "FIND",
    "FILEOPEN", "FILECLOSE", "FILEREAD", "FILEWRITE", "FILEEXISTS"
};

static bool isBuiltinFunction(const std::string& name) {
    for (const auto& fn : BUILTIN_FUNCTIONS) {
        if (name == fn) return true;
    }
    return false;
}

SemanticAnalyzer::SemanticAnalyzer() : errorCount(0), currentFunctionReturnType("VOID") {}

bool SemanticAnalyzer::analyze(std::shared_ptr<ProgramNode> program) {
    if (!program) return false;
    
    try {
        program->accept(this);
    } catch (...) {
        reportError("Exception during semantic analysis");
    }
    
    return errorCount == 0;
}

void SemanticAnalyzer::reportError(const std::string& msg) {
    errors.push_back("[ERROR] " + msg);
    errorCount++;
    std::cerr << "[SEMANTIC ERROR] " << msg << std::endl;
}

void SemanticAnalyzer::reportWarning(const std::string& msg) {
    std::cout << "[SEMANTIC WARNING] " << msg << std::endl;
}

void SemanticAnalyzer::visit(ProgramNode* node) {
    for (const auto& block : node->programBlocks) {
        block->accept(this);
    }
}

void SemanticAnalyzer::visit(GlobalSectionNode* node) {
    symbolTable.enterScope("global");
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ImportNode* node) {}
void SemanticAnalyzer::visit(MergeNode* node) {}

void SemanticAnalyzer::visit(SourceDefinitionNode* node) {
    symbolTable.enterScope("source:" + node->sourceName);
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(EventDefinitionNode* node) {
    symbolTable.enterScope("event:" + node->eventName);
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ObserverDefinitionNode* node) {
    symbolTable.enterScope("observer:" + node->observerName);
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(FunctionDefinitionNode* node) {
    std::string funcName = node->functionName;
    if (!symbolTable.addSymbol(funcName, SymbolType::FUNCTION, "global")) {
        reportError("Function '" + funcName + "' already declared");
    }
    
    symbolTable.enterScope("function:" + funcName);
    
    for (const auto& param : node->parameters) {
        symbolTable.addSymbol(param, SymbolType::UNKNOWN);
    }
    
    currentFunctionReturnType = "VOID";
    
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(FunctionCallNode* node) {
    std::string funcName = node->functionName;
    if (!symbolTable.exists(funcName) && !isBuiltinFunction(funcName)) {
        reportError("Call to undeclared function: " + funcName);
    }
}

SymbolType SemanticAnalyzer::getExpressionType(std::shared_ptr<ASTNode> node) {
    if (!node) return SymbolType::UNKNOWN;
    
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(node)) {
        std::string val = lit->value;
        if (val == "TRUE" || val == "FALSE") return SymbolType::BOOL;
        if (val.find('.') != std::string::npos) return SymbolType::FLOAT;
        if (val.front() == '"' && val.back() == '"') return SymbolType::STRING;
        try {
            std::stoi(val);
            return SymbolType::INT;
        } catch (...) {
            return SymbolType::UNKNOWN;
        }
    }
    
    if (auto var = std::dynamic_pointer_cast<VariableNode>(node)) {
        if (!symbolTable.exists(var->name)) {
            reportError("Use of undeclared variable: " + var->name);
            return SymbolType::UNKNOWN;
        }
        return symbolTable.getType(var->name);
    }
    
    if (auto binOp = std::dynamic_pointer_cast<BinaryOperatorNode>(node)) {
        SymbolType leftType = getExpressionType(binOp->left);
        SymbolType rightType = getExpressionType(binOp->right);
        
        std::string op = binOp->op;
        if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op <= "<=" ||
            op == "AND" || op == "OR" || op == "&" || op == "|" || op == "GREATER THAN" || op == "LESS THAN") {
            return SymbolType::BOOL;
        }
        
        if (leftType == SymbolType::STRING || rightType == SymbolType::STRING) {
            return SymbolType::STRING;
        }
        
        if (leftType == SymbolType::FLOAT || rightType == SymbolType::FLOAT) {
            return SymbolType::FLOAT;
        }
        
        return SymbolType::INT;
    }
    
    if (auto unOp = std::dynamic_pointer_cast<UnaryOperatorNode>(node)) {
        if (unOp->op == "NOT" || unOp->op == "!") {
            return SymbolType::BOOL;
        }
        return getExpressionType(unOp->right);
    }
    
    return SymbolType::UNKNOWN;
}

void SemanticAnalyzer::visit(LiteralNode* node) {}
void SemanticAnalyzer::visit(VariableNode* node) {
    if (!symbolTable.exists(node->name)) {
        reportError("Use of undeclared variable: " + node->name);
    }
}

void SemanticAnalyzer::visit(VariableDeclarationNode* node) {
    std::string varName = node->varName;
    SymbolType varType = stringToSymbolType(node->dataType);
    
    if (symbolTable.existsInCurrentScope(varName)) {
        reportError("Variable '" + varName + "' already declared in this scope");
        return;
    }
    
    symbolTable.addSymbol(varName, varType);
    
    if (node->value) {
        SymbolType initType = getExpressionType(node->value);
        if (!areTypesCompatible(varType, initType)) {
            reportError("Type mismatch: cannot initialize " + node->dataType + " with incompatible type");
        }
        symbolTable.setInitialized(varName);
    }
}

void SemanticAnalyzer::visit(AssignmentNode* node) {
    std::string varName = node->varName;
    
    if (!symbolTable.exists(varName)) {
        reportError("Assignment to undeclared variable: " + varName);
        return;
    }
    
    SymbolType varType = symbolTable.getType(varName);
    SymbolType valueType = getExpressionType(node->expression);
    
    if (!areTypesCompatible(varType, valueType)) {
        reportError("Type mismatch in assignment to '" + varName + "'");
    }
    
    symbolTable.setInitialized(varName);
}

void SemanticAnalyzer::visit(UnaryOperatorNode* node) {}
void SemanticAnalyzer::visit(BinaryOperatorNode* node) {}

void SemanticAnalyzer::visit(IfStatementNode* node) {
    if (node->condition) {
        SymbolType condType = getExpressionType(node->condition);
        if (condType != SymbolType::BOOL) {
            reportWarning("Condition in IF statement should be BOOL type");
        }
    }
    
    symbolTable.enterScope("if");
    for (const auto& stmt : node->ifBody) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
    
    if (!node->elseBody.empty()) {
        symbolTable.enterScope("else");
        for (const auto& stmt : node->elseBody) {
            stmt->accept(this);
        }
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::visit(WhileStatementNode* node) {
    if (node->condition) {
        SymbolType condType = getExpressionType(node->condition);
        if (condType != SymbolType::BOOL) {
            reportWarning("Condition in WHILE statement should be BOOL type");
        }
    }
    
    symbolTable.enterScope("while");
    for (const auto& stmt : node->body) {
        stmt->accept(this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ForStatementNode* node) {
    symbolTable.enterScope("for");
    
    if (node->initialization) {
        node->initialization->accept(this);
    }
    
    if (node->condition) {
        SymbolType condType = getExpressionType(node->condition);
        if (condType != SymbolType::BOOL) {
            reportWarning("Condition in FOR statement should be BOOL type");
        }
    }
    
    if (node->increment) {
        node->increment->accept(this);
    }
    
    for (const auto& stmt : node->body) {
        stmt->accept(this);
    }
    
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ReturnStatementNode* node) {
    if (node->returnValue) {
        SymbolType retType = getExpressionType(node->returnValue);
        SymbolType expectedType = stringToSymbolType(currentFunctionReturnType);
        
        if (!areTypesCompatible(expectedType, retType)) {
            reportError("Return type mismatch: expected " + currentFunctionReturnType);
        }
    }
}

void printSemanticErrors(const SemanticAnalyzer& analyzer) {
    for (const auto& error : analyzer.getErrors()) {
        std::cerr << error << std::endl;
    }
}
