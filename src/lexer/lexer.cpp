#include "lexer.h"
#include <cctype>
#include <sstream>

namespace SimpleLang {

Lexer::Lexer(const std::string& source) 
    : source(source), current(0), position(1, 1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = scanToken();
        if (token.type != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    // Add exactly one EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", position);
    return tokens;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source[current];
    current++;
    position.advance(c);
    return c;
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    position.advance(expected);
    return true;
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\r' || c == '\t';
}

Token Lexer::makeToken(TokenType type) const {
    return Token(type, "", position);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) const {
    return Token(type, lexeme, position);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, long long value) const {
    return Token(type, lexeme, position, value);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, double value) const {
    return Token(type, lexeme, position, value);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, bool value) const {
    return Token(type, lexeme, position, value);
}

Token Lexer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::INVALID); // Let tokenize() add EOF
    }
    
    Position startPos = position;
    char c = advance();
    
    // Single-character tokens
    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", startPos);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startPos);
        case '{': return Token(TokenType::LEFT_BRACE, "{", startPos);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startPos);
        case ',': return Token(TokenType::COMMA, ",", startPos);
        case ';': return Token(TokenType::SEMICOLON, ";", startPos);
        case ':': return Token(TokenType::COLON, ":", startPos);
        case '+': return Token(TokenType::PLUS, "+", startPos);
        case '-': return Token(TokenType::MINUS, "-", startPos);
        case '*': return Token(TokenType::MULTIPLY, "*", startPos);
        case '%': return Token(TokenType::MODULO, "%", startPos);
        case '\n': return Token(TokenType::NEWLINE, "\\n", startPos);
    }
    
    // Two-character tokens
    switch (c) {
        case '!':
            if (match('=')) {
                return Token(TokenType::NOT_EQUAL, "!=", startPos);
            } else {
                return Token(TokenType::NOT, "!", startPos);
            }
        case '=':
            if (match('=')) {
                return Token(TokenType::EQUAL, "==", startPos);
            } else {
                return Token(TokenType::ASSIGN, "=", startPos);
            }
        case '<':
            if (match('=')) {
                return Token(TokenType::LESS_EQUAL, "<=", startPos);
            } else {
                return Token(TokenType::LESS_THAN, "<", startPos);
            }
        case '>':
            if (match('=')) {
                return Token(TokenType::GREATER_EQUAL, ">=", startPos);
            } else {
                return Token(TokenType::GREATER_THAN, ">", startPos);
            }
        case '&':
            if (match('&')) {
                return Token(TokenType::AND, "&&", startPos);
            } else {
                addError("Unexpected character '&'. Did you mean '&&'?");
                return Token(TokenType::INVALID, "&", startPos);
            }
        case '|':
            if (match('|')) {
                return Token(TokenType::OR, "||", startPos);
            } else {
                addError("Unexpected character '|'. Did you mean '||'?");
                return Token(TokenType::INVALID, "|", startPos);
            }
        case '/':
            if (match('/')) {
                // Line comment
                return comment();
            } else {
                return Token(TokenType::DIVIDE, "/", startPos);
            }
    }
    
    // String literals
    if (c == '"') {
        current--; // Back up to include the quote
        position.column--; // Adjust position
        return string();
    }
    
    // Number literals
    if (isDigit(c)) {
        current--; // Back up to include the digit
        position.column--; // Adjust position
        return number();
    }
    
    // Identifiers and keywords
    if (isAlpha(c)) {
        current--; // Back up to include the character
        position.column--; // Adjust position
        return identifier();
    }
    
    // Unexpected character
    addError(std::string("Unexpected character: '") + c + "'");
    return Token(TokenType::INVALID, std::string(1, c), startPos);
}

Token Lexer::string() {
    Position startPos = position;
    std::string value;
    
    advance(); // Consume opening quote
    
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            addError("Unterminated string literal");
            return Token(TokenType::INVALID, value, startPos);
        }
        
        if (peek() == '\\') {
            advance(); // Consume backslash
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default:
                    addError(std::string("Unknown escape sequence: \\") + escaped);
                    value += escaped;
                    break;
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        addError("Unterminated string literal");
        return Token(TokenType::INVALID, value, startPos);
    }
    
    advance(); // Consume closing quote
    return Token(TokenType::STRING, value, startPos);
}

Token Lexer::number() {
    Position startPos = position;
    size_t start = current;
    
    while (isDigit(peek())) {
        advance();
    }
    
    bool isFloat = false;
    if (peek() == '.' && isDigit(peekNext())) {
        isFloat = true;
        advance(); // Consume '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string lexeme = substring(start, current);
    
    if (isFloat) {
        double value = std::stod(lexeme);
        return Token(TokenType::FLOAT, lexeme, startPos, value);
    } else {
        long long value = std::stoll(lexeme);
        return Token(TokenType::INTEGER, lexeme, startPos, value);
    }
}

Token Lexer::identifier() {
    Position startPos = position;
    size_t start = current;
    
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    std::string text = substring(start, current);
    
    if (TokenUtils::isKeyword(text)) {
        TokenType type = TokenUtils::getKeywordType(text);
        if (type == TokenType::TRUE) {
            return Token(TokenType::BOOLEAN, text, startPos, true);
        } else if (type == TokenType::FALSE) {
            return Token(TokenType::BOOLEAN, text, startPos, false);
        } else {
            return Token(type, text, startPos);
        }
    }
    
    return Token(TokenType::IDENTIFIER, text, startPos);
}

Token Lexer::comment() {
    // Skip to end of line
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    
    // Comments are not returned as tokens
    return scanToken(); // Continue scanning for the next token
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::addError(const std::string& message) {
    errors.emplace_back(position, message);
}

std::string Lexer::substring(size_t start, size_t end) const {
    return source.substr(start, end - start);
}

} // namespace SimpleLang