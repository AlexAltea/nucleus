/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 *
 * Based on:
 *  - ANGLE:
 *    - The ANGLE Project Authors (2002-2013)
 *  - ANSI C Yacc grammar:
 *    - Jeff Lee (1985)
 *    - Jutta Degener (1995)
 */

%option outfile="glsl_parser.l.cpp"
%option nounistd
%option noyywrap nounput never-interactive
%option yylineno reentrant bison-bridge bison-locations
%option extra-type="void*"

%top {
/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 *
 * Warning:
 *  - This is an automatically generated file.
 *
 * Based on:
 *  - ANGLE:
 *    - The ANGLE Project Authors (2002-2013)
 *  - ANSI C Yacc grammar:
 *    - Jeff Lee (1985)
 *    - Jutta Degener (1995)
 */

#include "nucleus/common.h"
#include "nucleus/assert.h"
#include "nucleus/graphics/frontend/opengl/glsl_parser.y.hpp"

// Ignore errors in auto-generated code
#if defined(NUCLEUS_COMPILER_GCC)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wswitch-enum"

#elif defined(NUCLEUS_COMPILER_MSVC)
#pragma warning(disable: 4005)
#pragma warning(disable: 4065)
#pragma warning(disable: 4102)
#pragma warning(disable: 4189)
#pragma warning(disable: 4244)
#pragma warning(disable: 4505)
#pragma warning(disable: 4701)
#pragma warning(disable: 4702)
#endif
}

%{
	struct YYLTYPE {
        int first_file;
        int first_line;
        int last_file;
        int last_line;
    };

	// Workaround for flex using the register keyword, deprecated in C++11.
	#ifdef __cplusplus
	#if __cplusplus > 199711L
	#define register
	#endif
	#endif

	#define YY_USER_ACTION                                 \
		yylloc->first_file = yylloc->last_file = yycolumn; \
		yylloc->first_line = yylloc->last_line = yylineno;

	#define YY_INPUT(buf, result, max_size) \
		result = string_input(buf, max_size, yyscanner);

	static yy_size_t string_input(char* buf, yy_size_t max_size, yyscan_t yyscanner);
	static int check_type(yyscan_t yyscanner);
	static int reserved_word(yyscan_t yyscanner);
	static int uint_constant(void* context);
	static int int_constant(void* context);
	static int float_constant(yyscan_t yyscanner);
	static int floatsuffix_check(void* context);
%}

%x FIELDS

D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
E           [Ee][+-]?{D}+
O           [0-7]

%%

%{
    void* context = yyextra;
%}

"in"           { return IN; }
"out"          { return OUT; }
"inout"        { return INOUT; }
"attribute"    { return ATTRIBUTE; }
"uniform"      { return UNIFORM; }
"varying"      { return VARYING; }
"centroid"     { return CENTROID; }
"flat"         { return FLAT; }
"smooth"       { return SMOOTH; }
"discard"      { return DISCARD; }
"layout"       { return LAYOUT; }

"const"        { return CONST; }
"break"        { return BREAK; }
"continue"     { return CONTINUE; }
"do"           { return DO; }
"for"          { return FOR; }
"while"        { return WHILE; }
"if"           { return IF; }
"else"         { return ELSE; }
"switch"       { return SWITCH; }
"case"         { return CASE; }
"default"      { return DEFAULT; }
"struct"       { return STRUCT; }
"return"       { return RETURN; }

"float"        { return FLOAT; }
"int"          { return INT; }
"uint"         { return UINT; }
"void"         { return VOID; }
"bool"         { return BOOL; }
"true"         { yylval->lex.b = true;  return BOOLCONSTANT; }
"false"        { yylval->lex.b = false; return BOOLCONSTANT; }

"bvec2"        { return BVEC2; }
"bvec3"        { return BVEC3; }
"bvec4"        { return BVEC4; }
"ivec2"        { return IVEC2; }
"ivec3"        { return IVEC3; }
"ivec4"        { return IVEC4; }
"uvec2"        { return UVEC2; }
"uvec3"        { return UVEC3; }
"uvec4"        { return UVEC4; }
"vec2"         { return VEC2; }
"vec3"         { return VEC3; }
"vec4"         { return VEC4; }
"dvec2"        { return DVEC2; }
"dvec3"        { return DVEC3; }
"dvec4"        { return DVEC4; }

"mat2"         { return MAT2; }
"mat3"         { return MAT3; }
"mat4"         { return MAT4; }
"mat2x2"       { return MAT2X2; }
"mat2x3"       { return MAT2X3; }
"mat2x4"       { return MAT2X4; }
"mat3x2"       { return MAT3X2; }
"mat3x3"       { return MAT3X3; }
"mat3x4"       { return MAT3X4; }
"mat4x2"       { return MAT4X2; }
"mat4x3"       { return MAT4X3; }
"mat4x4"       { return MAT4X4; }

    /* Reserved keywords in GLSL ES 1.00 that are not reserved in GLSL ES 3.00 */
