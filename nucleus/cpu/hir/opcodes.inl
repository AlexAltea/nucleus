/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

OPCODE(ADD,    "add")    // Addition
OPCODE(SUB,    "sub")    // Subtraction
OPCODE(MUL,    "mul")    // Multiplication
OPCODE(MULH,   "mulh")   // Multiplication (high)
OPCODE(DIV,    "div")    // Division
OPCODE(ZEXT,   "zext")   // Integer zero-extend 
OPCODE(SEXT,   "sext")   // Integer sign-extend
OPCODE(TRUNC,  "trunc")  // Integer truncation
OPCODE(CTLZ,   "ctlz")   // Count leading zeros
OPCODE(NOT,    "not")    // Bitwise Not
OPCODE(AND,    "and")    // Bitwise And
OPCODE(OR,     "or")     // Bitwise Or
OPCODE(XOR,    "xor")    // Bitwise Xor
OPCODE(SHL,    "shl")    // Shift to left
OPCODE(SHLA,   "shla")   // Shift to left (algebraic)
OPCODE(SHR,    "shr")    // Shift to right
OPCODE(SHRA,   "shra")   // Shift to right (algebraic)
OPCODE(ROL,    "rol")    // Rotate to left
OPCODE(ROR,    "ror")    // Rotate to right
OPCODE(SQRT,   "sqrt")   // Square root
OPCODE(ABS,    "abs")    // Absolute value
OPCODE(LOAD,   "load")   // Load from memory
OPCODE(STORE,  "store")  // Store to memory
OPCODE(SELECT, "select") // Select
OPCODE(CMP,    "cmp")    // Compare
OPCODE(BR,     "br")     // Branch
OPCODE(CALL,   "call")   // Call
OPCODE(RET,    "ret")    // Return
OPCODE(PHI,    "phi")    // Phi node
OPCODE(FADD,   "fadd")   // Floating-point addition
OPCODE(FSUB,   "fsub")   // Floating-point subtraction
OPCODE(FMUL,   "fmul")   // Floating-point multiplication
OPCODE(FDIV,   "fdiv")   // Floating-point division
OPCODE(FNEG,   "fneg")   // Floating-point negation
OPCODE(FEXT,   "fext")   // Floating-point extend
OPCODE(FTRUNC, "ftrunc") // Floating-point truncation
OPCODE(VADD,   "vadd")   // Vector addition
OPCODE(VSUB,   "vadd")   // Vector subtraction
OPCODE(VAVG,   "vavg")   // Vector average
OPCODE(VCMP,   "vcmp")   // Vector compare
