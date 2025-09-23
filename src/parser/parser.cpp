#include "parser.h"
#include <stdexcept>
#include <iostream>

namespace SimpleLang {

Parser::Parser(std::vector<Token> tokens) 
    : tokens(std::move(tokens)), current(0) {}

std::unique_ptr<Program> Parser::parse() {
    try {
        return program();
    } catch (const std::exception& e) {
        addError("Fatal parsing error: " + std::string(e.what()));
        return std::make_unique<Program>();
    }
}

const Token& Parser::peek() const {
    if (current >= tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", Position());
        return eofToken;
    }
    return tokens[current];
}

const Token& Parser::peekNext() const {
    if (current + 1 >= tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", Position());
        return eofToken;
    }
    return tokens[current + 1];
}

const Token& Parser::previous() const {
    if (current == 0) return tokens[0];
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    addError(message + " Got: " + TokenUtils::tokenTypeToString(peek().type));
    throw std::runtime_error(message);
}

void Parser::addError(const std::string& message) {
    errors.emplace_back(peek().position, message);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::FUNCTION:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<Program> Parser::program() {
    auto prog = std::make_unique<Program>(peek().position);
    
    while (!isAtEnd()) {
        try {
            if (match(TokenType::NEWLINE)) {
                continue; // Skip newlines at top level
            }
            
            if (check(TokenType::FUNCTION)) {
                auto func = functionDeclaration();
                if (func) {
                    prog->functions.push_back(std::move(func));
                }
            } else if (check(TokenType::VAR)) {
                auto var = variableDeclaration();
                if (var) {
                    prog->globalVariables.push_back(std::move(var));
                }
            } else {
                addError("Expected function or variable declaration");
                synchronize();
            }
        } catch (const std::exception&) {
            synchronize();
        }
    }
    
    return prog;
}

std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    Position pos = peek().position;
    consume(TokenType::FUNCTION, "Expected 'function'");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<Parameter> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        params = parameterList();
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    TypeInfo returnType(Type::VOID);
    if (match(TokenType::COLON)) {
        returnType = parseType();
    }
    
    auto body = blockStatement();
    
    return std::make_unique<FunctionDeclaration>(
        pos, name.lexeme, std::move(params), returnType, std::move(body)
    );
}

std::unique_ptr<VariableDeclaration> Parser::variableDeclaration() {
    Position pos = peek().position;
    consume(TokenType::VAR, "Expected 'var'");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    TypeInfo type(Type::UNKNOWN);
    if (match(TokenType::COLON)) {
        type = parseType();
    }
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VariableDeclaration>(
        pos, name.lexeme, type, std::move(initializer)
    );
}

std::vector<Parameter> Parser::parameterList() {
    std::vector<Parameter> params;
    
    do {
        params.push_back(parameter());
    } while (match(TokenType::COMMA));
    
    return params;
}

Parameter Parser::parameter() {
    Position pos = peek().position;
    Token name = consume(TokenType::IDENTIFIER, "Expected parameter name");
    consume(TokenType::COLON, "Expected ':' after parameter name");
    TypeInfo type = parseType();
    
    return Parameter(name.lexeme, type, pos);
}

TypeInfo Parser::parseType() {
    if (match({TokenType::INT_TYPE, TokenType::FLOAT_TYPE, TokenType::BOOL_TYPE, 
               TokenType::STRING_TYPE, TokenType::VOID_TYPE})) {
        return TypeInfo::fromTokenType(previous().type);
    }
    
    addError("Expected type");
    return TypeInfo(Type::UNKNOWN);
}

std::unique_ptr<Statement> Parser::statement() {
    try {
        if (match(TokenType::NEWLINE)) {
            return statement(); // Skip newlines
        }
        
        if (check(TokenType::LEFT_BRACE)) {
            return blockStatement();
        }
        
        if (match(TokenType::IF)) {
            return ifStatement();
        }
        
        if (match(TokenType::WHILE)) {
            return whileStatement();
        }
        
        if (match(TokenType::FOR)) {
            return forStatement();
        }
        
        if (match(TokenType::RETURN)) {
            return returnStatement();
        }
        
        if (match(TokenType::VAR)) {
            current--; // Back up to parse as variable declaration
            return variableDeclaration();
        }
        
        // Check for assignment vs expression statement
        if (check(TokenType::IDENTIFIER) && peekNext().type == TokenType::ASSIGN) {
            return assignmentStatement();
        }
        
        return expressionStatement();
        
    } catch (const std::exception&) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {
    Position pos = peek().position;
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = statement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    
    return std::make_unique<BlockStatement>(pos, std::move(statements));
}

std::unique_ptr<ExpressionStatement> Parser::expressionStatement() {
    Position pos = peek().position;
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    
    return std::make_unique<ExpressionStatement>(pos, std::move(expr));
}

std::unique_ptr<AssignmentStatement> Parser::assignmentStatement() {
    Position pos = peek().position;
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::ASSIGN, "Expected '='");
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    
    return std::make_unique<AssignmentStatement>(pos, name.lexeme, std::move(value));
}

std::unique_ptr<IfStatement> Parser::ifStatement() {
    Position pos = previous().position; // 'if' was already consumed
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    
    auto thenStmt = statement();
    std::unique_ptr<Statement> elseStmt = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseStmt = statement();
    }
    
    return std::make_unique<IfStatement>(pos, std::move(condition), 
                                        std::move(thenStmt), std::move(elseStmt));
}

std::unique_ptr<WhileStatement> Parser::whileStatement() {
    Position pos = previous().position; // 'while' was already consumed
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    
    auto body = statement();
    
    return std::make_unique<WhileStatement>(pos, std::move(condition), std::move(body));
}

