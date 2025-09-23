#pragma once

#include "../lexer/token.h"
#include <memory>
#include <vector>
#include <string>

namespace SimpleLang {

// Forward declarations
class ASTVisitor;

// Base AST node class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    
    Position position;
    
protected:
    ASTNode(const Position& pos) : position(pos) {}
};

// Type system
enum class Type {
    INT,
    FLOAT,
    BOOL,
    STRING,
    VOID,
    UNKNOWN
};

struct TypeInfo {
    Type type;
    std::string name;
    
    TypeInfo(Type t = Type::UNKNOWN) : type(t) {
        switch (t) {
            case Type::INT: name = "int"; break;
            case Type::FLOAT: name = "float"; break;
            case Type::BOOL: name = "bool"; break;
            case Type::STRING: name = "string"; break;
            case Type::VOID: name = "void"; break;
            default: name = "unknown"; break;
        }
    }
    
    bool isCompatible(const TypeInfo& other) const;
    bool isNumeric() const { return type == Type::INT || type == Type::FLOAT; }
    static TypeInfo fromTokenType(TokenType tokenType);
};

// Expression nodes
class Expression : public ASTNode {
public:
    TypeInfo type;
    
protected:
    Expression(const Position& pos) : ASTNode(pos) {}
};

// Literal expressions
class IntegerLiteral : public Expression {
public:
    long long value;
    
    IntegerLiteral(const Position& pos, long long val) 
        : Expression(pos), value(val) {
        type = TypeInfo(Type::INT);
    }
    
    void accept(ASTVisitor& visitor) override;
};

class FloatLiteral : public Expression {
public:
    double value;
    
    FloatLiteral(const Position& pos, double val) 
        : Expression(pos), value(val) {
        type = TypeInfo(Type::FLOAT);
    }
    
    void accept(ASTVisitor& visitor) override;
};

class StringLiteral : public Expression {
public:
    std::string value;
    
    StringLiteral(const Position& pos, const std::string& val) 
        : Expression(pos), value(val) {
        type = TypeInfo(Type::STRING);
    }
    
    void accept(ASTVisitor& visitor) override;
};

class BooleanLiteral : public Expression {
public:
    bool value;
    
    BooleanLiteral(const Position& pos, bool val) 
        : Expression(pos), value(val) {
        type = TypeInfo(Type::BOOL);
    }
    
    void accept(ASTVisitor& visitor) override;
};

class Identifier : public Expression {
public:
    std::string name;
    
    Identifier(const Position& pos, const std::string& n) 
        : Expression(pos), name(n) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Binary expressions
enum class BinaryOperator {
    ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
    EQUAL, NOT_EQUAL, LESS_THAN, LESS_EQUAL, GREATER_THAN, GREATER_EQUAL,
    AND, OR
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    BinaryOperator operator_;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(const Position& pos, std::unique_ptr<Expression> l, 
                    BinaryOperator op, std::unique_ptr<Expression> r)
        : Expression(pos), left(std::move(l)), operator_(op), right(std::move(r)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Unary expressions
enum class UnaryOperator {
    MINUS, PLUS, NOT
};

class UnaryExpression : public Expression {
public:
    UnaryOperator operator_;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const Position& pos, UnaryOperator op, std::unique_ptr<Expression> operand)
        : Expression(pos), operator_(op), operand(std::move(operand)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Function call expression
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    FunctionCall(const Position& pos, const std::string& name, 
                std::vector<std::unique_ptr<Expression>> args)
        : Expression(pos), functionName(name), arguments(std::move(args)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
protected:
    Statement(const Position& pos) : ASTNode(pos) {}
};

// Expression statement
class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(const Position& pos, std::unique_ptr<Expression> expr)
        : Statement(pos), expression(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Variable declaration
class VariableDeclaration : public Statement {
public:
    std::string name;
    TypeInfo type;
    std::unique_ptr<Expression> initializer;
    
    VariableDeclaration(const Position& pos, const std::string& n, 
                       const TypeInfo& t, std::unique_ptr<Expression> init = nullptr)
        : Statement(pos), name(n), type(t), initializer(std::move(init)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Assignment statement
class AssignmentStatement : public Statement {
public:
    std::string variableName;
    std::unique_ptr<Expression> value;
    
    AssignmentStatement(const Position& pos, const std::string& name, 
                       std::unique_ptr<Expression> val)
        : Statement(pos), variableName(name), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Block statement
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement(const Position& pos, std::vector<std::unique_ptr<Statement>> stmts)
        : Statement(pos), statements(std::move(stmts)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// If statement
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenStatement;
    std::unique_ptr<Statement> elseStatement;
    
    IfStatement(const Position& pos, std::unique_ptr<Expression> cond, 
               std::unique_ptr<Statement> then, std::unique_ptr<Statement> else_ = nullptr)
        : Statement(pos), condition(std::move(cond)), 
          thenStatement(std::move(then)), elseStatement(std::move(else_)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// While statement
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    
    WhileStatement(const Position& pos, std::unique_ptr<Expression> cond, 
                  std::unique_ptr<Statement> b)
        : Statement(pos), condition(std::move(cond)), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// For statement
class ForStatement : public Statement {
public:
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> increment;
    std::unique_ptr<Statement> body;
    
    ForStatement(const Position& pos, std::unique_ptr<Statement> init,
                std::unique_ptr<Expression> cond, std::unique_ptr<Statement> inc,
                std::unique_ptr<Statement> b)
        : Statement(pos), initializer(std::move(init)), condition(std::move(cond)),
          increment(std::move(inc)), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Return statement
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    ReturnStatement(const Position& pos, std::unique_ptr<Expression> val = nullptr)
        : Statement(pos), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Function parameter
struct Parameter {
    std::string name;
    TypeInfo type;
    Position position;
    
    Parameter(const std::string& n, const TypeInfo& t, const Position& pos)
        : name(n), type(t), position(pos) {}
};

// Function declaration
class FunctionDeclaration : public ASTNode {
public:
    std::string name;
    std::vector<Parameter> parameters;
    TypeInfo returnType;
    std::unique_ptr<BlockStatement> body;
    
    FunctionDeclaration(const Position& pos, const std::string& n, 
                       std::vector<Parameter> params, const TypeInfo& retType,
                       std::unique_ptr<BlockStatement> b)
        : ASTNode(pos), name(n), parameters(std::move(params)), 
          returnType(retType), body(std::move(b)) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;
    std::vector<std::unique_ptr<VariableDeclaration>> globalVariables;
    
    Program(const Position& pos = Position()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
};

} // namespace SimpleLang