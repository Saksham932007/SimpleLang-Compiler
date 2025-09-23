#pragma once

#include "token.h"
#include <string>
#include <vector>
#include <memory>

namespace SimpleLang {

class LexerError {
public:
    Position position;
    std::string message;
    
    LexerError(const Position& pos, const std::string& msg)
        : position(pos), message(msg) {}
};

class Lexer {
private:
    std::string source;
    size_t current;
    Position position;
    std::vector<LexerError> errors;
    
public:
    explicit Lexer(const std::string& source);
    
    // Main tokenization method
    std::vector<Token> tokenize();
    
    // Error handling
    const std::vector<LexerError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
private:
    // Character examination
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    
    // Character classification
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    // Token creation
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& lexeme) const;
    Token makeToken(TokenType type, const std::string& lexeme, long long value) const;
    Token makeToken(TokenType type, const std::string& lexeme, double value) const;
    Token makeToken(TokenType type, const std::string& lexeme, bool value) const;
    
    // Specific token parsers
    Token scanToken();
    Token string();
    Token number();
    Token identifier();
    Token comment();
    
    // Utility methods
    void skipWhitespace();
    void addError(const std::string& message);
    std::string substring(size_t start, size_t end) const;
};

} // namespace SimpleLang