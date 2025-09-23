// Fibonacci sequence generator
function fibonacci(n: int): int {
    if (n <= 1) {
        return n;
    }
    
    var a: int = 0;
    var b: int = 1;
    var i: int = 2;
    
    while (i <= n) {
        var temp: int = a + b;
        a = b;
        b = temp;
        i = i + 1;
    }
    
    return b;
}

function main(): int {
    var i: int = 0;
    
    print("Fibonacci sequence:");
    
    for (i = 0; i < 10; i = i + 1) {
        var fib: int = fibonacci(i);
        print("F(" + string(i) + ") = " + string(fib));
    }
    
    return 0;
}