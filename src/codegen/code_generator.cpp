#include "code_generator.h"
#include <sstream>
#include <iomanip>

namespace SimpleLang {

CodeGenerator::CodeGenerator() 
    : nextTemp(0), nextLabel(0), currentOffset(0) {}

std::vector<Instruction> CodeGenerator::generate(Program& program) {
    instructions.clear();
    program.accept(*this);
    emit(OpCode::HALT); // End program
    return instructions;
}

void CodeGenerator::visitIntegerLiteral(IntegerLiteral& node) {
    lastResult = newTemp();
    emit(OpCode::LOAD_CONST, std::to_string(node.value), "", lastResult);
}

void CodeGenerator::visitFloatLiteral(FloatLiteral& node) {
    lastResult = newTemp();
    emit(OpCode::LOAD_CONST, std::to_string(node.value), "", lastResult);
}

void CodeGenerator::visitStringLiteral(StringLiteral& node) {
    lastResult = newTemp();
    emit(OpCode::LOAD_CONST, "\"" + escapeString(node.value) + "\"", "", lastResult);
}

void CodeGenerator::visitBooleanLiteral(BooleanLiteral& node) {
    lastResult = newTemp();
    emit(OpCode::LOAD_CONST, node.value ? "1" : "0", "", lastResult);
}

void CodeGenerator::visitIdentifier(Identifier& node) {
    lastResult = newTemp();
    emit(OpCode::LOAD, node.name, "", lastResult);
}

void CodeGenerator::visitBinaryExpression(BinaryExpression& node) {
    node.left->accept(*this);
    std::string leftResult = lastResult;
    
    node.right->accept(*this);
    std::string rightResult = lastResult;
    
    lastResult = newTemp();
    emit(binaryOpToOpCode(node.operator_), leftResult, rightResult, lastResult);
}

void CodeGenerator::visitUnaryExpression(UnaryExpression& node) {
    node.operand->accept(*this);
    std::string operandResult = lastResult;
    
    lastResult = newTemp();
    emit(unaryOpToOpCode(node.operator_), operandResult, "", lastResult);
}

void CodeGenerator::visitFunctionCall(FunctionCall& node) {
    // Push arguments in reverse order
    for (int i = node.arguments.size() - 1; i >= 0; i--) {
        node.arguments[i]->accept(*this);
        emit(OpCode::PARAM, lastResult);
    }
    
    lastResult = newTemp();
    emit(OpCode::CALL, node.functionName, std::to_string(node.arguments.size()), lastResult);
}

void CodeGenerator::visitExpressionStatement(ExpressionStatement& node) {
    node.expression->accept(*this);
    // Result is discarded for expression statements
}

void CodeGenerator::visitVariableDeclaration(VariableDeclaration& node) {
    variables[node.name] = currentOffset++;
    
    if (node.initializer) {
        node.initializer->accept(*this);
        emit(OpCode::STORE, lastResult, "", node.name);
    }
}

void CodeGenerator::visitAssignmentStatement(AssignmentStatement& node) {
    node.value->accept(*this);
    emit(OpCode::STORE, lastResult, "", node.variableName);
}

void CodeGenerator::visitBlockStatement(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void CodeGenerator::visitIfStatement(IfStatement& node) {
    node.condition->accept(*this);
    
    std::string elseLabel = newLabel();
    std::string endLabel = newLabel();
    
    emit(OpCode::JZ, lastResult, elseLabel);
    
    node.thenStatement->accept(*this);
    emit(OpCode::JMP, endLabel);
    
    emitLabel(elseLabel);
    if (node.elseStatement) {
        node.elseStatement->accept(*this);
    }
    
    emitLabel(endLabel);
}

void CodeGenerator::visitWhileStatement(WhileStatement& node) {
    std::string loopLabel = newLabel();
    std::string endLabel = newLabel();
    
    emitLabel(loopLabel);
    node.condition->accept(*this);
    emit(OpCode::JZ, lastResult, endLabel);
    
    node.body->accept(*this);
    emit(OpCode::JMP, loopLabel);
    
    emitLabel(endLabel);
}

void CodeGenerator::visitForStatement(ForStatement& node) {
    std::string loopLabel = newLabel();
    std::string continueLabel = newLabel();
    std::string endLabel = newLabel();
    
    // Initializer
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    
    emitLabel(loopLabel);
    
    // Condition
    if (node.condition) {
        node.condition->accept(*this);
        emit(OpCode::JZ, lastResult, endLabel);
    }
    
    // Body
    node.body->accept(*this);
    
    emitLabel(continueLabel);
    
    // Increment
    if (node.increment) {
        node.increment->accept(*this);
    }
    
    emit(OpCode::JMP, loopLabel);
    
    emitLabel(endLabel);
}

void CodeGenerator::visitReturnStatement(ReturnStatement& node) {
    if (node.value) {
        node.value->accept(*this);
        emit(OpCode::RET, lastResult);
    } else {
        emit(OpCode::RET);
    }
}

void CodeGenerator::visitFunctionDeclaration(FunctionDeclaration& node) {
    std::string funcLabel = "func_" + node.name;
    functions[node.name] = instructions.size();
    
    emitLabel(funcLabel);
    emit(OpCode::ENTER, std::to_string(node.parameters.size()));
    
    // Parameters are loaded by the ENTER instruction
    for (size_t i = 0; i < node.parameters.size(); i++) {
        variables[node.parameters[i].name] = i;
    }
    
    node.body->accept(*this);
    
    // Implicit return for void functions
    if (node.returnType.type == Type::VOID) {
        emit(OpCode::RET);
    }
    
    emit(OpCode::LEAVE);
}

void CodeGenerator::visitProgram(Program& node) {
    // Generate global variables
    for (auto& var : node.globalVariables) {
        var->accept(*this);
    }
    
    // Generate main function call
    emit(OpCode::CALL, "main", "0", newTemp());
    emit(OpCode::HALT);
    
    // Generate functions
    for (auto& func : node.functions) {
        func->accept(*this);
    }
}

std::string CodeGenerator::generateAssembly(const std::vector<Instruction>& instructions) {
    AssemblyGenerator generator;
    return generator.generate(instructions);
}

std::string CodeGenerator::generateC(const std::vector<Instruction>& instructions) {
    CGenerator generator;
    return generator.generate(instructions);
}

// Helper methods
std::string CodeGenerator::newTemp() {
    return "t" + std::to_string(nextTemp++);
}

std::string CodeGenerator::newLabel() {
    return "L" + std::to_string(nextLabel++);
}

void CodeGenerator::emit(OpCode opcode, const std::string& arg1, 
                        const std::string& arg2, const std::string& result) {
    instructions.emplace_back(opcode, arg1, arg2, result);
}

void CodeGenerator::emitLabel(const std::string& label) {
    labelMap[label] = instructions.size();
    emit(OpCode::LABEL, label);
}

OpCode CodeGenerator::binaryOpToOpCode(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::ADD: return OpCode::ADD;
        case BinaryOperator::SUBTRACT: return OpCode::SUB;
        case BinaryOperator::MULTIPLY: return OpCode::MUL;
        case BinaryOperator::DIVIDE: return OpCode::DIV;
        case BinaryOperator::MODULO: return OpCode::MOD;
        case BinaryOperator::EQUAL: return OpCode::EQ;
        case BinaryOperator::NOT_EQUAL: return OpCode::NE;
        case BinaryOperator::LESS_THAN: return OpCode::LT;
        case BinaryOperator::LESS_EQUAL: return OpCode::LE;
        case BinaryOperator::GREATER_THAN: return OpCode::GT;
        case BinaryOperator::GREATER_EQUAL: return OpCode::GE;
        case BinaryOperator::AND: return OpCode::AND;
        case BinaryOperator::OR: return OpCode::OR;
        default: return OpCode::ADD; // fallback
    }
}

OpCode CodeGenerator::unaryOpToOpCode(UnaryOperator op) {
    switch (op) {
        case UnaryOperator::MINUS: return OpCode::NEG;
        case UnaryOperator::PLUS: return OpCode::POS;
        case UnaryOperator::NOT: return OpCode::NOT;
        default: return OpCode::POS; // fallback
    }
}

std::string CodeGenerator::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            default: result += c; break;
        }
    }
    return result;
}