std::unique_ptr<ForStatement> Parser::forStatement() {
    Position pos = previous().position; // 'for' was already consumed
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Initializer
    std::unique_ptr<Statement> initializer = nullptr;
    if (match(TokenType::SEMICOLON)) {
        // No initializer
    } else if (match(TokenType::VAR)) {
        current--; // Back up
        initializer = variableDeclaration();
    } else {
        initializer = expressionStatement();
    }
    
    // Condition
    std::unique_ptr<Expression> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for loop condition");
    
    // Increment
    std::unique_ptr<Statement> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        if (check(TokenType::IDENTIFIER) && peekNext().type == TokenType::ASSIGN) {
            increment = assignmentStatement();
            // Remove the semicolon requirement for increment in for loop
            if (previous().type == TokenType::SEMICOLON) {
                current--; // Back up if semicolon was consumed
            }
        } else {
            auto expr = expression();
            increment = std::make_unique<ExpressionStatement>(expr->position, std::move(expr));
        }
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after for clauses");
    
    auto body = statement();
    
    return std::make_unique<ForStatement>(pos, std::move(initializer), 
                                         std::move(condition), std::move(increment), 
                                         std::move(body));
}

std::unique_ptr<ReturnStatement> Parser::returnStatement() {
    Position pos = previous().position; // 'return' was already consumed
    
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return value");
    
    return std::make_unique<ReturnStatement>(pos, std::move(value));
}

// Expression parsing (operator precedence)
std::unique_ptr<Expression> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match(TokenType::OR)) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match(TokenType::AND)) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = equality();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::NOT_EQUAL, TokenType::EQUAL})) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = addition();
    
    while (match({TokenType::GREATER_THAN, TokenType::GREATER_EQUAL, 
                  TokenType::LESS_THAN, TokenType::LESS_EQUAL})) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = addition();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::addition() {
    auto expr = multiplication();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = multiplication();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::multiplication() {
    auto expr = unary();
    
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MODULO})) {
        BinaryOperator op = tokenToBinaryOperator(previous().type);
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(expr->position, std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS, TokenType::PLUS})) {
        UnaryOperator op = tokenToUnaryOperator(previous().type);
        auto right = unary();
        return std::make_unique<UnaryExpression>(previous().position, op, std::move(right));
    }
    
    return postfix();
}

std::unique_ptr<Expression> Parser::postfix() {
    auto expr = primary();
    
    // Handle function calls
    if (auto identifier = dynamic_cast<Identifier*>(expr.get())) {
        if (match(TokenType::LEFT_PAREN)) {
            std::string name = identifier->name;
            Position pos = identifier->position;
            
            std::vector<std::unique_ptr<Expression>> args;
            if (!check(TokenType::RIGHT_PAREN)) {
                args = argumentList();
            }
            
            consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
            
            return std::make_unique<FunctionCall>(pos, name, std::move(args));
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::TRUE)) {
        return std::make_unique<BooleanLiteral>(previous().position, true);
    }
    
    if (match(TokenType::FALSE)) {
        return std::make_unique<BooleanLiteral>(previous().position, false);
    }
    
    if (match(TokenType::INTEGER)) {
        Token token = previous();
        return std::make_unique<IntegerLiteral>(token.position, token.int_value);
    }
    
    if (match(TokenType::FLOAT)) {
        Token token = previous();
        return std::make_unique<FloatLiteral>(token.position, token.float_value);
    }
    
    if (match(TokenType::STRING)) {
        Token token = previous();
        return std::make_unique<StringLiteral>(token.position, token.lexeme);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token token = previous();
        return std::make_unique<Identifier>(token.position, token.lexeme);
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    addError("Expected expression");
    throw std::runtime_error("Expected expression");
}

std::vector<std::unique_ptr<Expression>> Parser::argumentList() {
    std::vector<std::unique_ptr<Expression>> args;
    
    do {
        args.push_back(expression());
    } while (match(TokenType::COMMA));
    
    return args;
}

BinaryOperator Parser::tokenToBinaryOperator(TokenType type) {
    switch (type) {
        case TokenType::PLUS: return BinaryOperator::ADD;
        case TokenType::MINUS: return BinaryOperator::SUBTRACT;
        case TokenType::MULTIPLY: return BinaryOperator::MULTIPLY;
        case TokenType::DIVIDE: return BinaryOperator::DIVIDE;
        case TokenType::MODULO: return BinaryOperator::MODULO;
        case TokenType::EQUAL: return BinaryOperator::EQUAL;
        case TokenType::NOT_EQUAL: return BinaryOperator::NOT_EQUAL;
        case TokenType::LESS_THAN: return BinaryOperator::LESS_THAN;
        case TokenType::LESS_EQUAL: return BinaryOperator::LESS_EQUAL;
        case TokenType::GREATER_THAN: return BinaryOperator::GREATER_THAN;
        case TokenType::GREATER_EQUAL: return BinaryOperator::GREATER_EQUAL;
        case TokenType::AND: return BinaryOperator::AND;
        case TokenType::OR: return BinaryOperator::OR;
        default:
            throw std::runtime_error("Invalid binary operator");
    }
}

UnaryOperator Parser::tokenToUnaryOperator(TokenType type) {
    switch (type) {
        case TokenType::MINUS: return UnaryOperator::MINUS;
        case TokenType::PLUS: return UnaryOperator::PLUS;
        case TokenType::NOT: return UnaryOperator::NOT;
        default:
            throw std::runtime_error("Invalid unary operator");
    }
}

} // namespace SimpleLang