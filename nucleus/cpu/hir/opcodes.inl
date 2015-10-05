/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

OPCODE(ADD,       "add",       OPCODE_SIG_V_V_V)   // Addition
OPCODE(SUB,       "sub",       OPCODE_SIG_V_V_V)   // Subtraction
OPCODE(MUL,       "mul",       OPCODE_SIG_V_V_V)   // Multiplication
OPCODE(MULH,      "mulh",      OPCODE_SIG_V_V_V)   // Multiplication (high)
OPCODE(DIV,       "div",       OPCODE_SIG_V_V_V)   // Division
OPCODE(ZEXT,      "zext",      OPCODE_SIG_V_V)     // Integer zero-extend 
OPCODE(SEXT,      "sext",      OPCODE_SIG_V_V)     // Integer sign-extend
OPCODE(TRUNC,     "trunc",     OPCODE_SIG_V_V)     // Integer truncation
OPCODE(CTLZ,      "ctlz",      OPCODE_SIG_V_V)     // Count leading zeros
OPCODE(NOT,       "not",       OPCODE_SIG_V_V)     // Bitwise Not
OPCODE(AND,       "and",       OPCODE_SIG_V_V_V)   // Bitwise And
OPCODE(OR,        "or",        OPCODE_SIG_V_V_V)   // Bitwise Or
OPCODE(XOR,       "xor",       OPCODE_SIG_V_V_V)   // Bitwise Xor
OPCODE(SHL,       "shl",       OPCODE_SIG_V_V_V)   // Shift to left
OPCODE(SHR,       "shr",       OPCODE_SIG_V_V_V)   // Shift to right
OPCODE(SHRA,      "shra",      OPCODE_SIG_V_V_V)   // Shift to right (algebraic)
OPCODE(ROL,       "rol",       OPCODE_SIG_V_V_V)   // Rotate to left
OPCODE(ROR,       "ror",       OPCODE_SIG_V_V_V)   // Rotate to right
OPCODE(SQRT,      "sqrt",      OPCODE_SIG_V_V)     // Square root
OPCODE(ABS,       "abs",       OPCODE_SIG_V_V)     // Absolute value
OPCODE(LOAD,      "load",      OPCODE_SIG_V_V)     // Load from memory
OPCODE(STORE,     "store",     OPCODE_SIG_X_V_V)   // Store to memory
OPCODE(CTXLOAD,   "ctxload",   OPCODE_SIG_V_I)     // Context load
OPCODE(CTXSTORE,  "ctxstore",  OPCODE_SIG_X_I_V)   // Context store
OPCODE(MEMFENCE,  "memfence",  OPCODE_SIG_X_V_V)   // Memory fence
OPCODE(SELECT,    "select",    OPCODE_SIG_V_V_V_V) // Select
OPCODE(CMP,       "cmp",       OPCODE_SIG_V_V_V)   // Compare
OPCODE(BR,        "br",        OPCODE_SIG_X_B)     // Branch
OPCODE(ARG,       "arg",       OPCODE_SIG_V_I_V)   // Argument
OPCODE(CALL,      "call",      OPCODE_SIG_M_F)     // Call
OPCODE(BRCOND,    "brcond",    OPCODE_SIG_X_V_B)   // Conditional branch
OPCODE(CALLCOND,  "callcond",  OPCODE_SIG_M_V_F)   // Conditional call
OPCODE(RET,       "ret",       OPCODE_SIG_X_M)     // Return
OPCODE(PHI,       "phi",       OPCODE_SIG_V_V_V)   // Phi node
OPCODE(FADD,      "fadd",      OPCODE_SIG_V_V_V)   // Floating-point addition
OPCODE(FSUB,      "fsub",      OPCODE_SIG_V_V_V)   // Floating-point subtraction
OPCODE(FMUL,      "fmul",      OPCODE_SIG_V_V_V)   // Floating-point multiplication
OPCODE(FDIV,      "fdiv",      OPCODE_SIG_V_V_V)   // Floating-point division
OPCODE(FNEG,      "fneg",      OPCODE_SIG_V_V)     // Floating-point negation
OPCODE(FEXT,      "fext",      OPCODE_SIG_V_V)     // Floating-point extend
OPCODE(FTRUNC,    "ftrunc",    OPCODE_SIG_V_V)     // Floating-point truncation
OPCODE(VADD,      "vadd",      OPCODE_SIG_V_V_V)   // Vector addition
OPCODE(VSUB,      "vadd",      OPCODE_SIG_V_V_V)   // Vector subtraction
OPCODE(VAVG,      "vavg",      OPCODE_SIG_V_V_V)   // Vector average
OPCODE(VCMP,      "vcmp",      OPCODE_SIG_V_V_V)   // Vector compare