// Assembly Generator implementation
AssemblyGenerator::AssemblyGenerator() : stackOffset(0) {
    availableRegs = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"};
}

std::string AssemblyGenerator::generate(const std::vector<Instruction>& instructions) {
    std::ostringstream asm_code;
    
    // Header
    asm_code << ".section .data\n";
    for (const auto& line : dataSection) {
        asm_code << line << "\n";
    }
    
    asm_code << "\n.section .text\n";
    asm_code << ".globl _start\n";
    asm_code << "_start:\n";
    
    for (const auto& inst : instructions) {
        generateInstruction(inst);
    }
    
    for (const auto& line : textSection) {
        asm_code << line << "\n";
    }
    
    return asm_code.str();
}

void AssemblyGenerator::generateInstruction(const Instruction& inst) {
    switch (inst.opcode) {
        case OpCode::ADD:
            emitText("  mov " + inst.arg1 + ", %rax");
            emitText("  add " + inst.arg2 + ", %rax");
            emitText("  mov %rax, " + inst.result);
            break;
        case OpCode::LOAD_CONST:
            emitText("  mov $" + inst.arg1 + ", " + inst.result);
            break;
        case OpCode::HALT:
            emitText("  mov $60, %rax");  // sys_exit
            emitText("  mov $0, %rdi");   // exit status
            emitText("  syscall");
            break;
        // Add more instruction implementations...
        default:
            emitText("  # Unimplemented instruction");
            break;
    }
}

