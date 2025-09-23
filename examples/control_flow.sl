// Control flow and conditional statements
function isEven(n: int): bool {
    return (n % 2) == 0;
}

function max(a: int, b: int): int {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

function printNumbers(limit: int): void {
    var i: int = 1;
    
    while (i <= limit) {
        if (isEven(i)) {
            print(string(i) + " is even");
        } else {
            print(string(i) + " is odd");
        }
        i = i + 1;
    }
}

function main(): int {
    var a: int = 10;
    var b: int = 15;
    
    print("Control Flow Demo:");
    
    var maximum: int = max(a, b);
    print("Max of " + string(a) + " and " + string(b) + " is " + string(maximum));
    
    print("Numbers 1 to 5:");
    printNumbers(5);
    
    // For loop example
    print("Countdown:");
    for (var count: int = 5; count > 0; count = count - 1) {
        print(string(count) + "...");
    }
    print("Blast off!");
    
    return 0;
}