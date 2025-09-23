#!/bin/bash

# Integration tests for SimpleLang Compiler

set -e

COMPILER="../bin/compiler"
EXAMPLES_DIR="../examples"
OUTPUT_DIR="output"

# Create output directory
mkdir -p $OUTPUT_DIR

echo "Running integration tests..."

# Test 1: Hello World
echo "Test 1: Hello World"
$COMPILER -f c -o $OUTPUT_DIR/hello_world.c $EXAMPLES_DIR/hello_world.sl
echo "✓ Hello World compiled successfully"

# Test 2: Factorial
echo "Test 2: Factorial"
$COMPILER -f c -o $OUTPUT_DIR/factorial.c $EXAMPLES_DIR/factorial.sl
echo "✓ Factorial compiled successfully"

# Test 3: Fibonacci
echo "Test 3: Fibonacci"
$COMPILER -f c -o $OUTPUT_DIR/fibonacci.c $EXAMPLES_DIR/fibonacci.sl
echo "✓ Fibonacci compiled successfully"

# Test 4: Calculator
echo "Test 4: Calculator"
$COMPILER -f c -o $OUTPUT_DIR/calculator.c $EXAMPLES_DIR/calculator.sl
echo "✓ Calculator compiled successfully"

# Test 5: Control Flow
echo "Test 5: Control Flow"
$COMPILER -f c -o $OUTPUT_DIR/control_flow.c $EXAMPLES_DIR/control_flow.sl
echo "✓ Control Flow compiled successfully"

# Test 6: Assembly output
echo "Test 6: Assembly Output"
$COMPILER -f asm -o $OUTPUT_DIR/hello_world.s $EXAMPLES_DIR/hello_world.sl
echo "✓ Assembly output generated successfully"

# Test 7: IR output
echo "Test 7: Intermediate Representation"
$COMPILER -f ir -o $OUTPUT_DIR/hello_world.ir $EXAMPLES_DIR/hello_world.sl
echo "✓ IR output generated successfully"

# Test 8: Verbose and AST printing
echo "Test 8: Verbose Output and AST"
$COMPILER --verbose --print-ast $EXAMPLES_DIR/hello_world.sl > $OUTPUT_DIR/verbose_output.txt 2>&1
echo "✓ Verbose output and AST generated successfully"

# Test 9: Error handling
echo "Test 9: Error Handling"
cat > $OUTPUT_DIR/error_test.sl << 'EOF'
function main(): int {
    var x: int = undefined_variable; // This should cause an error
    return x;
}
EOF

if $COMPILER $OUTPUT_DIR/error_test.sl 2> $OUTPUT_DIR/error_output.txt; then
    echo "✗ Error test failed - should have reported an error"
    exit 1
else
    echo "✓ Error handling works correctly"
fi

# Test 10: Syntax error handling
echo "Test 10: Syntax Error Handling"
cat > $OUTPUT_DIR/syntax_error_test.sl << 'EOF'
function main(): int {
    var x: int = ; // Syntax error
    return x
} // Missing semicolon
EOF

if $COMPILER $OUTPUT_DIR/syntax_error_test.sl 2> $OUTPUT_DIR/syntax_error_output.txt; then
    echo "✗ Syntax error test failed - should have reported syntax errors"
    exit 1
else
    echo "✓ Syntax error handling works correctly"
fi

echo ""
echo "All integration tests passed! ✓"
echo ""
echo "Generated files in $OUTPUT_DIR:"
ls -la $OUTPUT_DIR/