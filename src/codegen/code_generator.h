#pragma once

#include "../ast/ast_nodes.h"
#include "../ast/ast_visitor.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace SimpleLang {

// Intermediate code representation
enum class OpCode {
    // Arithmetic
    ADD, SUB, MUL, DIV, MOD,
    // Comparison
    EQ, NE, LT, LE, GT, GE,
    // Logical
    AND, OR, NOT,
    // Unary
    NEG, POS,
    // Memory
    LOAD, STORE, LOAD_CONST,
    // Control flow
    JMP, JZ, JNZ, LABEL,
    // Function calls
    CALL, RET, PARAM, ENTER, LEAVE,
    // I/O
    PRINT,
    // Conversion
    CONV,
    // Halt
    HALT
};

struct Instruction {
    OpCode opcode;
    std::string arg1;
    std::string arg2;
    std::string result;
    
    Instruction(OpCode op, const std::string& a1 = "", 
               const std::string& a2 = "", const std::string& res = "")
        : opcode(op), arg1(a1), arg2(a2), result(res) {}
};

class CodeGenerator : public ASTVisitor {
private:
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, int> labelMap;
    std::unordered_map<std::string, int> variables;
    std::unordered_map<std::string, int> functions;
    
    int nextTemp;
    int nextLabel;
    int currentOffset;
    std::string lastResult;
    
public:
    CodeGenerator();
    
    // Main generation method
    std::vector<Instruction> generate(Program& program);
    
    // Expression visitors
    void visitIntegerLiteral(IntegerLiteral& node) override;
    void visitFloatLiteral(FloatLiteral& node) override;
    void visitStringLiteral(StringLiteral& node) override;
    void visitBooleanLiteral(BooleanLiteral& node) override;
    void visitIdentifier(Identifier& node) override;
    void visitBinaryExpression(BinaryExpression& node) override;
    void visitUnaryExpression(UnaryExpression& node) override;
    void visitFunctionCall(FunctionCall& node) override;
    
    // Statement visitors
    void visitExpressionStatement(ExpressionStatement& node) override;
    void visitVariableDeclaration(VariableDeclaration& node) override;
    void visitAssignmentStatement(AssignmentStatement& node) override;
    void visitBlockStatement(BlockStatement& node) override;
    void visitIfStatement(IfStatement& node) override;
    void visitWhileStatement(WhileStatement& node) override;
    void visitForStatement(ForStatement& node) override;
    void visitReturnStatement(ReturnStatement& node) override;
    
    // Declaration visitors
    void visitFunctionDeclaration(FunctionDeclaration& node) override;
    void visitProgram(Program& node) override;
    
    // Code generation utilities
    std::string generateAssembly(const std::vector<Instruction>& instructions);
    std::string generateC(const std::vector<Instruction>& instructions);
    
private:
    // Helper methods
    std::string newTemp();
    std::string newLabel();
    void emit(OpCode opcode, const std::string& arg1 = "", 
             const std::string& arg2 = "", const std::string& result = "");
    void emitLabel(const std::string& label);
    
    OpCode binaryOpToOpCode(BinaryOperator op);
    OpCode unaryOpToOpCode(UnaryOperator op);
    
    std::string escapeString(const std::string& str);
};

// Assembly code generator
class AssemblyGenerator {
private:
    std::vector<std::string> dataSection;
    std::vector<std::string> textSection;
    std::unordered_map<std::string, std::string> tempToReg;
    std::vector<std::string> availableRegs;
    int stackOffset;
    
public:
    AssemblyGenerator();
    std::string generate(const std::vector<Instruction>& instructions);
    
private:
    void generateInstruction(const Instruction& inst);
    std::string allocateRegister(const std::string& temp);
    void freeRegister(const std::string& temp);
    void emitData(const std::string& line);
    void emitText(const std::string& line);
};

// C code generator
class CGenerator {
private:
    std::vector<std::string> includes;
    std::vector<std::string> globals;
    std::vector<std::string> functions;
    std::string currentFunction;
    
public:
    CGenerator();
    std::string generate(const std::vector<Instruction>& instructions);
    
private:
    void generateInstruction(const Instruction& inst);
    void emitInclude(const std::string& header);
    void emitGlobal(const std::string& decl);
    void emitFunction(const std::string& line);
};

} // namespace SimpleLang