/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

OPCODE(ADD,       "add",       OPCODE_SIG_V_V_V)   // Addition
OPCODE(SUB,       "sub",       OPCODE_SIG_V_V_V)   // Subtraction
OPCODE(MUL,       "mul",       OPCODE_SIG_V_V_V)   // Multiplication
OPCODE(DIV,       "div",       OPCODE_SIG_V_V_V)   // Division
OPCODE(NEG,       "neg",       OPCODE_SIG_V_V)     // Negation
OPCODE(NOT,       "not",       OPCODE_SIG_V_V)     // Bitwise Not
OPCODE(AND,       "and",       OPCODE_SIG_V_V_V)   // Bitwise And
OPCODE(OR,        "or",        OPCODE_SIG_V_V_V)   // Bitwise Or
OPCODE(XOR,       "xor",       OPCODE_SIG_V_V_V)   // Bitwise Xor
OPCODE(LOG2,      "log2",      OPCODE_SIG_V_V)     // Logarithm in base 2
OPCODE(EXP2,      "exp2",      OPCODE_SIG_V_V)     // Exponentiation in base 2
OPCODE(RCP,       "rcp",       OPCODE_SIG_V_V)     // Reciprocal
OPCODE(RSQRT,     "rsqrt",     OPCODE_SIG_V_V)     // Reciprocal square root
OPCODE(SQRT,      "sqrt",      OPCODE_SIG_V_V)     // Square root
OPCODE(COS,       "cos",       OPCODE_SIG_V_V)     // Cosine
OPCODE(SIN,       "sin",       OPCODE_SIG_V_V)     // Sine
