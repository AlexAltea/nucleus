/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

OPCODE(ADD,    "add")    // Addition
OPCODE(SUB,    "sub")    // Subtraction
OPCODE(MUL,    "mul")    // Multiplication
OPCODE(MULH,   "mulh")   // Multiplication (high)
OPCODE(DIV,    "div")    // Division
OPCODE(CTLZ,   "ctlz")   // Count leading zeros
OPCODE(UEXT,   "uext")   // Unsigned integer extension
OPCODE(SEXT,   "sext")   // Signed integer extension
OPCODE(TRUNC,  "trunc")  // Integer truncation
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
OPCODE(LOAD,   "load")   // Load from memory
OPCODE(STORE,  "store")  // Store to memory
OPCODE(SELECT, "select") // Select
OPCODE(BR,     "br")     // Branch
OPCODE(CALL,   "call")   // Call
OPCODE(RET,    "ret")    // Return
OPCODE(PHI,    "phi")    // Phi node
