# SimpleLang Language Specification

## Overview
SimpleLang is a statically-typed, imperative programming language designed for learning compiler construction. It supports basic data types, functions, control flow, and modular programming.

## Grammar (EBNF)

```ebnf
program = { declaration } ;

declaration = function_declaration 
            | variable_declaration ;

function_declaration = "function" identifier "(" [ parameter_list ] ")" [ ":" type ] block ;

parameter_list = parameter { "," parameter } ;
parameter = identifier ":" type ;

variable_declaration = "var" identifier [ ":" type ] [ "=" expression ] ";" ;

block = "{" { statement } "}" ;

statement = expression_statement
          | assignment_statement
          | if_statement
          | while_statement
          | for_statement
          | return_statement
          | block ;

expression_statement = expression ";" ;
assignment_statement = identifier "=" expression ";" ;
if_statement = "if" "(" expression ")" statement [ "else" statement ] ;
while_statement = "while" "(" expression ")" statement ;
for_statement = "for" "(" [ variable_declaration | assignment_statement ] ";" 
                      [ expression ] ";" 
                      [ assignment_statement ] ")" statement ;
return_statement = "return" [ expression ] ";" ;

expression = logical_or ;
logical_or = logical_and { "||" logical_and } ;
logical_and = equality { "&&" equality } ;
equality = comparison { ( "==" | "!=" ) comparison } ;
comparison = addition { ( "<" | "<=" | ">" | ">=" ) addition } ;
addition = multiplication { ( "+" | "-" ) multiplication } ;
multiplication = unary { ( "*" | "/" | "%" ) unary } ;
unary = ( "!" | "-" | "+" ) unary | postfix ;
postfix = primary [ "(" [ argument_list ] ")" ] ;
primary = identifier | literal | "(" expression ")" ;

argument_list = expression { "," expression } ;

type = "int" | "float" | "bool" | "string" | "void" ;

literal = integer_literal | float_literal | string_literal | boolean_literal ;
boolean_literal = "true" | "false" ;

identifier = letter { letter | digit | "_" } ;
integer_literal = digit { digit } ;
float_literal = digit { digit } "." digit { digit } ;
string_literal = '"' { character } '"' ;

letter = "a" | "b" | ... | "z" | "A" | "B" | ... | "Z" ;
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
```

## Keywords
- `function` - Function declaration
- `var` - Variable declaration
- `if`, `else` - Conditional statements
- `while`, `for` - Loop statements
- `return` - Return statement
- `int`, `float`, `bool`, `string`, `void` - Basic types
- `true`, `false` - Boolean literals

## Operators
### Arithmetic
- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `%` - Modulo

### Comparison
- `==` - Equal
- `!=` - Not equal
- `<` - Less than
- `<=` - Less than or equal
- `>` - Greater than
- `>=` - Greater than or equal

### Logical
- `&&` - Logical AND
- `||` - Logical OR
- `!` - Logical NOT

### Assignment
- `=` - Assignment

## Type System
- `int` - 32-bit signed integers
- `float` - 64-bit floating point numbers
- `bool` - Boolean values (true/false)
- `string` - String literals
- `void` - No return value (functions only)

## Example Program

```simplelang
// Factorial function
function factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// Main function
function main(): int {
    var num: int = 5;
    var result: int = factorial(num);
    
    // Print result (built-in function)
    print("Factorial of " + string(num) + " is " + string(result));
    
    return 0;
}
```

## Semantic Rules
1. All variables must be declared before use
2. Function overloading is not supported
3. Type checking is performed at compile time
4. Automatic type conversion between int and float
5. String concatenation using `+` operator
6. Functions must return a value of declared type (except void)
7. Main function serves as program entry point