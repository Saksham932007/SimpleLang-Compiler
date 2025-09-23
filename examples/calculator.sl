// Simple calculator with basic operations
function add(a: int, b: int): int {
    return a + b;
}

function subtract(a: int, b: int): int {
    return a - b;
}

function multiply(a: int, b: int): int {
    return a * b;
}

function divide(a: int, b: int): int {
    if (b == 0) {
        print("Error: Division by zero!");
        return 0;
    }
    return a / b;
}

function main(): int {
    var x: int = 15;
    var y: int = 3;
    
    print("Calculator Demo:");
    print("x = " + string(x) + ", y = " + string(y));
    
    var sum: int = add(x, y);
    print("x + y = " + string(sum));
    
    var diff: int = subtract(x, y);
    print("x - y = " + string(diff));
    
    var product: int = multiply(x, y);
    print("x * y = " + string(product));
    
    var quotient: int = divide(x, y);
    print("x / y = " + string(quotient));
    
    return 0;
}