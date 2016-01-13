/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 *
 * Based on:
 *  - GLSLang:
 *    - 3Dlabs Inc. Ltd. (2002-2005)
 *    - LunarG, Inc. (2012-2013)
 *  - ANSI C Yacc grammar:
 *    - Jeff Lee (1985)
 *    - Jutta Degener (1995)
 */

%output   "glsl_parser.y.cpp"
%require  "3.0"
%debug 
%defines

%lex-param   { gfx::frontend::opengl::OpenGLShaderParser  &parser  }
%parse-param { gfx::frontend::opengl::OpenGLShaderParser  &parser  }

%{
#include "nucleus/common.h"
#include "nucleus/graphics/frontend/opengl/opengl_shader_parser.h"

#include <iostream>
#include <cstdlib>
#include <fstream>

extern int yylex(gfx::frontend::opengl::OpenGLShaderParser& parser);
void yyerror(gfx::frontend::opengl::OpenGLShaderParser& parser, const std::string &error);
%}

// Bison dependencies
%code requires {
	#include <string>

	namespace gfx {
	namespace frontend {
	namespace opengl {

	class OpenGLShaderParser;

	}  // namespace opengl
	}  // namespace frontend
	}  // namespace gfx
}

// Bison declarations
%union {
	union {
		bool b;
		int i;
		unsigned int u;
		float f;
		double d;
	} lex;
	union {
		int test;
	} interm;
}

// Symbols
%token <lex> PLUS DASH BANG TILDE COMMA COLON EQUAL SEMICOLON STAR SLASH PERCENT VERTICAL_BAR CARET AMPERSAND QUESTION
%token <lex> ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN
%token <lex> LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE LEFT_ANGLE RIGHT_ANGLE

// Keywords
%token <lex> ATTRIBUTE VARYING CONST STRUCT
%token <lex> WHILE BREAK CONTINUE DO ELSE FOR IF RETURN SWITCH CASE DEFAULT DISCARD SUBROUTINE
%token <lex> CENTROID IN OUT INOUT
%token <lex> UNIFORM PATCH SAMPLE BUFFER SHARED
%token <lex> COHERENT VOLATILE RESTRICT READONLY WRITEONLY
%token <lex> NOPERSPECTIVE FLAT SMOOTH LAYOUT

// Primitive types
%token <lex> VOID BOOL INT UINT FLOAT DOUBLE
%token <lex> BVEC2 BVEC3 BVEC4 IVEC2 IVEC3 IVEC4 UVEC2 UVEC3 UVEC4 VEC2 VEC3 VEC4 DVEC2 DVEC3 DVEC4
%token <lex> MAT2 MAT3 MAT4 DMAT2 DMAT3 DMAT4
%token <lex> MAT2X2 MAT2X3 MAT2X4 DMAT2X2 DMAT2X3 DMAT2X4
%token <lex> MAT3X2 MAT3X3 MAT3X4 DMAT3X2 DMAT3X3 DMAT3X4
%token <lex> MAT4X2 MAT4X3 MAT4X4 DMAT4X2 DMAT4X3 DMAT4X4
%token <lex> ATOMIC_UINT

// Constants
%token <lex> BOOLCONSTANT INTCONSTANT UINTCONSTANT FLOATCONSTANT DOUBLECONSTANT

