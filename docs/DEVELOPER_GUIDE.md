# SimpleLang Developer Guide

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Compiler Phases](#compiler-phases)
3. [Code Organization](#code-organization)
4. [Building and Testing](#building-and-testing)
5. [Extending the Language](#extending-the-language)
6. [Debugging the Compiler](#debugging-the-compiler)
7. [Contributing](#contributing)

## Architecture Overview

The SimpleLang compiler follows a traditional multi-pass architecture:

```
Source Code → Lexer → Parser → Semantic Analyzer → Code Generator → Output
```

### Key Components

1. **Lexer**: Converts source code into tokens
2. **Parser**: Builds Abstract Syntax Tree (AST) from tokens
3. **Semantic Analyzer**: Type checking and semantic validation
4. **Code Generator**: Produces target code (C, Assembly, or IR)
5. **Error Reporter**: Handles error reporting across all phases

## Compiler Phases

### 1. Lexical Analysis (`src/lexer/`)

**Purpose**: Convert source code into a stream of tokens

**Key Files**:
- `token.h/cpp` - Token definitions and utilities
- `lexer.h/cpp` - Main lexer implementation

**Process**:
1. Read source code character by character
2. Recognize patterns (keywords, identifiers, literals, operators)
3. Generate tokens with position information
4. Handle lexical errors (invalid characters, unterminated strings)

**Example**:
```cpp
Lexer lexer(sourceCode);
std::vector<Token> tokens = lexer.tokenize();
```

### 2. Syntax Analysis (`src/parser/`)

**Purpose**: Build Abstract Syntax Tree from tokens

**Key Files**:
- `parser.h/cpp` - Recursive descent parser

**Process**:
1. Use recursive descent parsing
2. Handle operator precedence and associativity
3. Build AST nodes for all language constructs
4. Provide error recovery for syntax errors

**Grammar Rules**:
- Expression parsing with precedence climbing
- Statement parsing with context-sensitive decisions
- Declaration parsing for functions and variables

### 3. Abstract Syntax Tree (`src/ast/`)

**Purpose**: Represent program structure in memory

**Key Files**:
- `ast_nodes.h/cpp` - AST node definitions
- `ast_visitor.h/cpp` - Visitor pattern implementation

**Design Patterns**:
- **Visitor Pattern**: For tree traversal operations
- **Composite Pattern**: For hierarchical node structure
- **Factory Pattern**: For node creation

### 4. Semantic Analysis (`src/semantic/`)

**Purpose**: Type checking and semantic validation

**Key Files**:
- `semantic_analyzer.h/cpp` - Main semantic analyzer

**Features**:
- Symbol table management with scoping
- Type checking and inference
- Function signature validation
- Variable initialization tracking
- Semantic error reporting

**Symbol Table**:
```cpp
class Scope {
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;
    // ...
};
```

### 5. Code Generation (`src/codegen/`)

**Purpose**: Generate target code from AST

**Key Files**:
- `code_generator.h/cpp` - Main code generator

**Supported Targets**:
- **C Code**: High-level readable output
- **Assembly**: x86-64 assembly code
- **IR**: Intermediate representation

**Process**:
1. Traverse AST using visitor pattern
2. Generate intermediate instructions
3. Perform register allocation (basic)
4. Output target-specific code

## Code Organization

### Directory Structure
```
src/
├── lexer/          # Lexical analysis
├── parser/         # Syntax analysis
├── ast/            # Abstract Syntax Tree
├── semantic/       # Semantic analysis
├── codegen/        # Code generation
├── utils/          # Utilities and error handling
└── main.cpp        # CLI interface

tests/              # Test suite
├── test_runner.cpp # Unit tests
└── integration_tests.sh # Integration tests

examples/           # Example programs
docs/              # Documentation
tools/             # Development tools
```

### Key Design Principles

1. **Single Responsibility**: Each class has one clear purpose
2. **Visitor Pattern**: For AST traversal operations
3. **Error Recovery**: Graceful handling of errors at each phase
4. **Extensibility**: Easy to add new language features
5. **Separation of Concerns**: Clear boundaries between phases

## Building and Testing

### Build System

The project uses Make for building:

```makefile
# Main targets
build:     # Build the compiler
test:      # Run all tests
clean:     # Clean build files
debug:     # Build with debug symbols
release:   # Build optimized release
```

### Build Process

1. **Compile Sources**: Each .cpp file compiled to .o
2. **Link Executable**: All object files linked together
3. **Create Binary**: Place in `bin/` directory

### Testing Framework

#### Unit Tests (`tests/test_runner.cpp`)
- Tests individual components
- Uses custom assertion framework
- Covers all major functionality

#### Integration Tests (`tests/integration_tests.sh`)
- Tests complete compiler pipeline
- Validates example programs
- Tests error handling

#### Running Tests
```bash
make test                    # Run all tests
./bin/test_runner           # Unit tests only
cd tests && ./integration_tests.sh  # Integration tests only
```

## Extending the Language

### Adding New Language Features

#### 1. Add Token Types (`src/lexer/token.h`)
```cpp
enum class TokenType {
    // ... existing tokens ...
    NEW_KEYWORD,
    NEW_OPERATOR
};
```

#### 2. Update Lexer (`src/lexer/lexer.cpp`)
```cpp
// Add keyword to map
keywords["new_keyword"] = TokenType::NEW_KEYWORD;

// Add operator recognition
case 'new_char':
    return Token(TokenType::NEW_OPERATOR, "new_op", startPos);
```

#### 3. Add AST Nodes (`src/ast/ast_nodes.h`)
```cpp
class NewStatement : public Statement {
public:
    // Add fields
    void accept(ASTVisitor& visitor) override;
};
```

#### 4. Update Parser (`src/parser/parser.cpp`)
```cpp
std::unique_ptr<Statement> Parser::newStatement() {
    // Parse new statement type
    return std::make_unique<NewStatement>(/* ... */);
}
```

#### 5. Update Semantic Analyzer
```cpp
void SemanticAnalyzer::visitNewStatement(NewStatement& node) {
    // Add semantic checks
}
```

#### 6. Update Code Generator
```cpp
void CodeGenerator::visitNewStatement(NewStatement& node) {
    // Generate code for new feature
}
```

### Adding Built-in Functions

1. **Add to Semantic Analyzer**:
```cpp
void SemanticAnalyzer::defineBuiltinFunctions() {
    Symbol newFunc("new_function", TypeInfo(Type::INT), Position(), true);
    newFunc.parameterTypes.push_back(TypeInfo(Type::STRING));
    defineSymbol(newFunc);
}
```

2. **Add to Code Generator**:
```cpp
void CodeGenerator::visitFunctionCall(FunctionCall& node) {
    if (node.functionName == "new_function") {
        // Generate code for new built-in
    }
}
```

## Debugging the Compiler

### Debugging Tools

#### 1. Verbose Mode
```bash
./bin/compiler --verbose program.sl
```

#### 2. AST Printing
```bash
./bin/compiler --print-ast program.sl
```

#### 3. IR Printing
```bash
./bin/compiler --print-ir program.sl
```

### Common Issues

#### Memory Management
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Avoid raw pointers where possible
- Check for memory leaks with valgrind

#### Error Handling
- Always check for errors at each phase
- Provide meaningful error messages
- Include position information

#### Performance
- Profile with `gprof` or similar tools
- Optimize hot paths in lexer and parser
- Consider memoization for recursive operations

### Debugging Strategies

1. **Unit Test First**: Write tests for new features
2. **Incremental Development**: Add one feature at a time
3. **Error Messages**: Make them clear and actionable
4. **Code Review**: Use consistent style and patterns

## Contributing

### Development Setup

1. **Clone Repository**: Get the latest source code
2. **Install Dependencies**: Ensure C++17 compiler and Make
3. **Build and Test**: Verify everything works
4. **Create Branch**: Work on separate feature branches

### Code Style

#### Naming Conventions
- **Classes**: PascalCase (`TokenType`, `ASTNode`)
- **Functions**: camelCase (`tokenize`, `parseExpression`)
- **Variables**: camelCase (`currentToken`, `nodeList`)
- **Constants**: UPPER_CASE (`MAX_TOKENS`)

#### File Organization
- **Headers**: `.h` extension, include guards
- **Implementation**: `.cpp` extension
- **One class per file**: Unless closely related

#### Documentation
- **Public APIs**: Document all public methods
- **Complex Logic**: Add inline comments
- **Examples**: Provide usage examples

### Testing Guidelines

1. **Test Coverage**: Aim for high test coverage
2. **Edge Cases**: Test boundary conditions
3. **Error Cases**: Test error handling paths
4. **Integration**: Test complete workflows

### Pull Request Process

1. **Create Feature Branch**: From main branch
2. **Implement Feature**: With tests
3. **Run Tests**: Ensure all tests pass
4. **Submit PR**: With description and examples
5. **Code Review**: Address feedback
6. **Merge**: After approval

### Release Process

1. **Version Bump**: Update version numbers
2. **Run Full Test Suite**: All tests must pass
3. **Generate Documentation**: Update user guides
4. **Create Release**: Tag and package
5. **Announce**: Update changelog

---

For more technical details, see the individual source files and their documentation comments.