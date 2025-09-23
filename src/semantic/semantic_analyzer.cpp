#include "semantic_analyzer.h"
#include <algorithm>

namespace SimpleLang {

// Scope implementation
void Scope::define(const Symbol& symbol) {
    symbols[symbol.name] = symbol;
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    
    if (parent) {
        return parent->lookup(name);
    }
    
    return nullptr;
}

Symbol* Scope::lookupLocal(const std::string& name) {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

bool Scope::exists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

bool Scope::existsLocal(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

// SemanticAnalyzer implementation
SemanticAnalyzer::SemanticAnalyzer() 
    : currentFunctionReturnType(Type::VOID), inFunction(false) {
    beginScope(); // Global scope
}

void SemanticAnalyzer::analyze(Program& program) {
    defineBuiltinFunctions();
    program.accept(*this);
}

void SemanticAnalyzer::visitIntegerLiteral(IntegerLiteral& /*node*/) {
    // Type is already set in constructor
}

void SemanticAnalyzer::visitFloatLiteral(FloatLiteral& /*node*/) {
    // Type is already set in constructor
}

void SemanticAnalyzer::visitStringLiteral(StringLiteral& /*node*/) {
    // Type is already set in constructor
}

void SemanticAnalyzer::visitBooleanLiteral(BooleanLiteral& /*node*/) {
    // Type is already set in constructor
}

void SemanticAnalyzer::visitIdentifier(Identifier& node) {
    Symbol* symbol = lookupSymbol(node.name);
    if (!symbol) {
        addError(node.position, "Undefined variable '" + node.name + "'");
        node.type = TypeInfo(Type::UNKNOWN);
        return;
    }
    
    if (!symbol->isInitialized && !symbol->is_function) {
        addError(node.position, "Variable '" + node.name + "' used before initialization");
    }
    
    node.type = symbol->type;
}

void SemanticAnalyzer::visitBinaryExpression(BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);
    
    node.type = getCompatibleType(node.left->type, node.right->type, node.operator_);
    
    if (node.type.type == Type::UNKNOWN) {
        addError(node.position, "Type mismatch in binary expression");
    }
}

void SemanticAnalyzer::visitUnaryExpression(UnaryExpression& node) {
    node.operand->accept(*this);
    
    node.type = getUnaryResultType(node.operand->type, node.operator_);
    
    if (node.type.type == Type::UNKNOWN) {
        addError(node.position, "Invalid unary operation");
    }
}

void SemanticAnalyzer::visitFunctionCall(FunctionCall& node) {
    Symbol* symbol = lookupSymbol(node.functionName);
    if (!symbol) {
        addError(node.position, "Undefined function '" + node.functionName + "'");
        node.type = TypeInfo(Type::UNKNOWN);
        return;
    }
    
    if (!symbol->is_function) {
        addError(node.position, "'" + node.functionName + "' is not a function");
        node.type = TypeInfo(Type::UNKNOWN);
        return;
    }
    
    // Check argument count
    if (node.arguments.size() != symbol->parameterTypes.size()) {
        addError(node.position, "Function '" + node.functionName + "' expects " +
                 std::to_string(symbol->parameterTypes.size()) + " arguments, got " +
                 std::to_string(node.arguments.size()));
    }
    
    // Check argument types
    for (size_t i = 0; i < std::min(node.arguments.size(), symbol->parameterTypes.size()); i++) {
        node.arguments[i]->accept(*this);
        
        if (!canConvert(node.arguments[i]->type, symbol->parameterTypes[i])) {
            addError(node.arguments[i]->position,
                     "Argument " + std::to_string(i + 1) + " type mismatch. Expected " +
                     symbol->parameterTypes[i].name + ", got " + node.arguments[i]->type.name);
        }
    }
    
    node.type = symbol->type;
}

void SemanticAnalyzer::visitExpressionStatement(ExpressionStatement& node) {
    node.expression->accept(*this);
}

void SemanticAnalyzer::visitVariableDeclaration(VariableDeclaration& node) {
    // Check if variable already exists in current scope
    if (currentScope().existsLocal(node.name)) {
        addError(node.position, "Variable '" + node.name + "' already declared in this scope");
        return;
    }
    
    TypeInfo declaredType = node.type;
    
    if (node.initializer) {
        node.initializer->accept(*this);
        
        // Type inference if no explicit type
        if (declaredType.type == Type::UNKNOWN) {
            declaredType = node.initializer->type;
            node.type = declaredType;
        }
        
        // Type checking
        if (!canConvert(node.initializer->type, declaredType)) {
            addError(node.position, "Cannot convert " + node.initializer->type.name +
                     " to " + declaredType.name);
        }
    } else if (declaredType.type == Type::UNKNOWN) {
        addError(node.position, "Variable '" + node.name + "' must have explicit type or initializer");
        return;
    }
    
    Symbol symbol(node.name, declaredType, node.position);
    symbol.isInitialized = (node.initializer != nullptr);
    defineSymbol(symbol);
}

void SemanticAnalyzer::visitAssignmentStatement(AssignmentStatement& node) {
    Symbol* symbol = lookupSymbol(node.variableName);
    if (!symbol) {
        addError(node.position, "Undefined variable '" + node.variableName + "'");
        return;
    }
    
    if (symbol->is_function) {
        addError(node.position, "Cannot assign to function '" + node.variableName + "'");
        return;
    }
    
    node.value->accept(*this);
    
    if (!canConvert(node.value->type, symbol->type)) {
        addError(node.position, "Cannot convert " + node.value->type.name +
                 " to " + symbol->type.name);
    }
    
    symbol->isInitialized = true;
}

void SemanticAnalyzer::visitBlockStatement(BlockStatement& node) {
    beginScope();
    
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    endScope();
}

void SemanticAnalyzer::visitIfStatement(IfStatement& node) {
    node.condition->accept(*this);
    
    if (node.condition->type.type != Type::BOOL) {
        addError(node.condition->position, "If condition must be boolean");
    }
    
    node.thenStatement->accept(*this);
    
    if (node.elseStatement) {
        node.elseStatement->accept(*this);
    }
}

void SemanticAnalyzer::visitWhileStatement(WhileStatement& node) {
    node.condition->accept(*this);
    
    if (node.condition->type.type != Type::BOOL) {
        addError(node.condition->position, "While condition must be boolean");
    }
    
    node.body->accept(*this);
}

void SemanticAnalyzer::visitForStatement(ForStatement& node) {
    beginScope(); // For loop creates its own scope
    
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    
    if (node.condition) {
        node.condition->accept(*this);
        if (node.condition->type.type != Type::BOOL) {
            addError(node.condition->position, "For condition must be boolean");
        }
    }
    
    if (node.increment) {
        node.increment->accept(*this);
    }
    
    node.body->accept(*this);
    
    endScope();
}

void SemanticAnalyzer::visitReturnStatement(ReturnStatement& node) {
    if (!inFunction) {
        addError(node.position, "Return statement outside function");
        return;
    }
    
    if (node.value) {
        node.value->accept(*this);
        
        if (!canConvert(node.value->type, currentFunctionReturnType)) {
            addError(node.position, "Return type mismatch. Expected " +
                     currentFunctionReturnType.name + ", got " + node.value->type.name);
        }
    } else {
        if (currentFunctionReturnType.type != Type::VOID) {
            addError(node.position, "Function must return a value of type " +
                     currentFunctionReturnType.name);
        }
    }
}

void SemanticAnalyzer::visitFunctionDeclaration(FunctionDeclaration& node) {
    // Check if function already exists
    if (currentScope().existsLocal(node.name)) {
        addError(node.position, "Function '" + node.name + "' already declared");
        return;
    }
    
    // Create function symbol
    Symbol functionSymbol(node.name, node.returnType, node.position, true);
    for (const auto& param : node.parameters) {
        functionSymbol.parameterTypes.push_back(param.type);
    }
    functionSymbol.isInitialized = true;
    defineSymbol(functionSymbol);
    
    // Analyze function body
    beginScope();
    
    // Add parameters to function scope
    for (const auto& param : node.parameters) {
        Symbol paramSymbol(param.name, param.type, param.position);
        paramSymbol.isInitialized = true;
        defineSymbol(paramSymbol);
    }
    
    TypeInfo previousReturnType = currentFunctionReturnType;
    bool previousInFunction = inFunction;
    
    currentFunctionReturnType = node.returnType;
    inFunction = true;
    
    node.body->accept(*this);
    
    currentFunctionReturnType = previousReturnType;
    inFunction = previousInFunction;
    
    endScope();
}

void SemanticAnalyzer::visitProgram(Program& node) {
    // First pass: declare all functions
    for (auto& func : node.functions) {
        if (currentScope().existsLocal(func->name)) {
            addError(func->position, "Function '" + func->name + "' already declared");
            continue;
        }
        
        Symbol functionSymbol(func->name, func->returnType, func->position, true);
        for (const auto& param : func->parameters) {
            functionSymbol.parameterTypes.push_back(param.type);
        }
        functionSymbol.isInitialized = true;
        defineSymbol(functionSymbol);
    }
    
    // Declare global variables
    for (auto& var : node.globalVariables) {
        var->accept(*this);
    }
    
    // Second pass: analyze function bodies
    for (auto& func : node.functions) {
        visitFunctionDeclaration(*func);
    }
    
    // Check for main function
    Symbol* mainFunc = lookupSymbol("main");
    if (!mainFunc) {
        addError(Position(), "No main function found");
    } else if (!mainFunc->is_function) {
        addError(Position(), "'main' is not a function");
    } else if (mainFunc->type.type != Type::INT && mainFunc->type.type != Type::VOID) {
        addError(Position(), "Main function must return int or void");
    }
}

// Private methods
void SemanticAnalyzer::beginScope() {
    scopes.emplace_back(scopes.empty() ? nullptr : &scopes.back());
}

void SemanticAnalyzer::endScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

Scope& SemanticAnalyzer::currentScope() {
    return scopes.back();
}

void SemanticAnalyzer::defineSymbol(const Symbol& symbol) {
    currentScope().define(symbol);
}

Symbol* SemanticAnalyzer::lookupSymbol(const std::string& name) {
    return currentScope().lookup(name);
}

TypeInfo SemanticAnalyzer::getCompatibleType(const TypeInfo& left, const TypeInfo& right, BinaryOperator op) {
    if (isLogicalOperation(op)) {
        if (left.type == Type::BOOL && right.type == Type::BOOL) {
            return TypeInfo(Type::BOOL);
        }
        return TypeInfo(Type::UNKNOWN);
    }
    
    if (isComparisonOperation(op)) {
        if (left.isCompatible(right)) {
            return TypeInfo(Type::BOOL);
        }
        return TypeInfo(Type::UNKNOWN);
    }
    
    if (isNumericOperation(op)) {
        if (left.isNumeric() && right.isNumeric()) {
            // Float takes precedence
            if (left.type == Type::FLOAT || right.type == Type::FLOAT) {
                return TypeInfo(Type::FLOAT);
            }
            return TypeInfo(Type::INT);
        }
        
        // String concatenation
        if (op == BinaryOperator::ADD && 
            (left.type == Type::STRING || right.type == Type::STRING)) {
            return TypeInfo(Type::STRING);
        }
        
        return TypeInfo(Type::UNKNOWN);
    }
    
    return TypeInfo(Type::UNKNOWN);
}

TypeInfo SemanticAnalyzer::getUnaryResultType(const TypeInfo& operand, UnaryOperator op) {
    switch (op) {
        case UnaryOperator::MINUS:
        case UnaryOperator::PLUS:
            return operand.isNumeric() ? operand : TypeInfo(Type::UNKNOWN);
        case UnaryOperator::NOT:
            return (operand.type == Type::BOOL) ? TypeInfo(Type::BOOL) : TypeInfo(Type::UNKNOWN);
        default:
            return TypeInfo(Type::UNKNOWN);
    }
}

bool SemanticAnalyzer::isNumericOperation(BinaryOperator op) {
    return op == BinaryOperator::ADD || op == BinaryOperator::SUBTRACT ||
           op == BinaryOperator::MULTIPLY || op == BinaryOperator::DIVIDE ||
           op == BinaryOperator::MODULO;
}

bool SemanticAnalyzer::isComparisonOperation(BinaryOperator op) {
    return op == BinaryOperator::EQUAL || op == BinaryOperator::NOT_EQUAL ||
           op == BinaryOperator::LESS_THAN || op == BinaryOperator::LESS_EQUAL ||
           op == BinaryOperator::GREATER_THAN || op == BinaryOperator::GREATER_EQUAL;
}

bool SemanticAnalyzer::isLogicalOperation(BinaryOperator op) {
    return op == BinaryOperator::AND || op == BinaryOperator::OR;
}

bool SemanticAnalyzer::canConvert(const TypeInfo& from, const TypeInfo& to) {
    if (from.type == to.type) return true;
    
    // Numeric conversions
    if (from.isNumeric() && to.isNumeric()) return true;
    
    return false;
}

void SemanticAnalyzer::addError(const Position& position, const std::string& message) {
    errors.emplace_back(position, message);
}

void SemanticAnalyzer::defineBuiltinFunctions() {
    // print function: print(string) -> void
    Symbol printFunc("print", TypeInfo(Type::VOID), Position(), true);
    printFunc.parameterTypes.push_back(TypeInfo(Type::STRING));
    printFunc.isInitialized = true;
    defineSymbol(printFunc);
    
    // string conversion functions
    Symbol toStringInt("string", TypeInfo(Type::STRING), Position(), true);
    toStringInt.parameterTypes.push_back(TypeInfo(Type::INT));
    toStringInt.isInitialized = true;
    defineSymbol(toStringInt);
}

} // namespace SimpleLang