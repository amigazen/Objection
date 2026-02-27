/*   GRAM.H    Constants used by grammar                     July 89, bjw */

/*****************************************************************
 *   This program is (c) COPYRIGHT 1989,1990,1992 by Brian J. Witt.
 *                  gentoo.com!rael!aleks
 *
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responsibility of usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.  A
 *   small fee ( < $25 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *   	This file contains no user tuneable parameters.  Different
 *   	scanners may require changes here.
 *
 *		28-june-89	bjw/bll lex no longer used.
 *		24-aug-89	bjw		Updated to Objective-C version 4.
 *		4-nov-89	bjw 	Moved token vars here.
 *		21-nov-89	bjw 	Added storage class and OC special tests.
 *     20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
*/


#ifndef PARMS
# define  PARMS(x)  ()
#endif


/* ---------------------  Globals Based Here  ------------------------ */


#ifndef GRAMMAR

/*  This is a `quality of' array for each symbol.  The "88" codes follow,
//	along with macros for their lookup.  Assumes token is in legal range.
*/
extern unsigned char 	tokenkey[ ];
#define  TKEY_MONADIC    0x01
#define  TKEY_DYADIC     0x02
#define  TKEY_TYPENAME   0x04
#define  TKEY_TRAILING   0x08
#define  TKEY_SEPERATE   0x10
#define  TKEY_STORECLASS 0x20
#define  TKEY_OC_SPECIAL 0x40


/*  Returns TRUE if token is a monadic class, ie "!", "~". */
#define  monadic(tok)   (tokenkey[tok]&TKEY_MONADIC)

/*  Returns TRUE is the token is a dyadic class, ie "%", "||". */
#define  dyadic(tok)    (tokenkey[tok]&TKEY_DYADIC)

/*  Returns TRUE if token is builtin type, ie, "unsigned" */
#define  typetoken(tok)  (tokenkey[tok]&TKEY_TYPENAME)

/*  Returns TRUE if token ends expression, ie ")", "," */
#define  trailing(tok)   (tokenkey[tok]&TKEY_TRAILING)

/*  Returns TRUE if token separators expressions, ie ";" */
#define  separator(tok)  (tokenkey[tok]&TKEY_SEPERATE)

/*  Returns TRUE if token a storage class, ie "automatic" */
#define  storage(tok)    (tokenkey[tok]&TKEY_STORECLASS)

/*	Returns TRUE if token is special OC extension, ie "@interface" */
#define  ocspecial(tok)  (tokenkey[tok]&TKEY_OC_SPECIAL)


extern int 			yydebug;		/* Debugging level */


/*  Three level look back on tokens.  Maintained by get_tok() */
extern int			curr_tok;		/* Token read by last function */
extern int 			prev_tok;	    /* Looking Back.. */
extern int 			pp_tok;   	    /* Looking way back */

extern unsigned  	lineat;   		/* From lexer */

#endif


/* ------------------------  Scanner Codes  ------------------------- */


/*  These are multi-charactor or position dependent symbols: */
enum TOKEN_SYMBOLS {
		DONE = 0,					/* yylex() done working on input */

		IDENTIFIER =  256,
		CONSTANT ,
		STRING_LITERAL ,
		SIZEOF,

		PTR_OP ,
		INC_OP ,
		DEC_OP ,
		LEFT_OP ,
		RIGHT_OP ,
		LE_OP ,
		GE_OP ,
		EQ_OP ,			/* "==" */
		NE_OP,
		AND_OP ,
		OR_OP ,

		MUL_ASSIGN ,
		DIV_ASSIGN ,
		MOD_ASSIGN ,
		ADD_ASSIGN,
		SUB_ASSIGN ,
		LEFT_ASSIGN ,
		RIGHT_ASSIGN ,
		AND_ASSIGN,
		XOR_ASSIGN ,
		OR_ASSIGN ,

		TYPE_NAME,
		T_TYPEDEF ,
		T_EXTERN ,
		T_STATIC ,
		T_AUTO ,
		T_REGISTER,

		T_CHAR ,
		T_SHORT ,
		T_INT ,
		T_LONG ,
		T_SIGNED ,
		T_UNSIGNED ,
		T_FLOAT ,
		T_DOUBLE,

		T_VOID ,
		T_STRUCT ,
		T_UNION ,
		T_ENUM,
		T_CONST ,
		T_VOLATILE ,
		ELIPSIS ,
		RANGE,

		CASE ,
		DEFAULT ,
		IF ,
		ELSE ,
		SWITCH ,
		WHILE ,
		DO ,
		FOR ,
		GOTO ,
		CONTINUE ,
		BREAK ,
		RETURN,

		OPEN_INTERFACE,
		OPEN_IMPLEMENTATION, 	/* @implementation  class definition */
		OPEN_PROTOCOL,			/* @protocol  class specification */
		CLOSE_END ,

		OPEN_FACTORY_METHOD ,
		OPEN_INSTANCE_METHOD,
		PARM_LABEL,				/* IDENTIFIER":"  -- label with colon */
		OOC_DEFS ,				/* "@defs("  -- our preprocessor */
		OOC_SELECTOR,			/* "@selector(" */
		OOC_PUBLIC,				/* "@public", instance public access */
		OOC_ENCODE,				/* Return type encoding */

		T_DUMMY,				/* Used by grammar to zap previous token */

		_LAST_TOKEN_SYMBOL			/* Always the last one! */
} ;


/* end of gram.h */

