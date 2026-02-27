/*   ERROR.C    Erorr reporting and Recovrey functions       Oct 89, bjw */
/*****************************************************************
 *   This program is copywrite (c) 1989, 1990 by Brian J. Witt.
 *                  rael%aleks@seer.UUCP
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assume
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $15 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
 *		04-nov-89	bjw 	Initial coding.
 *     21-april-89   bjw    Added 'want an identifier'.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 */

#define  DEBUG  0
#define  SW_DBG_WATCH  1
#define  SW_ASSERT  1


#include "oct.h"
#include <tree.h>
#include "gram.h"
#include "error.h"



/*  All error messages are sent to this file: */
#define  STD_ERR_MSG	stdout


/* -----------------  Externals We Take Advantage Of  ------------------ */


/* ---------------------  Globals Based Here  ------------------------ */


/* --------------------  Our Private Variables  ---------------------- */


static char 	obselete_str[ ] = { "<< Obselete >>" } ;

static char 	*gram_errmsg[] =
{
		"?err?",
		"Method declared before class specified",	/* [1] */
		"Bad variable name",
		"Bad syntax in method definition",
		"Extraneous class definition",				/* [4] */
		"Bad expression", 
		"I'm confused; syntax error?",
		"Message expr ended early",
		"Message contains extraneous text",	        /* [8] */
		"Variable declared illegally",
		"Missing selector var, found '(' instead",
		"Missing closing parenthesis",
		"Unexpected symbol occurred:",  		/* %s, [12] */
		"Internal Error:",      		/* THE BIG OPPS!  %s */
		"Bad method declared (in @i/f file)",
		"Underflow in msg context",
		"Not enough core",
		"@defs() class unknown:",       		/* %s */
		"Unexpected EOF",
		"@interface after @includes!",
		"Class category used wrongly",
		"Token ought already be defined:",
		"Directive out of context:",       	 	/* %s */
        "Expected type-name, found:",           /* %s */
        "Expected identifer, found:",           /* %s */
        "Missing @implemention before method declare",
        "Missing @includes() before function declare",
        NULL,
        NULL
} ;

static char    *gram_warnmsg[] =
{
		"?warn?",
		"Superclass mismatch from @i/f to @impl",	  /* [1] */
		"Ill formed selector type",
		"Ill formed typecast in method header",
		"Class not specified in @i/f clause", 			/* [4] */
		"The @includes() directive missing",
		"Missing semi-colon",
		"Method is generic:",    		/* %s */
		"Mismatch in return type:", 	/* %s */
		"Mismatch in argument type:", 	/* %s */
		"Weird internal state:",    	/* %s */
        "Duplicate typedef name:",      /* %s */
        "Bad token in bitfield length:", /* %s */      /* [12] */
		NULL,
		NULL
} ;


static char      err_fmt[ ] = {	 "**** %s(%u) %s %d: %s %s\n"  } ;


/* -------------------  Under-developed Helpers  --------------------- */


/* -------------------------  Error Recovery  ------------------------ */


/*   er_semi  --  Error Recover: Find next semicolon, preceed from there.
//				  Returns 0 if OK, else non-zero to abort.
*/
	int
er_semi()
{

	while( curr_tok != ';' &&
		   curr_tok != CH_LCURLY && curr_tok != CH_RCURLY &&
		   td_nest > 0 )
	{
		(void) get_tok();
	}

	if( curr_tok == DONE )
		return( RC_ERROR );
	unget_tok();
	return( RC_OK );
}   /* er_semi */



/*   er_msg  --  Error Recover: Find end of message expr, preceed from there.
//				 Returns 0 if OK, else non-zero to abort.
*/
	int
er_msg()
{
	short	save_nest = td_nest;

	while( curr_tok != ';' && td_nest >= save_nest && td_nest > 0 )
		(void) get_tok();

	if( curr_tok == DONE )
		return( RC_ERROR );
	unget_tok();
	return( RC_OK );
}   /* er_msg */


/*   er_curly  --  Eat a curly-brace block.  Stops when scanner pops out
//					out of all blocks, or when nesting un-nested.
*/
	int
er_curly()
{

	while( curr_tok != CH_RCURLY && curr_tok != ';' && td_nest > 0 )
		(void) get_tok();

	if( curr_tok == DONE )
		return( RC_ERROR );
	unget_tok();
	return( RC_OK );
}


/*   er_end  --  Eats tokens until "@end" encoutnered.
*/
er_end()
{

	if( warn_level > WARN_NORMAL )
	{
		printf( "skipping until @end.." );
		fflush( stdout );
	}

	while( curr_tok != CLOSE_END && curr_tok != DONE )
	{
		if( get_tok() == OPEN_INTERFACE )
			er_end();
	}

	if( warn_level > WARN_NORMAL )
		putchar( '\n' );

	if( curr_tok == DONE )
		return( RC_ERROR );
	return( RC_OK );
}



/* -------------------  Under-developed Helpers  --------------------- */


/*   gerr  --  Handle the big errors.  Use level=ERROR_ABORT to kill oneself.
 *
 *   level :         0 = print msg, return to caller.
 *         ERROR_ABORT = print msg, call cleanup(10).
 *        ERROR_RESYNC = print msg, request #line real soon.
 *
 *   rec_fptr :   NULL = no resync/recover routine.
 *              ! NULL = a funct that returns 0 if OK, !=0 to Abort.
 */
	void
gerr( level, ecode, rec_fptr, rec_parm )
	int 		level;
	enum GERR_CODES 	ecode;
	PFI 		rec_fptr;
	ULONG		rec_parm;
{
	static char 	*level_str[] = { "Error", "Fatal" };
    int          is_fatal = 0;

	++cnt_errors;
	if( ecode >= GERR__LAST )
		ecode = 0;
    if( (level & IS_ABORT) )
        is_fatal = 1;

	fflush( stdout );
	fprintf( STD_ERR_MSG, err_fmt, src_filename, lineat,
				level_str[ is_fatal ],
				ecode, gram_errmsg[ (int)ecode ], error_string );

	error_string = "" ;

		/* Try to recover... */
	in_state |= (level & ~IS_NEED_HLINE); 	/* means "need resynch" */
	if( is_fatal ||
		(rec_fptr != NULL && (*rec_fptr)( rec_parm ) != RC_OK) )
	{
		printf( "Aborting. " );
		cleanup( 10 );        			/* Routine couldn't cleanup */
	}
}	/* gerr */


	void
gwarn( ecode )
	enum GW_CODES 	ecode;
{

	if( ecode >= GERR__LAST )
		ecode = 0;

	++cnt_warnings;
	fprintf( STD_ERR_MSG, err_fmt, src_filename, lineat, "warning",
				ecode, gram_warnmsg[ (int)ecode ], error_string );

	error_string = "" ;
}	/* gwarn */

