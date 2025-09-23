#pragma once

#include "ast_nodes.h"

namespace SimpleLang {

// Visitor pattern for AST traversal
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expression visitors
    virtual void visitIntegerLiteral(IntegerLiteral& node) = 0;
    virtual void visitFloatLiteral(FloatLiteral& node) = 0;
    virtual void visitStringLiteral(StringLiteral& node) = 0;
    virtual void visitBooleanLiteral(BooleanLiteral& node) = 0;
    virtual void visitIdentifier(Identifier& node) = 0;
    virtual void visitBinaryExpression(BinaryExpression& node) = 0;
    virtual void visitUnaryExpression(UnaryExpression& node) = 0;
    virtual void visitFunctionCall(FunctionCall& node) = 0;
    
    // Statement visitors
    virtual void visitExpressionStatement(ExpressionStatement& node) = 0;
    virtual void visitVariableDeclaration(VariableDeclaration& node) = 0;
    virtual void visitAssignmentStatement(AssignmentStatement& node) = 0;
    virtual void visitBlockStatement(BlockStatement& node) = 0;
    virtual void visitIfStatement(IfStatement& node) = 0;
    virtual void visitWhileStatement(WhileStatement& node) = 0;
    virtual void visitForStatement(ForStatement& node) = 0;
    virtual void visitReturnStatement(ReturnStatement& node) = 0;
    
    // Declaration visitors
    virtual void visitFunctionDeclaration(FunctionDeclaration& node) = 0;
    virtual void visitProgram(Program& node) = 0;
};

// Utility visitor for pretty printing AST
class ASTPrinter : public ASTVisitor {
private:
    std::string result;
    int indentLevel;
    
    void indent();
    void print(const std::string& text);
    void println(const std::string& text);
    
public:
    ASTPrinter() : indentLevel(0) {}
    
    std::string print(ASTNode& node);
    
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
    std::string binaryOpToString(BinaryOperator op);
    std::string unaryOpToString(UnaryOperator op);
};

} // namespace SimpleLang