std::string AssemblyGenerator::allocateRegister(const std::string& temp) {
    if (!availableRegs.empty()) {
        std::string reg = availableRegs.back();
        availableRegs.pop_back();
        tempToReg[temp] = reg;
        return reg;
    }
    // Handle register spilling
    return "(%rsp)"; // Fallback to stack
}

void AssemblyGenerator::freeRegister(const std::string& temp) {
    auto it = tempToReg.find(temp);
    if (it != tempToReg.end()) {
        availableRegs.push_back(it->second);
        tempToReg.erase(it);
    }
}

void AssemblyGenerator::emitData(const std::string& line) {
    dataSection.push_back(line);
}

void AssemblyGenerator::emitText(const std::string& line) {
    textSection.push_back(line);
}

// C Generator implementation
CGenerator::CGenerator() {
    emitInclude("<stdio.h>");
    emitInclude("<stdlib.h>");
    emitInclude("<string.h>");
}

std::string CGenerator::generate(const std::vector<Instruction>& instructions) {
    (void)instructions; // Mark as unused for now
    std::ostringstream c_code;
    
    // Includes
    for (const auto& inc : includes) {
        c_code << inc << "\n";
    }
    c_code << "\n";
    
    // Globals
    for (const auto& global : globals) {
        c_code << global << "\n";
    }
    c_code << "\n";
    
    // Built-in functions
    c_code << "void print(const char* str) {\n";
    c_code << "    printf(\"%s\\n\", str);\n";
    c_code << "}\n\n";
    
    // Generated functions
    for (const auto& func : functions) {
        c_code << func << "\n";
    }
    
    return c_code.str();
}

void CGenerator::generateInstruction(const Instruction& inst) {
    switch (inst.opcode) {
        case OpCode::ADD:
            emitFunction("  " + inst.result + " = " + inst.arg1 + " + " + inst.arg2 + ";");
            break;
        case OpCode::LOAD_CONST:
            emitFunction("  " + inst.result + " = " + inst.arg1 + ";");
            break;
        case OpCode::CALL:
            if (inst.arg1 == "print") {
                emitFunction("  print(" + inst.result + ");");
            } else {
                emitFunction("  " + inst.result + " = " + inst.arg1 + "();");
            }
            break;
        default:
            emitFunction("  // Unimplemented instruction");
            break;
    }
}

void CGenerator::emitInclude(const std::string& header) {
    includes.push_back("#include " + header);
}

void CGenerator::emitGlobal(const std::string& decl) {
    globals.push_back(decl);
}

void CGenerator::emitFunction(const std::string& line) {
    functions.push_back(line);
}

} // namespace SimpleLang