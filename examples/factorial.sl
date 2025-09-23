// Factorial calculation example
function factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

function main(): int {
    var num: int = 5;
    var result: int = factorial(num);
    
    print("Factorial of 5 is: " + string(result));
    
    return 0;
}