"packed"  {
    /*if (context->getShaderVersion() >= 300)
    {
        yylval->lex.string = NewPoolTString(yytext);
        return check_type(yyscanner);
    }*/

    return reserved_word(yyscanner);
}

    /* Reserved keywords */
"asm"          |

"class"        |
"union"        |
"enum"         |
"typedef"      |
"template"     |
"this"         |

"goto"         |

"inline"       |
"noinline"     |
"volatile"     |
"public"       |
"static"       |
"extern"       |
"external"     |
"interface"    |

"long"         |
"short"        |
"double"       |
"half"         |
"fixed"        |
"unsigned"     |
"superp"       |

"input"        |
"output"       |

"hvec2"        |
"hvec3"        |
"hvec4"        |
"fvec2"        |
"fvec3"        |
"fvec4"        |

"sampler1D"    |
"sampler1DShadow" |
"sampler2DRectShadow" |

"sizeof"       |
"cast"         |

"namespace"    |
"using"        { return reserved_word(yyscanner); }

{L}({L}|{D})*       {
   //yylval->lex.string = NewPoolTString(yytext); 
   return check_type(yyscanner);
}

0[xX]{H}+         { return int_constant(context); }
0{O}+             { return int_constant(context); }
{D}+              { return int_constant(context); }

0[xX]{H}+[uU]     { return uint_constant(context); }
0{O}+[uU]         { return uint_constant(context); }
{D}+[uU]          { return uint_constant(context); }

{D}+{E}           { return float_constant(yyscanner); }
{D}+"."{D}*({E})? { return float_constant(yyscanner); }
"."{D}+({E})?     { return float_constant(yyscanner); }

{D}+{E}[fF]           { return floatsuffix_check(context); }
{D}+"."{D}*({E})?[fF] { return floatsuffix_check(context); }
"."{D}+({E})?[fF]     { return floatsuffix_check(context); }

"+="            { return ADD_ASSIGN; }
"-="            { return SUB_ASSIGN; }
"*="            { return MUL_ASSIGN; }
"/="            { return DIV_ASSIGN; }
"%="            { return MOD_ASSIGN; }
"<<="           { return LEFT_ASSIGN; }
">>="           { return RIGHT_ASSIGN; }
"&="            { return AND_ASSIGN; }
"^="            { return XOR_ASSIGN; }
"|="            { return OR_ASSIGN; }

"++"            { return INC_OP; }
"--"            { return DEC_OP; }
"&&"            { return AND_OP; }
"||"            { return OR_OP; }
"^^"            { return XOR_OP; }
"<="            { return LE_OP; }
">="            { return GE_OP; }
"=="            { return EQ_OP; }
"!="            { return NE_OP; }
"<<"            { return LEFT_OP; }
">>"            { return RIGHT_OP; }
";"             { return SEMICOLON; }
("{"|"<%")      { return LEFT_BRACE; }
("}"|"%>")      { return RIGHT_BRACE; }
","             { return COMMA; }
":"             { return COLON; }
"="             { return EQUAL; }
"("             { return LEFT_PAREN; }
")"             { return RIGHT_PAREN; }
("["|"<:")      { return LEFT_BRACKET; }
("]"|":>")      { return RIGHT_BRACKET; }
"."             { BEGIN(FIELDS); return DOT; }
"!"             { return BANG; }
"-"             { return DASH; }
"~"             { return TILDE; }
"+"             { return PLUS; }
"*"             { return STAR; }
"/"             { return SLASH; }
"%"             { return PERCENT; }
"<"             { return LEFT_ANGLE; }
">"             { return RIGHT_ANGLE; }
"|"             { return VERTICAL_BAR; }
"^"             { return CARET; }
"&"             { return AMPERSAND; }
"?"             { return QUESTION; }

<FIELDS>[ \t\v\f\r] {}

[ \t\v\n\f\r] { }
<*><<EOF>>    { yyterminate(); }
<*>.          { assert_always("Unexpected byte sequence"); return 0; }

%%

yy_size_t string_input(char* buf, yy_size_t max_size, yyscan_t yyscanner) {
    /*pp::Token token;
    yyget_extra(yyscanner)->getPreprocessor().lex(&token);
    yy_size_t len = token.type == pp::Token::LAST ? 0 : token.text.size();
    if (len < max_size)
        memcpy(buf, token.text.c_str(), len);
    yyset_column(token.location.file, yyscanner);
    yyset_lineno(token.location.line, yyscanner);

    if (len >= max_size)
        YY_FATAL_ERROR("Input buffer overflow");
    else if (len > 0)
        buf[len++] = ' ';
    return len;*/return 0;
}

