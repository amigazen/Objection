/*
 * flexskeldef.h -- Skeleton definitions for flex-generated scan.c (OCT xlator).
 *
 * Original OCT build expected this from the flex distribution at "s:flexskeldef.h"
 * (Amiga assign). This version inlines the macros from FSF flex (Flex/CommonFiles
 * flexskelcom.h and Flex/Flex1 flexskeldef.h) so scan.c compiles without the
 * flex tree. Required because scan.c contains #line directives pointing at
 * "scan.l", so the compiler reports errors as "scan.l" even when compiling scan.c.
 */

#ifndef FLEXSKELDEF_H
#define FLEXSKELDEF_H

#include <stdio.h>
#include <stdlib.h>
#ifndef EOF
#define EOF  (-1)
#endif

/* -----------------------------------------------------------------------
 * Buffer size (override before including if needed). Default 16KB.
 * ----------------------------------------------------------------------- */
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE  16384
#endif

#define YY_BUF_LIM   YY_BUF_SIZE
#define YY_BUF_MAX   YY_BUF_SIZE
#define YY_MAX_LINE  YY_BUF_SIZE
#define YY_NULL      0

/* -----------------------------------------------------------------------
 * Token / action constants (from flexskelcom.h)
 * ----------------------------------------------------------------------- */
#define YY_END_TOK    0
#define YY_NEW_FILE  (-1)
#define YY_DO_DEFAULT (-2)

/* -----------------------------------------------------------------------
 * ECHO, yywrap, YY_DECL, YY_BREAK (from flexskelcom.h)
 * ----------------------------------------------------------------------- */
#define ECHO  fputs( yytext, yyout )
#define yywrap()  1
#define YY_DECL  int yylex()
#define YY_BREAK  break;

/* -----------------------------------------------------------------------
 * YY_INPUT: read into buf, set result to count read (or 0 on EOF/error).
 * Uses fread so it works on Amiga without fileno/read. For Unix, flex often
 * used: if ( (result = read( fileno(yyin), buf, max_size )) < 0 ) YY_FATAL_ERROR(...);
 * ----------------------------------------------------------------------- */
#define YY_INPUT(buf, result, max_size) \
	do { (result) = (int)fread( (buf), 1, (max_size), yyin ); if ( ferror(yyin) ) YY_FATAL_ERROR( "read in flex scanner failed" ); } while (0)

/* -----------------------------------------------------------------------
 * YY_FATAL_ERROR (from flexskelcom.h)
 * ----------------------------------------------------------------------- */
#define YY_FATAL_ERROR(msg) \
	do { fputs( (msg), stderr ); putc( '\n', stderr ); exit( 1 ); } while (0)

/* -----------------------------------------------------------------------
 * YY_INIT, YY_LENG, YY_DO_BEFORE_SCAN, YY_DO_BEFORE_ACTION (from Flex1 flexskeldef.h)
 * ----------------------------------------------------------------------- */
#define YY_INIT \
	do { \
		yyleng = yy_c_buf_p = yy_e_buf_p = 0; \
		yy_hold_char = yy_ch_buf[yy_c_buf_p] = '\n'; \
		yytext = &yy_ch_buf[yy_c_buf_p]; \
		yy_saw_eof = 0; \
	} while (0)

#define YY_LENG  (yy_c_buf_p - yy_b_buf_p + 1)

#define YY_DO_BEFORE_SCAN \
	yytext[yyleng] = yy_hold_char;

/* Expansion ends with semicolon so "YY_DO_BEFORE_ACTION" (no semicolon at line 505) is valid */
#define YY_DO_BEFORE_ACTION \
	yytext = &yy_ch_buf[yy_b_buf_p]; \
	yyleng = YY_LENG; \
	yy_hold_char = yytext[yyleng]; \
	yytext[yyleng] = '\0';

/* Return all but the first n matched characters back to the input stream (flexskelcom.h) */
#define yyless(n) \
	do { YY_DO_BEFORE_SCAN; yy_c_buf_p = yy_b_buf_p + (n) - 1; YY_DO_BEFORE_ACTION; } while (0)

#ifndef FLEX_REJECT_ENABLED
#define REJECT  YY_FATAL_ERROR( "REJECT used and scanner was not generated using -r" )
#endif

#endif
