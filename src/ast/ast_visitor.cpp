#include "ast_visitor.h"
#include <sstream>

namespace SimpleLang {

// ASTPrinter implementation
void ASTPrinter::indent() {
    for (int i = 0; i < indentLevel; i++) {
        result += "  ";
    }
}

void ASTPrinter::print(const std::string& text) {
    result += text;
}

void ASTPrinter::println(const std::string& text) {
    result += text + "\n";
}

std::string ASTPrinter::print(ASTNode& node) {
    result.clear();
    indentLevel = 0;
    node.accept(*this);
    return result;
}

void ASTPrinter::visitIntegerLiteral(IntegerLiteral& node) {
    print(std::to_string(node.value));
}

void ASTPrinter::visitFloatLiteral(FloatLiteral& node) {
    print(std::to_string(node.value));
}

void ASTPrinter::visitStringLiteral(StringLiteral& node) {
    print("\"" + node.value + "\"");
}

void ASTPrinter::visitBooleanLiteral(BooleanLiteral& node) {
    print(node.value ? "true" : "false");
}

void ASTPrinter::visitIdentifier(Identifier& node) {
    print(node.name);
}

void ASTPrinter::visitBinaryExpression(BinaryExpression& node) {
    print("(");
    node.left->accept(*this);
    print(" " + binaryOpToString(node.operator_) + " ");
    node.right->accept(*this);
    print(")");
}

void ASTPrinter::visitUnaryExpression(UnaryExpression& node) {
    print("(" + unaryOpToString(node.operator_));
    node.operand->accept(*this);
    print(")");
}

void ASTPrinter::visitFunctionCall(FunctionCall& node) {
    print(node.functionName + "(");
    for (size_t i = 0; i < node.arguments.size(); i++) {
        if (i > 0) print(", ");
        node.arguments[i]->accept(*this);
    }
    print(")");
}

void ASTPrinter::visitExpressionStatement(ExpressionStatement& node) {
    indent();
    node.expression->accept(*this);
    println(";");
}

void ASTPrinter::visitVariableDeclaration(VariableDeclaration& node) {
    indent();
    print("var " + node.name + ": " + node.type.name);
    if (node.initializer) {
        print(" = ");
        node.initializer->accept(*this);
    }
    println(";");
}

void ASTPrinter::visitAssignmentStatement(AssignmentStatement& node) {
    indent();
    print(node.variableName + " = ");
    node.value->accept(*this);
    println(";");
}

void ASTPrinter::visitBlockStatement(BlockStatement& node) {
    println("{");
    indentLevel++;
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    indentLevel--;
    indent();
    println("}");
}

void ASTPrinter::visitIfStatement(IfStatement& node) {
    indent();
    print("if (");
    node.condition->accept(*this);
    println(")");
    indentLevel++;
    node.thenStatement->accept(*this);
    indentLevel--;
    
    if (node.elseStatement) {
        indent();
        println("else");
        indentLevel++;
        node.elseStatement->accept(*this);
        indentLevel--;
    }
}

void ASTPrinter::visitWhileStatement(WhileStatement& node) {
    indent();
    print("while (");
    node.condition->accept(*this);
    println(")");
    indentLevel++;
    node.body->accept(*this);
    indentLevel--;
}

void ASTPrinter::visitForStatement(ForStatement& node) {
    indent();
    print("for (");
    if (node.initializer) {
        node.initializer->accept(*this);
        result.pop_back(); // Remove the newline from the initializer
        result.pop_back(); // Remove the last character (should be ';')
    }
    print("; ");
    if (node.condition) {
        node.condition->accept(*this);
    }
    print("; ");
    if (node.increment) {
        // Similar handling for increment
    }
    println(")");
    indentLevel++;
    node.body->accept(*this);
    indentLevel--;
}

void ASTPrinter::visitReturnStatement(ReturnStatement& node) {
    indent();
    print("return");
    if (node.value) {
        print(" ");
        node.value->accept(*this);
    }
    println(";");
}

void ASTPrinter::visitFunctionDeclaration(FunctionDeclaration& node) {
    indent();
    print("function " + node.name + "(");
    for (size_t i = 0; i < node.parameters.size(); i++) {
        if (i > 0) print(", ");
        print(node.parameters[i].name + ": " + node.parameters[i].type.name);
    }
    print("): " + node.returnType.name + " ");
    node.body->accept(*this);
}

void ASTPrinter::visitProgram(Program& node) {
    println("Program:");
    indentLevel++;
    
    for (auto& globalVar : node.globalVariables) {
        globalVar->accept(*this);
    }
    
    for (auto& function : node.functions) {
        function->accept(*this);
    }
    
    indentLevel--;
}

std::string ASTPrinter::binaryOpToString(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::ADD: return "+";
        case BinaryOperator::SUBTRACT: return "-";
        case BinaryOperator::MULTIPLY: return "*";
        case BinaryOperator::DIVIDE: return "/";
        case BinaryOperator::MODULO: return "%";
        case BinaryOperator::EQUAL: return "==";
        case BinaryOperator::NOT_EQUAL: return "!=";
        case BinaryOperator::LESS_THAN: return "<";
        case BinaryOperator::LESS_EQUAL: return "<=";
        case BinaryOperator::GREATER_THAN: return ">";
        case BinaryOperator::GREATER_EQUAL: return ">=";
        case BinaryOperator::AND: return "&&";
        case BinaryOperator::OR: return "||";
        default: return "?";
    }
}

std::string ASTPrinter::unaryOpToString(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::MINUS: return "-";
        case UnaryOperator::PLUS: return "+";
        case UnaryOperator::NOT: return "!";
        default: return "?";
    }
}

} // namespace SimpleLang