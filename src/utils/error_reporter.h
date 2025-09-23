#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace SimpleLang {

enum class ErrorType {
    LEXICAL,
    SYNTAX,
    SEMANTIC,
    CODEGEN,
    IO,
    INTERNAL
};

class CompilerError {
public:
    ErrorType type;
    std::string filename;
    int line;
    int column;
    std::string message;
    std::string hint;
    
    CompilerError(ErrorType t, const std::string& file, int l, int c, 
                 const std::string& msg, const std::string& h = "")
        : type(t), filename(file), line(l), column(c), message(msg), hint(h) {}
    
    std::string toString() const;
    std::string getTypeString() const;
};

class ErrorReporter {
private:
    std::vector<CompilerError> errors;
    std::vector<CompilerError> warnings;
    bool hasErrorsFlag;
    
public:
    ErrorReporter() : hasErrorsFlag(false) {}
    
    void reportError(ErrorType type, const std::string& filename, 
                    int line, int column, const std::string& message, 
                    const std::string& hint = "");
    
    void reportWarning(ErrorType type, const std::string& filename, 
                      int line, int column, const std::string& message, 
                      const std::string& hint = "");
    
    void reportLexerErrors(const std::vector<class LexerError>& lexerErrors, 
                          const std::string& filename);
    
    void reportParserErrors(const std::vector<class ParseError>& parseErrors, 
                           const std::string& filename);
    
    void reportSemanticErrors(const std::vector<class SemanticError>& semanticErrors, 
                             const std::string& filename);
    
    bool hasErrors() const { return hasErrorsFlag; }
    size_t getErrorCount() const { return errors.size(); }
    size_t getWarningCount() const { return warnings.size(); }
    
    void printSummary() const;
    void printErrors() const;
    void printWarnings() const;
    void clear();
    
private:
    void addError(const CompilerError& error);
    void addWarning(const CompilerError& warning);
};

// Utility functions for error handling
std::string getSourceLine(const std::string& filename, int lineNumber);
void printSourceContext(const std::string& filename, int line, int column);

} // namespace SimpleLang