int check_type(yyscan_t yyscanner) {
    struct yyguts_t* yyg = (struct yyguts_t*) yyscanner;
    
    int token = IDENTIFIER;
    /*TSymbol* symbol = yyextra->symbolTable.find(yytext, yyextra->getShaderVersion());
    if (symbol && symbol->isVariable()) {
        TVariable* variable = static_cast<TVariable*>(symbol);
        if (variable->isUserType()) {
            token = TYPE_NAME;
        }
    }
    yylval->lex.symbol = symbol;*/
    return token;
}

int reserved_word(yyscan_t yyscanner) {
    struct yyguts_t* yyg = (struct yyguts_t*) yyscanner;

    /*yyextra->error(*yylloc, "Illegal use of reserved word", yytext, "");
    yyextra->recover();*/
    return 0;
}

int uint_constant(void *context)
{
    /*struct yyguts_t* yyg = (struct yyguts_t*) context->getScanner();

    if (context->getShaderVersion() < 300)
    {
        context->error(*yylloc, "Unsigned integers are unsupported prior to GLSL ES 3.00", yytext, "");
        context->recover();
        return 0;
    }

    if (!atoi_clamp(yytext, &(yylval->lex.u)))
        yyextra->error(*yylloc, "Integer overflow", yytext, "");*/

    return UINTCONSTANT;
}

int floatsuffix_check(void* context)
{
    /*struct yyguts_t* yyg = (struct yyguts_t*) context->getScanner();

    if (context->getShaderVersion() < 300)
    {
        context->error(*yylloc, "Floating-point suffix unsupported prior to GLSL ES 3.00", yytext);
        context->recover();
        return 0;
    }

    std::string text = yytext;
    text.resize(text.size() - 1);
    if (!strtof_clamp(text, &(yylval->lex.f)))
        yyextra->warning(*yylloc, "Float overflow", yytext, "");*/

    return(FLOATCONSTANT);
}

void yyerror(YYLTYPE* lloc, void* context, void *scanner, const char* reason) {
    /*context->error(*lloc, reason, yyget_text(scanner));
    context->recover();*/
}

int int_constant(void *context) {
    /*struct yyguts_t* yyg = (struct yyguts_t*) context->getScanner();
	
    unsigned int u;
    if (!atoi_clamp(yytext, &u))
    {
        if (context->getShaderVersion() >= 300)
            yyextra->error(*yylloc, "Integer overflow", yytext, "");
        else
            yyextra->warning(*yylloc, "Integer overflow", yytext, "");
    }
    yylval->lex.i = static_cast<int>(u);*/
    return INTCONSTANT;
}

int float_constant(yyscan_t yyscanner) {
    /*struct yyguts_t* yyg = (struct yyguts_t*) yyscanner;

    if (!strtof_clamp(yytext, &(yylval->lex.f)))
        yyextra->warning(*yylloc, "Float overflow", yytext, "");*/
    return FLOATCONSTANT;
}

int glslang_initialize(void* context) {
    /*yyscan_t scanner = NULL;
    if (yylex_init_extra(context, &scanner))
        return 1;
	*/
    //context->setScanner(scanner);
    return 0;
}

int glslang_finalize(void* context) {
    /*yyscan_t scanner = context->getScanner();
    if (scanner == NULL) return 0;
    
    context->setScanner(NULL);
    yylex_destroy(scanner);
	*/
    return 0;
}

int glslang_scan(size_t count, const char* const string[], const int length[], void* context) {
    /*yyrestart(NULL, context->getScanner());
    yyset_column(0, context->getScanner());
    yyset_lineno(1, context->getScanner());

    // Initialize preprocessor.
    pp::Preprocessor *preprocessor = &context->getPreprocessor();

    if (!preprocessor->init(count, string, length))
        return 1;

    // Define extension macros.
    const TExtensionBehavior& extBehavior = context->extensionBehavior();
    for (TExtensionBehavior::const_iterator iter = extBehavior.begin();
         iter != extBehavior.end(); ++iter) {
        preprocessor->predefineMacro(iter->first.c_str(), 1);
    }
    if (context->getFragmentPrecisionHigh())
        preprocessor->predefineMacro("GL_FRAGMENT_PRECISION_HIGH", 1);

    preprocessor->setMaxTokenSize(GetGlobalMaxTokenSize(context->getShaderSpec()));
	*/
    return 0;
}
