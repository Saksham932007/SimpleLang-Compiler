#pragma once

#include "../ast/ast_nodes.h"
#include "../ast/ast_visitor.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace SimpleLang {

// Symbol table entry
struct Symbol {
    std::string name;
    TypeInfo type;
    Position position;
    bool is_function;
    bool isInitialized;
    std::vector<TypeInfo> parameterTypes;
    
    // Default constructor
    Symbol() : name(""), type(TypeInfo(Type::UNKNOWN)), position(Position{0, 0}), 
               is_function(false), isInitialized(false) {}
    
    Symbol(const std::string& n, const TypeInfo& t, const Position& pos, bool isFunc = false)
        : name(n), type(t), position(pos), is_function(isFunc), isInitialized(false) {}
};

// Scope management
class Scope {
private:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;
    
public:
    explicit Scope(Scope* parent = nullptr) : parent(parent) {}
    
    void define(const Symbol& symbol);
    Symbol* lookup(const std::string& name);
    Symbol* lookupLocal(const std::string& name);
    bool exists(const std::string& name) const;
    bool existsLocal(const std::string& name) const;
    
    Scope* getParent() const { return parent; }
    const std::unordered_map<std::string, Symbol>& getSymbols() const { return symbols; }
};

class SemanticError {
public:
    Position position;
    std::string message;
    
    SemanticError(const Position& pos, const std::string& msg)
        : position(pos), message(msg) {}
};

class SemanticAnalyzer : public ASTVisitor {
private:
    std::vector<Scope> scopes;
    std::vector<SemanticError> errors;
    TypeInfo currentFunctionReturnType;
    bool inFunction;
    
public:
    SemanticAnalyzer();
    
    // Main analysis method
    void analyze(Program& program);
    
    // Error handling
    const std::vector<SemanticError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    // Expression visitors
    void visitIntegerLiteral(IntegerLiteral& node) override;
    void visitFloatLiteral(FloatLiteral& node) override;
    void visitStringLiteral(StringLiteral& node) override;
    void visitBooleanLiteral(BooleanLiteral& node) override;
    void visitIdentifier(Identifier& node) override;
    void visitBinaryExpression(BinaryExpression& node) override;
    void visitUnaryExpression(UnaryExpression& node) override;
    void visitFunctionCall(FunctionCall& node) override;
    
    // Statement visitors
    void visitExpressionStatement(ExpressionStatement& node) override;
    void visitVariableDeclaration(VariableDeclaration& node) override;
    void visitAssignmentStatement(AssignmentStatement& node) override;
    void visitBlockStatement(BlockStatement& node) override;
    void visitIfStatement(IfStatement& node) override;
    void visitWhileStatement(WhileStatement& node) override;
    void visitForStatement(ForStatement& node) override;
    void visitReturnStatement(ReturnStatement& node) override;
    
    // Declaration visitors
    void visitFunctionDeclaration(FunctionDeclaration& node) override;
    void visitProgram(Program& node) override;
    
private:
    // Scope management
    void beginScope();
    void endScope();
    Scope& currentScope();
    
    // Symbol management
    void defineSymbol(const Symbol& symbol);
    Symbol* lookupSymbol(const std::string& name);
    
    // Type checking
    TypeInfo getCompatibleType(const TypeInfo& left, const TypeInfo& right, BinaryOperator op);
    TypeInfo getUnaryResultType(const TypeInfo& operand, UnaryOperator op);
    bool isNumericOperation(BinaryOperator op);
    bool isComparisonOperation(BinaryOperator op);
    bool isLogicalOperation(BinaryOperator op);
    bool canConvert(const TypeInfo& from, const TypeInfo& to);
    
    // Error reporting
    void addError(const Position& position, const std::string& message);
    
    // Built-in functions
    void defineBuiltinFunctions();
};

} // namespace SimpleLang