%% // Grammar rules
/*
variable_identifier
    : IDENTIFIER {
        $$ = parseContext.handleVariable($1.loc, $1.symbol, $1.string);
    }
    ;

primary_expression
    : variable_identifier {
        $$ = $1;
    }
    | INTCONSTANT {
        $$ = parseContext.intermediate.addConstantUnion($1.i, $1.loc, true);
    }
    | UINTCONSTANT {
        parseContext.fullIntegerCheck($1.loc, "unsigned literal");
        $$ = parseContext.intermediate.addConstantUnion($1.u, $1.loc, true);
    }
    | FLOATCONSTANT {
        $$ = parseContext.intermediate.addConstantUnion($1.d, EbtFloat, $1.loc, true);
    }
    | DOUBLECONSTANT {
        parseContext.doubleCheck($1.loc, "double literal");
        $$ = parseContext.intermediate.addConstantUnion($1.d, EbtDouble, $1.loc, true);
    }
    | BOOLCONSTANT {
        $$ = parseContext.intermediate.addConstantUnion($1.b, $1.loc, true);
    }
    | LEFT_PAREN expression RIGHT_PAREN {
        $$ = $2;
        if ($$->getAsConstantUnion())
            $$->getAsConstantUnion()->setExpression();
    }
    ;

postfix_expression
    : primary_expression {
        $$ = $1;
    }
    | postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET {
        $$ = parseContext.handleBracketDereference($2.loc, $1, $3);
    }
    | function_call {
        $$ = $1;
    }
    | postfix_expression DOT IDENTIFIER {
        $$ = parseContext.handleDotDereference($3.loc, $1, *$3.string);
    }
    | postfix_expression INC_OP {
        parseContext.variableCheck($1);
        parseContext.lValueErrorCheck($2.loc, "++", $1);
        $$ = parseContext.handleUnaryMath($2.loc, "++", EOpPostIncrement, $1);
    }
    | postfix_expression DEC_OP {
        parseContext.variableCheck($1);
        parseContext.lValueErrorCheck($2.loc, "--", $1);
        $$ = parseContext.handleUnaryMath($2.loc, "--", EOpPostDecrement, $1);
    }
    ;

integer_expression
    : expression {
        parseContext.integerCheck($1, "[]");
        $$ = $1;
    }
    ;

function_call
    : function_call_or_method {
        $$ = parseContext.handleFunctionCall($1.loc, $1.function, $1.intermNode);
        delete $1.function;
    }
    ;

function_call_or_method
    : function_call_generic {
        $$ = $1;
    }
    ;

function_call_generic
    : function_call_header_with_parameters RIGHT_PAREN {
        $$ = $1;
        $$.loc = $2.loc;
    }
    | function_call_header_no_parameters RIGHT_PAREN {
        $$ = $1;
        $$.loc = $2.loc;
    }
    ;

function_call_header_no_parameters
    : function_call_header VOID {
        $$ = $1;
    }
    | function_call_header {
        $$ = $1;
    }
    ;

function_call_header_with_parameters
    : function_call_header assignment_expression {
        TParameter param = { 0, new TType };
        param.type->shallowCopy($2->getType());
        $1.function->addParameter(param);
        $$.function = $1.function;
        $$.intermNode = $2;
    }
    | function_call_header_with_parameters COMMA assignment_expression {
        TParameter param = { 0, new TType };
        param.type->shallowCopy($3->getType());
        $1.function->addParameter(param);
        $$.function = $1.function;
        $$.intermNode = parseContext.intermediate.growAggregate($1.intermNode, $3, $2.loc);
    }
    ;

function_call_header
    : function_identifier LEFT_PAREN {
        $$ = $1;
    }
    ;

// Grammar Note:  Constructors look like functions, but are recognized as types.

function_identifier
    : type_specifier {
        // Constructor
        $$.intermNode = 0;
        $$.function = parseContext.handleConstructorCall($1.loc, $1);
    }
    | postfix_expression {
        //
        // Should be a method or subroutine call, but we haven't recognized the arguments yet.
        //
        $$.function = 0;
        $$.intermNode = 0;

        TIntermMethod* method = $1->getAsMethodNode();
        if (method) {
            $$.function = new TFunction(&method->getMethodName(), TType(EbtInt), EOpArrayLength);
            $$.intermNode = method->getObject();
        } else {
            TIntermSymbol* symbol = $1->getAsSymbolNode();
            if (symbol) {
                parseContext.reservedErrorCheck(symbol->getLoc(), symbol->getName());
                TFunction *function = new TFunction(&symbol->getName(), TType(EbtVoid));
                $$.function = function;
            } else
                parseContext.error($1->getLoc(), "function call, method, or subroutine call expected", "", "");
        }

        if ($$.function == 0) {
            // error recover
            TString empty("");
            $$.function = new TFunction(&empty, TType(EbtVoid), EOpNull);
        }
    }
    ;

unary_expression
    : postfix_expression {
        parseContext.variableCheck($1);
        $$ = $1;
        if (TIntermMethod* method = $1->getAsMethodNode())
            parseContext.error($1->getLoc(), "incomplete method syntax", method->getMethodName().c_str(), "");
    }
    | INC_OP unary_expression {
        parseContext.lValueErrorCheck($1.loc, "++", $2);
        $$ = parseContext.handleUnaryMath($1.loc, "++", EOpPreIncrement, $2);
    }
    | DEC_OP unary_expression {
        parseContext.lValueErrorCheck($1.loc, "--", $2);
        $$ = parseContext.handleUnaryMath($1.loc, "--", EOpPreDecrement, $2);
    }
    | unary_operator unary_expression {
        if ($1.op != EOpNull) {
            char errorOp[2] = {0, 0};
            switch($1.op) {
            case EOpNegative:   errorOp[0] = '-'; break;
            case EOpLogicalNot: errorOp[0] = '!'; break;
            case EOpBitwiseNot: errorOp[0] = '~'; break;
            default: break; // some compilers want this
            }
            $$ = parseContext.handleUnaryMath($1.loc, errorOp, $1.op, $2);
        } else {
            $$ = $2;
            if ($$->getAsConstantUnion())
                $$->getAsConstantUnion()->setExpression();
        }
    }
    ;
// Grammar Note:  No traditional style type casts.

unary_operator
    : PLUS  { $$.loc = $1.loc; $$.op = EOpNull; }
    | DASH  { $$.loc = $1.loc; $$.op = EOpNegative; }
    | BANG  { $$.loc = $1.loc; $$.op = EOpLogicalNot; }
    | TILDE { $$.loc = $1.loc; $$.op = EOpBitwiseNot;
              parseContext.fullIntegerCheck($1.loc, "bitwise not"); }
    ;
// Grammar Note:  No '*' or '&' unary ops.  Pointers are not supported.

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression STAR unary_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "*", EOpMul, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    | multiplicative_expression SLASH unary_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "/", EOpDiv, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    | multiplicative_expression PERCENT unary_expression {
        parseContext.fullIntegerCheck($2.loc, "%");
        $$ = parseContext.handleBinaryMath($2.loc, "%", EOpMod, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "+", EOpAdd, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    | additive_expression DASH multiplicative_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "-", EOpSub, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression LEFT_OP additive_expression {
        parseContext.fullIntegerCheck($2.loc, "bit shift left");
        $$ = parseContext.handleBinaryMath($2.loc, "<<", EOpLeftShift, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    | shift_expression RIGHT_OP additive_expression {
        parseContext.fullIntegerCheck($2.loc, "bit shift right");
        $$ = parseContext.handleBinaryMath($2.loc, ">>", EOpRightShift, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression LEFT_ANGLE shift_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "<", EOpLessThan, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    | relational_expression RIGHT_ANGLE shift_expression  {
        $$ = parseContext.handleBinaryMath($2.loc, ">", EOpGreaterThan, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    | relational_expression LE_OP shift_expression  {
        $$ = parseContext.handleBinaryMath($2.loc, "<=", EOpLessThanEqual, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    | relational_expression GE_OP shift_expression  {
        $$ = parseContext.handleBinaryMath($2.loc, ">=", EOpGreaterThanEqual, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ_OP relational_expression  {
        parseContext.arrayObjectCheck($2.loc, $1->getType(), "array comparison");
        parseContext.opaqueCheck($2.loc, $1->getType(), "==");
        $$ = parseContext.handleBinaryMath($2.loc, "==", EOpEqual, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    | equality_expression NE_OP relational_expression {
        parseContext.arrayObjectCheck($2.loc, $1->getType(), "array comparison");
        parseContext.opaqueCheck($2.loc, $1->getType(), "!=");
        $$ = parseContext.handleBinaryMath($2.loc, "!=", EOpNotEqual, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    ;

and_expression
    : equality_expression { $$ = $1; }
    | and_expression AMPERSAND equality_expression {
        parseContext.fullIntegerCheck($2.loc, "bitwise and");
        $$ = parseContext.handleBinaryMath($2.loc, "&", EOpAnd, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression CARET and_expression {
        parseContext.fullIntegerCheck($2.loc, "bitwise exclusive or");
        $$ = parseContext.handleBinaryMath($2.loc, "^", EOpExclusiveOr, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression VERTICAL_BAR exclusive_or_expression {
        parseContext.fullIntegerCheck($2.loc, "bitwise inclusive or");
        $$ = parseContext.handleBinaryMath($2.loc, "|", EOpInclusiveOr, $1, $3);
        if ($$ == 0)
            $$ = $1;
    }
    ;

logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression AND_OP inclusive_or_expression {
        $$ = parseContext.handleBinaryMath($2.loc, "&&", EOpLogicalAnd, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    ;

logical_xor_expression
    : logical_and_expression { $$ = $1; }
    | logical_xor_expression XOR_OP logical_and_expression  {
        $$ = parseContext.handleBinaryMath($2.loc, "^^", EOpLogicalXor, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    ;

logical_or_expression
    : logical_xor_expression { $$ = $1; }
    | logical_or_expression OR_OP logical_xor_expression  {
        $$ = parseContext.handleBinaryMath($2.loc, "||", EOpLogicalOr, $1, $3);
        if ($$ == 0)
            $$ = parseContext.intermediate.addConstantUnion(false, $2.loc);
    }
    ;

conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression QUESTION {
        ++parseContext.controlFlowNestingLevel;
    }
      expression COLON assignment_expression {
        --parseContext.controlFlowNestingLevel;
        parseContext.boolCheck($2.loc, $1);
        parseContext.rValueErrorCheck($2.loc, "?", $1);
        parseContext.rValueErrorCheck($5.loc, ":", $4);
        parseContext.rValueErrorCheck($5.loc, ":", $6);
        $$ = parseContext.intermediate.addSelection($1, $4, $6, $2.loc);
        if ($$ == 0) {
            parseContext.binaryOpError($2.loc, ":", $4->getCompleteString(), $6->getCompleteString());
            $$ = $6;
        }
    }
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | unary_expression assignment_operator assignment_expression {
        parseContext.arrayObjectCheck($2.loc, $1->getType(), "array assignment");
        parseContext.opaqueCheck($2.loc, $1->getType(), "=");
        parseContext.lValueErrorCheck($2.loc, "assign", $1);
        parseContext.rValueErrorCheck($2.loc, "assign", $3);
        $$ = parseContext.intermediate.addAssign($2.op, $1, $3, $2.loc);
        if ($$ == 0) {
            parseContext.assignError($2.loc, "assign", $1->getCompleteString(), $3->getCompleteString());
            $$ = $1;
        }
    }
    ;

assignment_operator
    : EQUAL {
        $$.loc = $1.loc;
        $$.op = EOpAssign;
    }
    | MUL_ASSIGN {
        $$.loc = $1.loc;
        $$.op = EOpMulAssign;
    }
    | DIV_ASSIGN {
        $$.loc = $1.loc;
        $$.op = EOpDivAssign;
    }
    | MOD_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "%=");
        $$.loc = $1.loc;
        $$.op = EOpModAssign;
    }
    | ADD_ASSIGN {
        $$.loc = $1.loc;
        $$.op = EOpAddAssign;
    }
    | SUB_ASSIGN {
        $$.loc = $1.loc;
        $$.op = EOpSubAssign;
    }
    | LEFT_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "bit-shift left assign");
        $$.loc = $1.loc; $$.op = EOpLeftShiftAssign;
    }
    | RIGHT_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "bit-shift right assign");
        $$.loc = $1.loc; $$.op = EOpRightShiftAssign;
    }
    | AND_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "bitwise-and assign");
        $$.loc = $1.loc; $$.op = EOpAndAssign;
    }
    | XOR_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "bitwise-xor assign");
        $$.loc = $1.loc; $$.op = EOpExclusiveOrAssign;
    }
    | OR_ASSIGN {
        parseContext.fullIntegerCheck($1.loc, "bitwise-or assign");
        $$.loc = $1.loc; $$.op = EOpInclusiveOrAssign;
    }
    ;

expression
    : assignment_expression {
        $$ = $1;
    }
    | expression COMMA assignment_expression {
        $$ = parseContext.intermediate.addComma($1, $3, $2.loc);
        if ($$ == 0) {
            parseContext.binaryOpError($2.loc, ",", $1->getCompleteString(), $3->getCompleteString());
            $$ = $3;
        }
    }
    ;

constant_expression
    : conditional_expression {
        parseContext.constantValueCheck($1, "");
        $$ = $1;
    }
    ;

declaration
    : function_prototype SEMICOLON {
        parseContext.handleFunctionDeclarator($1.loc, *$1.function, true /* prototype *//*);
        $$ = 0;
        // TODO: 4.0 functionality: subroutines: make the identifier a user type for this signature
    }
    | init_declarator_list SEMICOLON {
        if ($1.intermNode && $1.intermNode->getAsAggregate())
            $1.intermNode->getAsAggregate()->setOperator(EOpSequence);
        $$ = $1.intermNode;
    }
    | PRECISION precision_qualifier type_specifier SEMICOLON {
        parseContext.profileRequires($1.loc, ENoProfile, 130, 0, "precision statement");

        // lazy setting of the previous scope's defaults, has effect only the first time it is called in a particular scope
        parseContext.symbolTable.setPreviousDefaultPrecisions(&parseContext.defaultPrecision[0]);
        parseContext.setDefaultPrecision($1.loc, $3, $2.qualifier.precision);
        $$ = 0;
    }
    | block_structure SEMICOLON {
        parseContext.declareBlock($1.loc, *$1.typeList);
        $$ = 0;
    }
    | block_structure IDENTIFIER SEMICOLON {
        parseContext.declareBlock($1.loc, *$1.typeList, $2.string);
        $$ = 0;
    }
    | block_structure IDENTIFIER array_specifier SEMICOLON {
        parseContext.declareBlock($1.loc, *$1.typeList, $2.string, $3.arraySizes);
        $$ = 0;
    }
    | type_qualifier SEMICOLON {
        parseContext.globalQualifierFixCheck($1.loc, $1.qualifier);
        parseContext.updateStandaloneQualifierDefaults($1.loc, $1);
        $$ = 0;
    }
    | type_qualifier IDENTIFIER SEMICOLON {
        parseContext.checkNoShaderLayouts($1.loc, $1.shaderQualifiers);
        parseContext.addQualifierToExisting($1.loc, $1.qualifier, *$2.string);
        $$ = 0;
    }
    | type_qualifier IDENTIFIER identifier_list SEMICOLON {
        parseContext.checkNoShaderLayouts($1.loc, $1.shaderQualifiers);
        $3->push_back($2.string);
        parseContext.addQualifierToExisting($1.loc, $1.qualifier, *$3);
        $$ = 0;
    }
    ;

block_structure
    : type_qualifier IDENTIFIER LEFT_BRACE { parseContext.nestedBlockCheck($1.loc); } struct_declaration_list RIGHT_BRACE {
        --parseContext.structNestingLevel;
        parseContext.blockName = $2.string;
        parseContext.globalQualifierFixCheck($1.loc, $1.qualifier);
        parseContext.checkNoShaderLayouts($1.loc, $1.shaderQualifiers);
        parseContext.currentBlockQualifier = $1.qualifier;
        $$.loc = $1.loc;
        $$.typeList = $5;
    }

identifier_list
    : COMMA IDENTIFIER {
        $$ = new TIdentifierList;
        $$->push_back($2.string);
    }
    | identifier_list COMMA IDENTIFIER {
        $$ = $1;
        $$->push_back($3.string);
    }
    ;

function_prototype
    : function_declarator RIGHT_PAREN  {
        $$.function = $1;
        $$.loc = $2.loc;
    }
    ;

function_declarator
    : function_header {
        $$ = $1;
    }
    | function_header_with_parameters {
        $$ = $1;
    }
    ;


function_header_with_parameters
    : function_header parameter_declaration {
        // Add the parameter
        $$ = $1;
        if ($2.param.type->getBasicType() != EbtVoid)
            $1->addParameter($2.param);
        else
            delete $2.param.type;
    }
    | function_header_with_parameters COMMA parameter_declaration {
        //
        // Only first parameter of one-parameter functions can be void
        // The check for named parameters not being void is done in parameter_declarator
        //
        if ($3.param.type->getBasicType() == EbtVoid) {
            //
            // This parameter > first is void
            //
            parseContext.error($2.loc, "cannot be an argument type except for '(void)'", "void", "");
            delete $3.param.type;
        } else {
            // Add the parameter
            $$ = $1;
            $1->addParameter($3.param);
        }
    }
    ;

function_header
    : fully_specified_type IDENTIFIER LEFT_PAREN {
        if ($1.qualifier.storage != EvqGlobal && $1.qualifier.storage != EvqTemporary) {
            parseContext.error($2.loc, "no qualifiers allowed for function return",
                               GetStorageQualifierString($1.qualifier.storage), "");
        }
        if ($1.arraySizes)
            parseContext.arraySizeRequiredCheck($1.loc, *$1.arraySizes);

        // Add the function as a prototype after parsing it (we do not support recursion)
        TFunction *function;
        TType type($1);
        function = new TFunction($2.string, type);
        $$ = function;
    }
    ;

parameter_declarator
    // Type + name
    : type_specifier IDENTIFIER {
        if ($1.arraySizes) {
            parseContext.profileRequires($1.loc, ENoProfile, 120, E_GL_3DL_array_objects, "arrayed type");
            parseContext.profileRequires($1.loc, EEsProfile, 300, 0, "arrayed type");
            parseContext.arraySizeRequiredCheck($1.loc, *$1.arraySizes);
        }
        if ($1.basicType == EbtVoid) {
            parseContext.error($2.loc, "illegal use of type 'void'", $2.string->c_str(), "");
        }
        parseContext.reservedErrorCheck($2.loc, *$2.string);

        TParameter param = {$2.string, new TType($1)};
        $$.loc = $2.loc;
        $$.param = param;
    }
    | type_specifier IDENTIFIER array_specifier {
        if ($1.arraySizes) {
            parseContext.profileRequires($1.loc, ENoProfile, 120, E_GL_3DL_array_objects, "arrayed type");
            parseContext.profileRequires($1.loc, EEsProfile, 300, 0, "arrayed type");
            parseContext.arraySizeRequiredCheck($1.loc, *$1.arraySizes);
        }
        parseContext.arrayDimCheck($2.loc, $1.arraySizes, $3.arraySizes);

        parseContext.arraySizeRequiredCheck($3.loc, *$3.arraySizes);
        parseContext.reservedErrorCheck($2.loc, *$2.string);

        $1.arraySizes = $3.arraySizes;

        TParameter param = { $2.string, new TType($1)};
        $$.loc = $2.loc;
        $$.param = param;
    }
    ;

parameter_declaration
    //
    // With name
    //
    : type_qualifier parameter_declarator {
        $$ = $2;
        if ($1.qualifier.precision != EpqNone)
            $$.param.type->getQualifier().precision = $1.qualifier.precision;
        parseContext.precisionQualifierCheck($$.loc, $$.param.type->getBasicType(), $$.param.type->getQualifier());

        parseContext.checkNoShaderLayouts($1.loc, $1.shaderQualifiers);
        parseContext.parameterTypeCheck($2.loc, $1.qualifier.storage, *$$.param.type);
        parseContext.paramCheckFix($1.loc, $1.qualifier, *$$.param.type);

    }
    | parameter_declarator {
        $$ = $1;

        parseContext.parameterTypeCheck($1.loc, EvqIn, *$1.param.type);
        parseContext.paramCheckFix($1.loc, EvqTemporary, *$$.param.type);
        parseContext.precisionQualifierCheck($$.loc, $$.param.type->getBasicType(), $$.param.type->getQualifier());
    }
    //
    // Without name
    //
    | type_qualifier parameter_type_specifier {
        $$ = $2;
        if ($1.qualifier.precision != EpqNone)
            $$.param.type->getQualifier().precision = $1.qualifier.precision;
        parseContext.precisionQualifierCheck($1.loc, $$.param.type->getBasicType(), $$.param.type->getQualifier());

        parseContext.checkNoShaderLayouts($1.loc, $1.shaderQualifiers);
        parseContext.parameterTypeCheck($2.loc, $1.qualifier.storage, *$$.param.type);
        parseContext.paramCheckFix($1.loc, $1.qualifier, *$$.param.type);
    }
    | parameter_type_specifier {
        $$ = $1;

        parseContext.parameterTypeCheck($1.loc, EvqIn, *$1.param.type);
        parseContext.paramCheckFix($1.loc, EvqTemporary, *$$.param.type);
        parseContext.precisionQualifierCheck($$.loc, $$.param.type->getBasicType(), $$.param.type->getQualifier());
    }
    ;

parameter_type_specifier
    : type_specifier {
        TParameter param = { 0, new TType($1) };
        $$.param = param;
        if ($1.arraySizes)
            parseContext.arraySizeRequiredCheck($1.loc, *$1.arraySizes);
    }
    ;

init_declarator_list
    : single_declaration {
        $$ = $1;
    }
    | init_declarator_list COMMA IDENTIFIER {
        $$ = $1;
        parseContext.declareVariable($3.loc, *$3.string, $1.type);
    }
    | init_declarator_list COMMA IDENTIFIER array_specifier {
        $$ = $1;
        parseContext.declareVariable($3.loc, *$3.string, $1.type, $4.arraySizes);
    }
    | init_declarator_list COMMA IDENTIFIER array_specifier EQUAL initializer {
        $$.type = $1.type;
        TIntermNode* initNode = parseContext.declareVariable($3.loc, *$3.string, $1.type, $4.arraySizes, $6);
        $$.intermNode = parseContext.intermediate.growAggregate($1.intermNode, initNode, $5.loc);
    }
    | init_declarator_list COMMA IDENTIFIER EQUAL initializer {
        $$.type = $1.type;
        TIntermNode* initNode = parseContext.declareVariable($3.loc, *$3.string, $1.type, 0, $5);
        $$.intermNode = parseContext.intermediate.growAggregate($1.intermNode, initNode, $4.loc);
    }
    ;

single_declaration
    : fully_specified_type {
        $$.type = $1;
        $$.intermNode = 0;
        parseContext.declareTypeDefaults($$.loc, $$.type);
    }
    | fully_specified_type IDENTIFIER {
        $$.type = $1;
        $$.intermNode = 0;
        parseContext.declareVariable($2.loc, *$2.string, $1);
    }
    | fully_specified_type IDENTIFIER array_specifier {
        $$.type = $1;
        $$.intermNode = 0;
        parseContext.declareVariable($2.loc, *$2.string, $1, $3.arraySizes);
    }
    | fully_specified_type IDENTIFIER array_specifier EQUAL initializer {
        $$.type = $1;
        TIntermNode* initNode = parseContext.declareVariable($2.loc, *$2.string, $1, $3.arraySizes, $5);
        $$.intermNode = parseContext.intermediate.growAggregate(0, initNode, $4.loc);
    }
    | fully_specified_type IDENTIFIER EQUAL initializer {
        $$.type = $1;
        TIntermNode* initNode = parseContext.declareVariable($2.loc, *$2.string, $1, 0, $4);
        $$.intermNode = parseContext.intermediate.growAggregate(0, initNode, $3.loc);
    }

// Grammar Note:  No 'enum', or 'typedef'.

fully_specified_type
    : type_specifier {
        $$ = $1;

        parseContext.globalQualifierTypeCheck($1.loc, $1.qualifier, $$);
        if ($1.arraySizes) {
            parseContext.profileRequires($1.loc, ENoProfile, 120, E_GL_3DL_array_objects, "arrayed type");
            parseContext.profileRequires($1.loc, EEsProfile, 300, 0, "arrayed type");
        }

        parseContext.precisionQualifierCheck($$.loc, $$.basicType, $$.qualifier);
    }
    | type_qualifier type_specifier  {
        parseContext.globalQualifierFixCheck($1.loc, $1.qualifier);
        parseContext.globalQualifierTypeCheck($1.loc, $1.qualifier, $2);

        if ($2.arraySizes) {
            parseContext.profileRequires($2.loc, ENoProfile, 120, E_GL_3DL_array_objects, "arrayed type");
            parseContext.profileRequires($2.loc, EEsProfile, 300, 0, "arrayed type");
        }

        if ($2.arraySizes && parseContext.arrayQualifierError($2.loc, $1.qualifier))
            $2.arraySizes = 0;

        parseContext.checkNoShaderLayouts($2.loc, $1.shaderQualifiers);
        $2.shaderQualifiers.merge($1.shaderQualifiers);
        parseContext.mergeQualifiers($2.loc, $2.qualifier, $1.qualifier, true);
        parseContext.precisionQualifierCheck($2.loc, $2.basicType, $2.qualifier);

        $$ = $2;

        if (! $$.qualifier.isInterpolation() &&
            ((parseContext.language == EShLangVertex   && $$.qualifier.storage == EvqVaryingOut) ||
             (parseContext.language == EShLangFragment && $$.qualifier.storage == EvqVaryingIn)))
            $$.qualifier.smooth = true;
    }
    ;

layout_qualifier
    : LAYOUT LEFT_PAREN layout_qualifier_id_list RIGHT_PAREN {
        $$ = $3;
    }
    ;

layout_qualifier_id_list
    : layout_qualifier_id {
        $$ = $1;
    }
    | layout_qualifier_id_list COMMA layout_qualifier_id {
        $$ = $1;
        $$.shaderQualifiers.merge($3.shaderQualifiers);
        parseContext.mergeObjectLayoutQualifiers($$.qualifier, $3.qualifier, false);
    }

layout_qualifier_id
    : IDENTIFIER {
        $$.init($1.loc);
        parseContext.setLayoutQualifier($1.loc, $$, *$1.string);
    }
    | IDENTIFIER EQUAL constant_expression {
        $$.init($1.loc);
        parseContext.setLayoutQualifier($1.loc, $$, *$1.string, $3);
    }
    | SHARED { // because "shared" is both an identifier and a keyword
        $$.init($1.loc);
        TString strShared("shared");
        parseContext.setLayoutQualifier($1.loc, $$, strShared);
    }
    ;

precise_qualifier
    : PRECISE {
        $$.init($1.loc);
    }
    ;

type_qualifier
    : single_type_qualifier {
        $$ = $1;
    }
    | type_qualifier single_type_qualifier {
        $$ = $1;
        if ($$.basicType == EbtVoid)
            $$.basicType = $2.basicType;

        $$.shaderQualifiers.merge($2.shaderQualifiers);
        parseContext.mergeQualifiers($$.loc, $$.qualifier, $2.qualifier, false);
    }
    ;

single_type_qualifier
	: layout_qualifier
	| precise_qualifier
    | precision_qualifier
    | storage_qualifier
    ;
*/
unary_operator
    : PLUS
    | DASH
    | BANG
    | TILDE
    ;
	
%% // Additional C code

void yyerror(const std::string &error) {
    std::cerr << "Error: " << error << "\n";
}

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
