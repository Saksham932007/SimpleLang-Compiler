# SimpleLang User Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Getting Started](#getting-started)
4. [Language Features](#language-features)
5. [Compiler Usage](#compiler-usage)
6. [Examples](#examples)
7. [Troubleshooting](#troubleshooting)

## Introduction

SimpleLang is a statically-typed programming language designed for learning compiler construction. It features:

- **Simple Syntax**: Easy to read and write
- **Static Typing**: Catch errors at compile time
- **Functions**: First-class function support
- **Control Flow**: If/else, while loops, for loops
- **Built-in Types**: int, float, bool, string
- **Error Reporting**: Clear error messages with line numbers

## Installation

### Prerequisites
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- Make build system

### Building from Source

1. Clone or download the SimpleLang compiler source code
2. Navigate to the compiler directory
3. Build the compiler:

```bash
make build
```

4. (Optional) Install system-wide:

```bash
sudo make install
```

### Verify Installation

```bash
./bin/compiler --help
```

## Getting Started

### Your First Program

Create a file called `hello.sl`:

```simplelang
function main(): int {
    print("Hello, SimpleLang!");
    return 0;
}
```

Compile and run:

```bash
./bin/compiler hello.sl
gcc hello.c -o hello
./hello
```

### Basic Syntax

#### Variables
```simplelang
var x: int = 42;
var name: string = "SimpleLang";
var pi: float = 3.14159;
var isValid: bool = true;
```

#### Functions
```simplelang
function add(a: int, b: int): int {
    return a + b;
}

function greet(name: string): void {
    print("Hello, " + name + "!");
}
```

#### Control Flow
```simplelang
// If-else
if (x > 0) {
    print("Positive");
} else {
    print("Non-positive");
}

// While loop
while (x > 0) {
    x = x - 1;
}

// For loop
for (var i: int = 0; i < 10; i = i + 1) {
    print(string(i));
}
```

## Language Features

### Data Types

| Type | Description | Example |
|------|-------------|---------|
| `int` | 32-bit signed integer | `42`, `-17` |
| `float` | 64-bit floating point | `3.14`, `-2.5` |
| `bool` | Boolean value | `true`, `false` |
| `string` | String literal | `"Hello"` |
| `void` | No return value | Function return type only |

### Operators

#### Arithmetic
- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `%` - Modulo

#### Comparison
- `==` - Equal
- `!=` - Not equal
- `<` - Less than
- `<=` - Less than or equal
- `>` - Greater than
- `>=` - Greater than or equal

#### Logical
- `&&` - Logical AND
- `||` - Logical OR
- `!` - Logical NOT

### Functions

Functions are declared with the `function` keyword:

```simplelang
function functionName(param1: type1, param2: type2): returnType {
    // function body
    return value;
}
```

#### Recursive Functions
```simplelang
function factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
```

### Control Structures

#### Conditional Statements
```simplelang
if (condition) {
    // statements
} else if (anotherCondition) {
    // statements
} else {
    // statements
}
```

#### Loops
```simplelang
// While loop
while (condition) {
    // statements
}

// For loop
for (initialization; condition; increment) {
    // statements
}
```

### Built-in Functions

#### I/O Functions
- `print(message: string)` - Print a string to stdout

#### Type Conversion
- `string(value: int)` - Convert integer to string
- `string(value: float)` - Convert float to string

## Compiler Usage

### Basic Usage
```bash
./bin/compiler [options] <input_file>
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `-o <file>` | Specify output file |
| `-f <format>` | Output format: `c`, `asm`, `ir` |
| `-v, --verbose` | Verbose output |
| `--print-ast` | Print Abstract Syntax Tree |
| `--print-ir` | Print Intermediate Representation |
| `-O<level>` | Optimization level (0-3) |
| `-h, --help` | Show help message |

### Examples

#### Compile to C code:
```bash
./bin/compiler -f c -o output.c program.sl
```

#### Generate assembly:
```bash
./bin/compiler -f asm -o output.s program.sl
```

#### Verbose compilation with AST:
```bash
./bin/compiler --verbose --print-ast program.sl
```

#### Optimized compilation:
```bash
./bin/compiler -O2 program.sl
```

## Examples

### Example 1: Simple Calculator
```simplelang
function add(a: int, b: int): int {
    return a + b;
}

function main(): int {
    var x: int = 10;
    var y: int = 5;
    var result: int = add(x, y);
    
    print("Result: " + string(result));
    return 0;
}
```

### Example 2: Fibonacci Sequence
```simplelang
function fibonacci(n: int): int {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

function main(): int {
    for (var i: int = 0; i < 10; i = i + 1) {
        var fib: int = fibonacci(i);
        print("F(" + string(i) + ") = " + string(fib));
    }
    return 0;
}
```

### Example 3: Prime Number Checker
```simplelang
function isPrime(n: int): bool {
    if (n <= 1) {
        return false;
    }
    
    for (var i: int = 2; i * i <= n; i = i + 1) {
        if (n % i == 0) {
            return false;
        }
    }
    
    return true;
}

function main(): int {
    for (var num: int = 2; num <= 20; num = num + 1) {
        if (isPrime(num)) {
            print(string(num) + " is prime");
        }
    }
    return 0;
}
```

## Troubleshooting

### Common Errors

#### Syntax Errors
```
Error: Expected ';' after variable declaration
```
- **Solution**: Add missing semicolons after statements

#### Type Errors
```
Error: Cannot convert bool to int
```
- **Solution**: Use explicit type conversion or fix the type mismatch

#### Undefined Variables
```
Error: Undefined variable 'x'
```
- **Solution**: Declare the variable before using it

#### Function Errors
```
Error: Function 'foo' already declared
```
- **Solution**: Use unique function names

### Debugging Tips

1. **Use Verbose Mode**: Add `--verbose` to see compilation steps
2. **Print AST**: Use `--print-ast` to visualize the parsed structure
3. **Check Types**: Ensure all variables have correct types
4. **Verify Syntax**: Check for missing semicolons, braces, etc.

### Getting Help

1. **Check Examples**: Look at the example programs in `examples/`
2. **Read Error Messages**: Error messages include line numbers and hints
3. **Use Help**: Run `./bin/compiler --help` for usage information

## Advanced Features

### Multiple Files
Currently, SimpleLang supports single-file programs. Future versions will include module support.

### Standard Library
The compiler includes basic built-in functions. Additional library functions will be added in future releases.

### Optimization
The compiler supports basic optimization levels:
- `-O0`: No optimization (default)
- `-O1`: Basic optimization
- `-O2`: Standard optimization
- `-O3`: Aggressive optimization

---

For more information, see the [Language Specification](../LANGUAGE_SPEC.md) and [Developer Guide](DEVELOPER_GUIDE.md).