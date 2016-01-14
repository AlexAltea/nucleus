/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_GLSL_PARSER_Y_HPP_INCLUDED
# define YY_YY_GLSL_PARSER_Y_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 35 "glsl_parser.y" /* yacc.c:1909  */

	#include <string>

	namespace gfx {
	namespace frontend {
	namespace opengl {

	class OpenGLShaderParser;

	}  // namespace opengl
	}  // namespace frontend
	}  // namespace gfx

#line 58 "glsl_parser.y.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PLUS = 258,
    DASH = 259,
    BANG = 260,
    TILDE = 261,
    COMMA = 262,
    COLON = 263,
    EQUAL = 264,
    SEMICOLON = 265,
    STAR = 266,
    SLASH = 267,
    PERCENT = 268,
    VERTICAL_BAR = 269,
    CARET = 270,
    AMPERSAND = 271,
    QUESTION = 272,
    DOT = 273,
    INC_OP = 274,
    DEC_OP = 275,
    LE_OP = 276,
    GE_OP = 277,
    EQ_OP = 278,
    NE_OP = 279,
    LEFT_OP = 280,
    RIGHT_OP = 281,
    AND_OP = 282,
    OR_OP = 283,
    XOR_OP = 284,
    ADD_ASSIGN = 285,
    SUB_ASSIGN = 286,
    MUL_ASSIGN = 287,
    DIV_ASSIGN = 288,
    MOD_ASSIGN = 289,
    LEFT_ASSIGN = 290,
    RIGHT_ASSIGN = 291,
    AND_ASSIGN = 292,
    OR_ASSIGN = 293,
    XOR_ASSIGN = 294,
    LEFT_PAREN = 295,
    RIGHT_PAREN = 296,
    LEFT_BRACKET = 297,
    RIGHT_BRACKET = 298,
    LEFT_BRACE = 299,
    RIGHT_BRACE = 300,
    LEFT_ANGLE = 301,
    RIGHT_ANGLE = 302,
    ATTRIBUTE = 303,
    VARYING = 304,
    CONST = 305,
    STRUCT = 306,
    WHILE = 307,
    BREAK = 308,
    CONTINUE = 309,
    DO = 310,
    ELSE = 311,
    FOR = 312,
    IF = 313,
    RETURN = 314,
    SWITCH = 315,
    CASE = 316,
    DEFAULT = 317,
    DISCARD = 318,
    SUBROUTINE = 319,
    CENTROID = 320,
    IN = 321,
    OUT = 322,
    INOUT = 323,
    UNIFORM = 324,
    PATCH = 325,
    SAMPLE = 326,
    BUFFER = 327,
    SHARED = 328,
    COHERENT = 329,
    VOLATILE = 330,
    RESTRICT = 331,
    READONLY = 332,
    WRITEONLY = 333,
    NOPERSPECTIVE = 334,
    FLAT = 335,
    SMOOTH = 336,
    LAYOUT = 337,
    VOID = 338,
    BOOL = 339,
    INT = 340,
    UINT = 341,
    FLOAT = 342,
    DOUBLE = 343,
    BVEC2 = 344,
    BVEC3 = 345,
    BVEC4 = 346,
    IVEC2 = 347,
    IVEC3 = 348,
    IVEC4 = 349,
    UVEC2 = 350,
    UVEC3 = 351,
    UVEC4 = 352,
    VEC2 = 353,
    VEC3 = 354,
    VEC4 = 355,
    DVEC2 = 356,
    DVEC3 = 357,
    DVEC4 = 358,
    MAT2 = 359,
    MAT3 = 360,
    MAT4 = 361,
    DMAT2 = 362,
    DMAT3 = 363,
    DMAT4 = 364,
    MAT2X2 = 365,
    MAT2X3 = 366,
    MAT2X4 = 367,
    DMAT2X2 = 368,
    DMAT2X3 = 369,
    DMAT2X4 = 370,
    MAT3X2 = 371,
    MAT3X3 = 372,
    MAT3X4 = 373,
    DMAT3X2 = 374,
    DMAT3X3 = 375,
    DMAT3X4 = 376,
    MAT4X2 = 377,
    MAT4X3 = 378,
    MAT4X4 = 379,
    DMAT4X2 = 380,
    DMAT4X3 = 381,
    DMAT4X4 = 382,
    ATOMIC_UINT = 383,
    BOOLCONSTANT = 384,
    INTCONSTANT = 385,
    UINTCONSTANT = 386,
    FLOATCONSTANT = 387,
    DOUBLECONSTANT = 388,
    IDENTIFIER = 389,
    TYPENAME = 390
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 50 "glsl_parser.y" /* yacc.c:1909  */

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

#line 219 "glsl_parser.y.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (gfx::frontend::opengl::OpenGLShaderParser  &parser);

#endif /* !YY_YY_GLSL_PARSER_Y_HPP_INCLUDED  */
