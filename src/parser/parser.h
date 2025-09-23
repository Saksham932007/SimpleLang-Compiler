#pragma once

#include "../lexer/lexer.h"
#include "../ast/ast_nodes.h"
#include <vector>
#include <memory>

namespace SimpleLang {

class ParseError {
public:
    Position position;
    std::string message;
    
    ParseError(const Position& pos, const std::string& msg)
        : position(pos), message(msg) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    std::vector<ParseError> errors;
    
public:
    explicit Parser(std::vector<Token> tokens);
    
    // Main parsing method
    std::unique_ptr<Program> parse();
    
    // Error handling
    const std::vector<ParseError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
private:
    // Token utilities
    const Token& peek() const;
    const Token& peekNext() const;
    const Token& previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token advance();
    Token consume(TokenType type, const std::string& message);
    
    // Error handling and recovery
    void addError(const std::string& message);
    void synchronize();
    
    // Parsing methods (recursive descent)
    std::unique_ptr<Program> program();
    std::unique_ptr<FunctionDeclaration> functionDeclaration();
    std::unique_ptr<VariableDeclaration> variableDeclaration();
    std::vector<Parameter> parameterList();
    Parameter parameter();
    TypeInfo parseType();
    
    // Statements
    std::unique_ptr<Statement> statement();
    std::unique_ptr<BlockStatement> blockStatement();
    std::unique_ptr<ExpressionStatement> expressionStatement();
    std::unique_ptr<AssignmentStatement> assignmentStatement();
    std::unique_ptr<IfStatement> ifStatement();
    std::unique_ptr<WhileStatement> whileStatement();
    std::unique_ptr<ForStatement> forStatement();
    std::unique_ptr<ReturnStatement> returnStatement();
    
    // Expressions (operator precedence parsing)
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> postfix();
    std::unique_ptr<Expression> primary();
    
    // Function calls
    std::unique_ptr<FunctionCall> functionCall(const std::string& name);
    std::vector<std::unique_ptr<Expression>> argumentList();
    
    // Utility methods
    BinaryOperator tokenToBinaryOperator(TokenType type);
    UnaryOperator tokenToUnaryOperator(TokenType type);
    bool isAssignmentTarget();
};

} // namespace SimpleLang