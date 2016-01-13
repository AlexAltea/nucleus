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
    ADD_ASSIGN = 273,
    SUB_ASSIGN = 274,
    MUL_ASSIGN = 275,
    DIV_ASSIGN = 276,
    MOD_ASSIGN = 277,
    LEFT_ASSIGN = 278,
    RIGHT_ASSIGN = 279,
    AND_ASSIGN = 280,
    OR_ASSIGN = 281,
    XOR_ASSIGN = 282,
    LEFT_PAREN = 283,
    RIGHT_PAREN = 284,
    LEFT_BRACKET = 285,
    RIGHT_BRACKET = 286,
    LEFT_BRACE = 287,
    RIGHT_BRACE = 288,
    LEFT_ANGLE = 289,
    RIGHT_ANGLE = 290,
    ATTRIBUTE = 291,
    VARYING = 292,
    CONST = 293,
    STRUCT = 294,
    WHILE = 295,
    BREAK = 296,
    CONTINUE = 297,
    DO = 298,
    ELSE = 299,
    FOR = 300,
    IF = 301,
    RETURN = 302,
    SWITCH = 303,
    CASE = 304,
    DEFAULT = 305,
    DISCARD = 306,
    SUBROUTINE = 307,
    CENTROID = 308,
    IN = 309,
    OUT = 310,
    INOUT = 311,
    UNIFORM = 312,
    PATCH = 313,
    SAMPLE = 314,
    BUFFER = 315,
    SHARED = 316,
    COHERENT = 317,
    VOLATILE = 318,
    RESTRICT = 319,
    READONLY = 320,
    WRITEONLY = 321,
    NOPERSPECTIVE = 322,
    FLAT = 323,
    SMOOTH = 324,
    LAYOUT = 325,
    VOID = 326,
    BOOL = 327,
    INT = 328,
    UINT = 329,
    FLOAT = 330,
    DOUBLE = 331,
    BVEC2 = 332,
    BVEC3 = 333,
    BVEC4 = 334,
    IVEC2 = 335,
    IVEC3 = 336,
    IVEC4 = 337,
    UVEC2 = 338,
    UVEC3 = 339,
    UVEC4 = 340,
    VEC2 = 341,
    VEC3 = 342,
    VEC4 = 343,
    DVEC2 = 344,
    DVEC3 = 345,
    DVEC4 = 346,
    MAT2 = 347,
    MAT3 = 348,
    MAT4 = 349,
    DMAT2 = 350,
    DMAT3 = 351,
    DMAT4 = 352,
    MAT2X2 = 353,
    MAT2X3 = 354,
    MAT2X4 = 355,
    DMAT2X2 = 356,
    DMAT2X3 = 357,
    DMAT2X4 = 358,
    MAT3X2 = 359,
    MAT3X3 = 360,
    MAT3X4 = 361,
    DMAT3X2 = 362,
    DMAT3X3 = 363,
    DMAT3X4 = 364,
    MAT4X2 = 365,
    MAT4X3 = 366,
    MAT4X4 = 367,
    DMAT4X2 = 368,
    DMAT4X3 = 369,
    DMAT4X4 = 370,
    ATOMIC_UINT = 371,
    BOOLCONSTANT = 372,
    INTCONSTANT = 373,
    UINTCONSTANT = 374,
    FLOATCONSTANT = 375,
    DOUBLECONSTANT = 376
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

#line 205 "glsl_parser.y.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (gfx::frontend::opengl::OpenGLShaderParser  &parser);

#endif /* !YY_YY_GLSL_PARSER_Y_HPP_INCLUDED  */
