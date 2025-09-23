#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "codegen/code_generator.h"
#include "utils/error_reporter.h"
#include "ast/ast_visitor.h"

using namespace SimpleLang;

struct CompilerOptions {
    std::string inputFile;
    std::string outputFile;
    std::string outputFormat = "c"; // c, asm, ir
    bool verbose = false;
    bool printAST = false;
    bool printIR = false;
    bool optimizeCode = false;
    int optimizationLevel = 0;
    
    void printHelp() const {
        std::cout << "SimpleLang Compiler v1.0\n";
        std::cout << "Usage: compiler [options] <input_file>\n\n";
        std::cout << "Options:\n";
        std::cout << "  -o <file>        Output file (default: a.out)\n";
        std::cout << "  -f <format>      Output format: c, asm, ir (default: c)\n";
        std::cout << "  -v, --verbose    Verbose output\n";
        std::cout << "  --print-ast      Print Abstract Syntax Tree\n";
        std::cout << "  --print-ir       Print Intermediate Representation\n";
        std::cout << "  -O<level>        Optimization level (0-3, default: 0)\n";
        std::cout << "  -h, --help       Show this help message\n";
        std::cout << "\nOutput formats:\n";
        std::cout << "  c                Generate C code\n";
        std::cout << "  asm              Generate assembly code\n";
        std::cout << "  ir               Generate intermediate representation\n";
        std::cout << "\nExamples:\n";
        std::cout << "  compiler program.sl\n";
        std::cout << "  compiler -o output.c -f c program.sl\n";
        std::cout << "  compiler --print-ast -v program.sl\n";
    }
};

bool parseArguments(int argc, char* argv[], CompilerOptions& options) {
    if (argc < 2) {
        options.printHelp();
        return false;
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options.printHelp();
            return false;
        } else if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--print-ast") {
            options.printAST = true;
        } else if (arg == "--print-ir") {
            options.printIR = true;
        } else if (arg == "-o" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "-f" && i + 1 < argc) {
            options.outputFormat = argv[++i];
        } else if (arg.substr(0, 2) == "-O" && arg.length() == 3) {
            options.optimizationLevel = arg[2] - '0';
            options.optimizeCode = (options.optimizationLevel > 0);
        } else if (arg[0] != '-') {
            options.inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return false;
        }
    }
    
    if (options.inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        return false;
    }
    
    if (options.outputFile.empty()) {
        if (options.outputFormat == "c") {
            options.outputFile = "a.c";
        } else if (options.outputFormat == "asm") {
            options.outputFile = "a.s";
        } else {
            options.outputFile = "a.out";
        }
    }
    
    return true;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }
    
    file << content;
}

int main(int argc, char* argv[]) {
    CompilerOptions options;
    ErrorReporter errorReporter;
    
    // Parse command line arguments
    if (!parseArguments(argc, argv, options)) {
        return 1;
    }
    
    try {
        if (options.verbose) {
            std::cout << "Compiling: " << options.inputFile << std::endl;
        }
        
        // Read source file
        std::string source = readFile(options.inputFile);
        
        // Lexical Analysis
        if (options.verbose) {
            std::cout << "Lexical analysis..." << std::endl;
        }
        
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        if (lexer.hasErrors()) {
            errorReporter.reportLexerErrors(lexer.getErrors(), options.inputFile);
            errorReporter.printErrors();
            errorReporter.printSummary();
            return 1;
        }
        
        // Syntax Analysis
        if (options.verbose) {
            std::cout << "Syntax analysis..." << std::endl;
        }
        
        Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        if (parser.hasErrors()) {
            errorReporter.reportParserErrors(parser.getErrors(), options.inputFile);
            errorReporter.printErrors();
            errorReporter.printSummary();
            return 1;
        }
        
        // Print AST if requested
        if (options.printAST) {
            std::cout << "\n=== Abstract Syntax Tree ===" << std::endl;
            ASTPrinter printer;
            std::cout << printer.print(*ast) << std::endl;
        }
        
        // Semantic Analysis
        if (options.verbose) {
            std::cout << "Semantic analysis..." << std::endl;
        }
        
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(*ast);
        
        if (semanticAnalyzer.hasErrors()) {
            errorReporter.reportSemanticErrors(semanticAnalyzer.getErrors(), options.inputFile);
            errorReporter.printErrors();
            errorReporter.printSummary();
            return 1;
        }
        
        // Code Generation
        if (options.verbose) {
            std::cout << "Code generation..." << std::endl;
        }
        
        CodeGenerator codeGenerator;
        auto instructions = codeGenerator.generate(*ast);
        
        // Print IR if requested
        if (options.printIR) {
            std::cout << "\n=== Intermediate Representation ===" << std::endl;
            for (const auto& inst : instructions) {
                std::cout << inst.arg1 << " " << inst.arg2 << " -> " << inst.result << std::endl;
            }
        }
        
        // Generate output based on format
        std::string output;
        if (options.outputFormat == "c") {
            output = codeGenerator.generateC(instructions);
        } else if (options.outputFormat == "asm") {
            output = codeGenerator.generateAssembly(instructions);
        } else if (options.outputFormat == "ir") {
            std::ostringstream oss;
            for (const auto& inst : instructions) {
                oss << inst.arg1 << " " << inst.arg2 << " -> " << inst.result << "\n";
            }
            output = oss.str();
        } else {
            std::cerr << "Unknown output format: " << options.outputFormat << std::endl;
            return 1;
        }
        
        // Write output file
        writeFile(options.outputFile, output);
        
        if (options.verbose) {
            std::cout << "Output written to: " << options.outputFile << std::endl;
        }
        
        errorReporter.printSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}