#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <functional>

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/semantic/semantic_analyzer.h"
#include "../src/codegen/code_generator.h"

using namespace SimpleLang;

class TestFramework {
private:
    int totalTests = 0;
    int passedTests = 0;
    
public:
    void runTest(const std::string& testName, std::function<void()> test) {
        totalTests++;
        try {
            test();
            passedTests++;
            std::cout << "[PASS] " << testName << std::endl;
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << testName << ": " << e.what() << std::endl;
        }
    }
    
    void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passedTests << "/" << totalTests << std::endl;
        if (passedTests == totalTests) {
            std::cout << "All tests passed!" << std::endl;
        } else {
            std::cout << "Some tests failed." << std::endl;
        }
    }
};

// Lexer Tests
void testLexerBasic() {
    std::string source = "var x: int = 42;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hasErrors());
    assert(tokens.size() == 8); // var, x, :, int, =, 42, ;, EOF
    assert(tokens[0].type == TokenType::VAR);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::COLON);
    assert(tokens[3].type == TokenType::INT_TYPE);
    assert(tokens[4].type == TokenType::ASSIGN);
    assert(tokens[5].type == TokenType::INTEGER);
    assert(tokens[6].type == TokenType::SEMICOLON);
}

void testLexerKeywords() {
    std::string source = "function if else while for return true false";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hasErrors());
    assert(tokens[0].type == TokenType::FUNCTION);
    assert(tokens[1].type == TokenType::IF);
    assert(tokens[2].type == TokenType::ELSE);
    assert(tokens[3].type == TokenType::WHILE);
    assert(tokens[4].type == TokenType::FOR);
    assert(tokens[5].type == TokenType::RETURN);
    assert(tokens[6].type == TokenType::BOOLEAN);
    assert(tokens[7].type == TokenType::BOOLEAN);
}

void testLexerOperators() {
    std::string source = "+ - * / % == != < <= > >= && || !";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hasErrors());
    assert(tokens[0].type == TokenType::PLUS);
    assert(tokens[1].type == TokenType::MINUS);
    assert(tokens[2].type == TokenType::MULTIPLY);
    assert(tokens[3].type == TokenType::DIVIDE);
    assert(tokens[4].type == TokenType::MODULO);
    assert(tokens[5].type == TokenType::EQUAL);
    assert(tokens[6].type == TokenType::NOT_EQUAL);
    assert(tokens[7].type == TokenType::LESS_THAN);
    assert(tokens[8].type == TokenType::LESS_EQUAL);
    assert(tokens[9].type == TokenType::GREATER_THAN);
    assert(tokens[10].type == TokenType::GREATER_EQUAL);
    assert(tokens[11].type == TokenType::AND);
    assert(tokens[12].type == TokenType::OR);
    assert(tokens[13].type == TokenType::NOT);
}

void testLexerLiterals() {
    std::string source = R"(42 3.14 "hello world" true false)";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hasErrors());
    assert(tokens[0].type == TokenType::INTEGER);
    assert(tokens[0].int_value == 42);
    assert(tokens[1].type == TokenType::FLOAT);
    assert(tokens[1].float_value == 3.14);
    assert(tokens[2].type == TokenType::STRING);
    assert(tokens[2].lexeme == "hello world");
    assert(tokens[3].type == TokenType::BOOLEAN);
    assert(tokens[3].bool_value == true);
    assert(tokens[4].type == TokenType::BOOLEAN);
    assert(tokens[4].bool_value == false);
}

// Parser Tests
void testParserSimpleFunction() {
    std::string source = R"(
        function main(): int {
            return 0;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    assert(!lexer.hasErrors());
    
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    assert(!parser.hasErrors());
    assert(ast != nullptr);
    assert(ast->functions.size() == 1);
    assert(ast->functions[0]->name == "main");
}

void testParserVariableDeclaration() {
    std::string source = R"(
        function test(): void {
            var x: int = 42;
            var y: float;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    assert(!lexer.hasErrors());
    
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    assert(!parser.hasErrors());
    assert(ast->functions.size() == 1);
    
    auto& body = ast->functions[0]->body;
    assert(body->statements.size() == 2);
}

void testParserExpressions() {
    std::string source = R"(
        function test(): int {
            var result: int = 1 + 2 * 3;
            return result;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    assert(!lexer.hasErrors());
    
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    assert(!parser.hasErrors());
}

// Semantic Analysis Tests
void testSemanticBasic() {
    std::string source = R"(
        function main(): int {
            var x: int = 42;
            return x;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);
    assert(!analyzer.hasErrors());
}

void testSemanticUndefinedVariable() {
    std::string source = R"(
        function main(): int {
            return x; // x is undefined
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);
    assert(analyzer.hasErrors()); // Should report undefined variable
}

void testSemanticTypeMismatch() {
    std::string source = R"(
        function main(): int {
            var x: int = true; // Type mismatch
            return x;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);
    assert(analyzer.hasErrors()); // Should report type mismatch
}

// Code Generation Tests
void testCodeGenBasic() {
    std::string source = R"(
        function main(): int {
            var x: int = 42;
            return x;
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);
    assert(!analyzer.hasErrors());
    
    CodeGenerator generator;
    auto instructions = generator.generate(*ast);
    assert(!instructions.empty());
}

// Integration Tests
void testFullCompilerPipeline() {
    std::string source = R"(
        function factorial(n: int): int {
            if (n <= 1) {
                return 1;
            } else {
                return n * factorial(n - 1);
            }
        }
        
        function main(): int {
            var result: int = factorial(5);
            return result;
        }
    )";
    
    // Lexical Analysis
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    assert(!lexer.hasErrors());
    
    // Syntax Analysis
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    assert(!parser.hasErrors());
    
    // Semantic Analysis
    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);
    assert(!analyzer.hasErrors());
    
    // Code Generation
    CodeGenerator generator;
    auto instructions = generator.generate(*ast);
    assert(!instructions.empty());
    
    // Generate C code
    std::string cCode = generator.generateC(instructions);
    assert(!cCode.empty());
}

int main() {
    TestFramework framework;
    
    std::cout << "Running SimpleLang Compiler Tests...\n" << std::endl;
    
    // Lexer Tests
    framework.runTest("Lexer Basic", testLexerBasic);
    framework.runTest("Lexer Keywords", testLexerKeywords);
    framework.runTest("Lexer Operators", testLexerOperators);
    framework.runTest("Lexer Literals", testLexerLiterals);
    
    // Parser Tests
    framework.runTest("Parser Simple Function", testParserSimpleFunction);
    framework.runTest("Parser Variable Declaration", testParserVariableDeclaration);
    framework.runTest("Parser Expressions", testParserExpressions);
    
    // Semantic Analysis Tests
    framework.runTest("Semantic Basic", testSemanticBasic);
    framework.runTest("Semantic Undefined Variable", testSemanticUndefinedVariable);
    framework.runTest("Semantic Type Mismatch", testSemanticTypeMismatch);
    
    // Code Generation Tests
    framework.runTest("Code Generation Basic", testCodeGenBasic);
    
    // Integration Tests
    framework.runTest("Full Compiler Pipeline", testFullCompilerPipeline);
    
    framework.printSummary();
    
    return 0;
}