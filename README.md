# SimpleLang Compiler

A full-featured compiler for the SimpleLang programming language, built from scratch with all essential compiler phases.

## Features

- **Complete Compiler Pipeline**: Lexical analysis, parsing, semantic analysis, and code generation
- **Rich Language**: Support for functions, variables, control flow, and basic data types
- **Error Handling**: Comprehensive error reporting with line numbers and suggestions
- **Optimization**: Basic optimization passes for generated code
- **Testing**: Extensive test suite with example programs
- **Documentation**: Complete language specification and user guide

## Language Features

- Static typing with type checking
- Functions with parameters and return values
- Control flow: if/else, while, for loops
- Basic data types: int, float, bool, string
- Arithmetic and logical expressions
- Built-in functions for I/O operations

## Quick Start

```bash
# Build the compiler
make build

# Compile a SimpleLang program
./compiler input.sl -o output

# Run the compiled program
./output
```

## Project Structure

```
compiler/
├── src/                    # Source code
│   ├── lexer/             # Lexical analysis
│   ├── parser/            # Syntax analysis
│   ├── ast/               # Abstract Syntax Tree
│   ├── semantic/          # Semantic analysis
│   ├── codegen/           # Code generation
│   ├── runtime/           # Runtime support
│   └── utils/             # Utilities
├── tests/                 # Test suite
├── examples/              # Example programs
├── docs/                  # Documentation
└── tools/                 # Development tools
```

## Building

Requires: C++17 or later, Make

```bash
make clean
make build
make test
```

## Examples

See the `examples/` directory for sample SimpleLang programs demonstrating language features.

## Documentation

- [Language Specification](LANGUAGE_SPEC.md)
- [User Guide](docs/USER_GUIDE.md)
- [Developer Guide](docs/DEVELOPER_GUIDE.md)

## License

MIT License - see LICENSE file for details.