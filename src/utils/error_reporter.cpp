#include "error_reporter.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../semantic/semantic_analyzer.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace SimpleLang {

std::string CompilerError::toString() const {
    std::ostringstream oss;
    oss << filename << ":" << line << ":" << column << ": " 
        << getTypeString() << ": " << message;
    if (!hint.empty()) {
        oss << "\n  Hint: " << hint;
    }
    return oss.str();
}

std::string CompilerError::getTypeString() const {
    switch (type) {
        case ErrorType::LEXICAL: return "lexical error";
        case ErrorType::SYNTAX: return "syntax error";
        case ErrorType::SEMANTIC: return "semantic error";
        case ErrorType::CODEGEN: return "code generation error";
        case ErrorType::IO: return "I/O error";
        case ErrorType::INTERNAL: return "internal error";
        default: return "error";
    }
}

void ErrorReporter::reportError(ErrorType type, const std::string& filename, 
                               int line, int column, const std::string& message, 
                               const std::string& hint) {
    addError(CompilerError(type, filename, line, column, message, hint));
}

void ErrorReporter::reportWarning(ErrorType type, const std::string& filename, 
                                 int line, int column, const std::string& message, 
                                 const std::string& hint) {
    addWarning(CompilerError(type, filename, line, column, message, hint));
}

void ErrorReporter::reportLexerErrors(const std::vector<LexerError>& lexerErrors, 
                                     const std::string& filename) {
    for (const auto& error : lexerErrors) {
        reportError(ErrorType::LEXICAL, filename, 
                   error.position.line, error.position.column, 
                   error.message);
    }
}

void ErrorReporter::reportParserErrors(const std::vector<ParseError>& parseErrors, 
                                      const std::string& filename) {
    for (const auto& error : parseErrors) {
        reportError(ErrorType::SYNTAX, filename, 
                   error.position.line, error.position.column, 
                   error.message);
    }
}

void ErrorReporter::reportSemanticErrors(const std::vector<SemanticError>& semanticErrors, 
                                        const std::string& filename) {
    for (const auto& error : semanticErrors) {
        reportError(ErrorType::SEMANTIC, filename, 
                   error.position.line, error.position.column, 
                   error.message);
    }
}

void ErrorReporter::printSummary() const {
    if (errors.empty() && warnings.empty()) {
        std::cout << "Compilation successful with no errors or warnings.\n";
        return;
    }
    
    if (!errors.empty()) {
        std::cout << "Compilation failed with " << errors.size() << " error(s)";
        if (!warnings.empty()) {
            std::cout << " and " << warnings.size() << " warning(s)";
        }
        std::cout << ".\n";
    } else {
        std::cout << "Compilation successful with " << warnings.size() << " warning(s).\n";
    }
}

void ErrorReporter::printErrors() const {
    for (const auto& error : errors) {
        std::cerr << error.toString() << std::endl;
        printSourceContext(error.filename, error.line, error.column);
        std::cerr << std::endl;
    }
}

void ErrorReporter::printWarnings() const {
    for (const auto& warning : warnings) {
        std::cout << "Warning: " << warning.toString() << std::endl;
        printSourceContext(warning.filename, warning.line, warning.column);
        std::cout << std::endl;
    }
}

void ErrorReporter::clear() {
    errors.clear();
    warnings.clear();
    hasErrorsFlag = false;
}

void ErrorReporter::addError(const CompilerError& error) {
    errors.push_back(error);
    hasErrorsFlag = true;
}

void ErrorReporter::addWarning(const CompilerError& warning) {
    warnings.push_back(warning);
}

std::string getSourceLine(const std::string& filename, int lineNumber) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::string line;
    int currentLine = 1;
    
    while (std::getline(file, line) && currentLine <= lineNumber) {
        if (currentLine == lineNumber) {
            return line;
        }
        currentLine++;
    }
    
    return "";
}

void printSourceContext(const std::string& filename, int line, int column) {
    std::string sourceLine = getSourceLine(filename, line);
    if (sourceLine.empty()) {
        return;
    }
    
    std::cout << std::setw(5) << line << " | " << sourceLine << std::endl;
    std::cout << std::setw(5) << " " << " | ";
    
    for (int i = 1; i < column; i++) {
        std::cout << " ";
    }
    std::cout << "^" << std::endl;
}

} // namespace SimpleLang