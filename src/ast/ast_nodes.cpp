#include "ast_nodes.h"
#include "ast_visitor.h"

namespace SimpleLang {

// TypeInfo implementation
bool TypeInfo::isCompatible(const TypeInfo& other) const {
    if (type == other.type) return true;
    
    // Numeric types are compatible with each other
    if (isNumeric() && other.isNumeric()) return true;
    
    return false;
}

TypeInfo TypeInfo::fromTokenType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::INT_TYPE: return TypeInfo(Type::INT);
        case TokenType::FLOAT_TYPE: return TypeInfo(Type::FLOAT);
        case TokenType::BOOL_TYPE: return TypeInfo(Type::BOOL);
        case TokenType::STRING_TYPE: return TypeInfo(Type::STRING);
        case TokenType::VOID_TYPE: return TypeInfo(Type::VOID);
        default: return TypeInfo(Type::UNKNOWN);
    }
}

// Expression node implementations
void IntegerLiteral::accept(ASTVisitor& visitor) {
    visitor.visitIntegerLiteral(*this);
}

void FloatLiteral::accept(ASTVisitor& visitor) {
    visitor.visitFloatLiteral(*this);
}

void StringLiteral::accept(ASTVisitor& visitor) {
    visitor.visitStringLiteral(*this);
}

void BooleanLiteral::accept(ASTVisitor& visitor) {
    visitor.visitBooleanLiteral(*this);
}

void Identifier::accept(ASTVisitor& visitor) {
    visitor.visitIdentifier(*this);
}

void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visitBinaryExpression(*this);
}

void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visitUnaryExpression(*this);
}

void FunctionCall::accept(ASTVisitor& visitor) {
    visitor.visitFunctionCall(*this);
}

// Statement node implementations
void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visitExpressionStatement(*this);
}

void VariableDeclaration::accept(ASTVisitor& visitor) {
    visitor.visitVariableDeclaration(*this);
}

void AssignmentStatement::accept(ASTVisitor& visitor) {
    visitor.visitAssignmentStatement(*this);
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visitBlockStatement(*this);
}

void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visitIfStatement(*this);
}

void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visitWhileStatement(*this);
}

void ForStatement::accept(ASTVisitor& visitor) {
    visitor.visitForStatement(*this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visitReturnStatement(*this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visitFunctionDeclaration(*this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visitProgram(*this);
}

} // namespace SimpleLang