#pragma once

#include <string>
#include <unordered_map>

namespace SimpleLang {

enum class TokenType {
    // Literals
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    
    // Identifiers
    IDENTIFIER,
    
    // Keywords
    FUNCTION,
    VAR,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    TRUE,
    FALSE,
    
    // Types
    INT_TYPE,
    FLOAT_TYPE,
    BOOL_TYPE,
    STRING_TYPE,
    VOID_TYPE,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    
    // Comparison
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    
    // Logical
    AND,
    OR,
    NOT,
    
    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    SEMICOLON,
    COLON,
    
    // Special
    NEWLINE,
    EOF_TOKEN,
    INVALID
};

struct Position {
    int line;
    int column;
    
    Position(int l = 1, int c = 1) : line(l), column(c) {}
    
    void advance(char ch) {
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
};

struct Token {
    TokenType type;
    std::string lexeme;
    Position position;
    
    // For literal values
    union {
        long long int_value;
        double float_value;
        bool bool_value;
    };
    
    Token(TokenType t, const std::string& lex, const Position& pos)
        : type(t), lexeme(lex), position(pos), int_value(0) {}
    
    Token(TokenType t, const std::string& lex, const Position& pos, long long value)
        : type(t), lexeme(lex), position(pos), int_value(value) {}
    
    Token(TokenType t, const std::string& lex, const Position& pos, double value)
        : type(t), lexeme(lex), position(pos), float_value(value) {}
    
    Token(TokenType t, const std::string& lex, const Position& pos, bool value)
        : type(t), lexeme(lex), position(pos), bool_value(value) {}
};

class TokenUtils {
public:
    static std::string tokenTypeToString(TokenType type);
    static bool isKeyword(const std::string& word);
    static TokenType getKeywordType(const std::string& word);
    static const std::unordered_map<std::string, TokenType>& getKeywords();
    
private:
    static std::unordered_map<std::string, TokenType> keywords;
};

} // namespace SimpleLang