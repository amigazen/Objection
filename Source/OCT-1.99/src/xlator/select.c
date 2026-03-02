/*   SELECT.C    Selector Logic for OCT Messages             June 89, bjw */
/*****************************************************************
 *   This program is (c) COPYRIGHT 1989, 1990 by Brian J. Witt.
 *                  seer.UUCP!rael!aleks
 *
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responsibility of usefulness or damage incurred arising
 *   from any use of this intellectual property.  Any distribution
 *   must include this copyright notice.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.  A
 *   small fee ( < $15 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *    This file contains the logic that sequences is used to drive
 *    through the message expressions.  The grammar calls here when
 *    a message occurs in the input stream, at the beginning of each
 *    message component, and at the close of a message expression.
 *
 *    Some support routines are here also.  The driver is m_call().
 *    Very long lines are extended with an escaped-newline.  This
 *    keeps the line counter synchronized with reality.
 *
 *    12-june-89	bjw    	Ripped code from main.c
 *    07-july-89	bjw    	Detects "super" with trailing space.
 *    13-july-89	bjw    	Remembers generic typed methods.
 *    20-july-89	bjw 	Some #if DEBUG now if( yydebug ).
 *    8-sept-89     bjw 	Added lex_text() check in push_context().
 *    30-sept-89	bjw 	Finally removed yyerror() calls, using gerr().
 *    26-oct-89     bjw 	Current token name now: curr_name.
 *    30-june-90	bjw 	m_call() extends line with backslash-Newline.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 */


#define  DEBUG 0
#define  SW_WATCH  0
#define  SW_ASSERT	1


#include "oct.h"
#include "error.h"



/* -----------------  Externals We Take Advantage Of  ----------------- */


/* ----------------------  Globals We Manage  ------------------------- */

char    	*selector = NULL;    	/* Current method incovation selector */
char    	*parms = NULL;    		/* text of parameters */
char    	*target = NULL;    		/* Target of method selector */


/* ----------------------  Our Private Parts  ------------------------- */

LOCAL CONTEXT     *curc = NULL;
LOCAL CONTEXT     *rootc = NULL;


/* -----------------------  Object Maintaince  ------------------------ */



/*   trim_parents_ob  -- Helper routine for m_call() to trim the last
 *    	    	    	 char stored in the parents ('ctx') context.
 *    	    	    	 Inspects state vars to see where last char stored.
 */
    LOCAL void
trim_parents_ob( ctx )
    CONTEXT  *ctx;
{
    char	*cp;
    int 	len;

    cp = (ctx->prev_context & IC_OUTPUT_CALL)
                ? ctx->prev_parms
                : ctx->prev_target;

    if( cp != NULL )
    {
        len = strlen(cp) - 1;
        if( ctx->prev_context & IC_VADCL )
        {
         /*	Called upon detecting the comma in a variable argument
         //	declaration list of a method.  Called before MSM_START
         //	as what follows can be variables or messages...
         //	Strings usually end like:  "3,[".  We eliminate
         //	the real and created commas; the bracket can be trimmed
         //	if a message follows.  This makes the string smaller.
         */
         if( len >= 3 && cp[ len ] == '[' )
                strcpy( cp+len, "\n\t" );
        } else
        if( len >= 0 && cp[len] != CH_CLOSE_PAREN )
         cp[ len ] = EOS;

#if SW_WATCH
        printf( "(trim_[) <%s>\n", cp );
#endif

    }	/* if location zero isn't accessable */

}	/* trim_parents_ob */


/* -------------------------  Context Stacking  ------------------------ */


    LOCAL void
push_context()
{
    register CONTEXT	*cp;
    char	*so_far = NULL;

    if( (cp=(CONTEXT *) MALLOC( sizeof(*cp) )) == NULL )
    {
        gerr( ERROR_ABORT, GERR_NO_CORE, NULL, 0L );
        /*NOTREACHED*/
    }

#if 1
    if( save_text == 0 && texts != NULL )
    {
        printf( "(%u) push_ctx not saving <%s>\n", lineat, texts );
    }
#endif

    if( save_text )
    {
        so_far = lex_text( LT_STOP, ' ' );
#if 0
        if( so_far == NULL )
         so_far = newstring( "" );
#else
        if( so_far == NULL )
        {
         printf( "(push_context) lex_text() returned NULL\n" );
         so_far = newstring( "" );
        }
#endif
    }

    cp->parent_context = NULL;
    cp->prev_context   = in_context;
    cp->prev_target    = target;
    cp->prev_selector  = selector;

 	if( in_context & IC_OUTPUT_CALL )
     	cp->prev_parms = newstrcat( parms, so_far );
 	else
     	cp->prev_parms = parms;
 
 	cp->prev_lext      = NULL;
 
    if( rootc == NULL )
    {
        rootc = cp;
        curc = cp;
        cp->ctx_depth = 0;
    } else
    {
        cp->parent_context = curc;
        cp->ctx_depth = curc->ctx_depth + 1;
        curc = cp;
    }

    if( so_far != NULL )
        MFREE( so_far );
    in_context = 0;
    selector   = NULL;
    target     = NULL;
    parms      = NULL;

}	/* push_context */


    LOCAL void
pop_context()
{
    register CONTEXT	*cp;

    /*  1.  Check for error conditions: */
    if( rootc == NULL )
    {
        error_string = "pop_context" ;
        gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
                /*NOTREACHED*/
    }

    /*  2.  Free previous context: */
    if( target != NULL )
        MFREE( target );
    if( selector != NULL )
        MFREE( selector );
    if( parms != NULL )
        MFREE( parms );

    /*  3.  Restore previous context: */
    cp = curc;
    in_context = cp->prev_context;
    selector   = cp->prev_selector;
    target     = cp->prev_target;
    parms      = cp->prev_parms;
    curc       = cp->parent_context;

    /*  Affect lex_text() state: */
    texts      = cp->prev_lext;
    if( texts != NULL ||
       (in_context&(IC_VADCL|IC_OUTPUT_CALL)) == (IC_VADCL|IC_OUTPUT_CALL) )
        save_text = 1;

    if( curc == NULL )
        rootc = NULL;
    MFREE( cp );

}	/* pop_context */



/*   m_call  --  When the eexpr parser encounters an metho opener, calls
 *    	    	 here with different 'starts' values.  To recurse, use
 *    	    	 push_context()/pop_context().  To extend a source line
 *    	    	 that has multiple method invokations, we escape the
 *    	newline, and then indent with a TAB.
 */
    void
m_call( starts )
    enum MSM   starts;
{
    extern char  	*lu_sel_type PARMS(( char **colon_name ));
    extern void 	add_generic  PARMS(( char *colon_name ));

    register char	*p;
    size_t      len;            /* used when checking selector suffix */
    int 	recurse_flag;

    switch( starts )
    {
        case MSM_START :    	/**  START MESSGE  **/
#if SW_WATCH
                if( yydebug )
                {
                   printf( "(m_call) (%u).", lineat );
                }
#endif
                recurse_flag = IC_MESSAGE;
                push_context( );

                if( curc->prev_context & IC_MESSAGE )
                {
                   if( yydebug )
                   	printf( ">> Recursing << " );
                   recurse_flag |= IC_RECURSE;
                }

                in_context |= recurse_flag;
                break ;

        case MSM_VADCL :    	/**  UNLABELED ARGUMENTS  **/
                /*	Called upon detecting the comma in a variable argument
                //	declaration list of a method.  Called before MSM_START
                //	as what follows can be variables or messages...
                */
                in_context |= IC_VADCL;
#if SW_WATCH
                printf( "va-dcl {%s} ", parms );
#endif
                break ;


        case MSM_OBJ_MSG :    	/** MESSAGE TARGET WAS FUNCTION  **/
                /*  Build the first _msg() call. It hasn't appeared since
                //	there was no IDENTIFIER following [.  Calls with
                //	'curr_name' as first method item, so we ignore that.
                //	Real text is saved in 'target'.
                */
                if( (p=strchr(target, CH_PARM_SEP)) != NULL )
                   *p = EOS;

                /*  If recursive, the context has just been pushed!
                */
                if( !(in_context & IC_OUTPUT_CALL) )
                {
                   if( !(in_context & (IC_RECURSE | IC_VADCL)) )
                   {
                   	/*  Back up and remove a right bracket.  This
                   	 *  is from one token look ahead.  Then we
                   	 *  realize it's a message expr...
                   	 *    --  Can you say "disk wear?"
                         */
                   	fseek( yyout, -1L, 1 );
                   } else
                   {
                   	/* Trim parent string to remove '[' */
                   	trim_parents_ob( curc );
                   }

                   if( strncmp( target, "super", 5 ) == 0 &&
                   	( (UCHAR)target[5] <= ' ' ||
                   	  (UCHAR)target[5] == CH_PARM_SEP) )
                   {
                   	/*  When requesting a superclass msg, the anchor
                   	//	point must be given to the dispatcher.
                   	//
                   	//	Dr. Cox does away with this by having the
                   	//	dispatcher search back through the stack
                   	//	frame for the caller's `self' variable (which
                   	//	is at a fixed location).  Thus, with a short
                   	//	dispatcher prolouge, the same arguments can
                   	//	be used when calling either entry point.
                   	*/
                   	in_context |= IC_SUPERCLASS;
                   	if( m_order == 'F' )
                   	{
                       	sprintf( buff, "&factory%s,self", className );
                   	} else
                   	{
                       	sprintf( buff, "&inst%s,self", className );
                   	}
                   	target = newstrjoin( "_msgSuper))(" , buff );
                   } else
                   {
                   	target = newstrins( "_msg))(", target );
                   }

                   in_context |= IC_OUTPUT_CALL;
                }
#if SW_ASSERT
                else {
                   printf( ":: Avoiding duplicate _msg() call ::" );
                }
#endif

                break ;

        case MSM_METHOD :    	/**  MESSAGE ID'S  **/
                p = newstrjoin( selector, curr_name );
                if( selector == NULL )
                   lex_text( LT_START );    		/* Start collecting */
                else
                   MFREE( selector );

                selector = p;
                lex_text( LT_APPEND1, CH_PARM_SEP );	/* Separate items */

                break ;

        case MSM_ID :    	/**  EITHER METHOD OR VARIABLE  **/
                if( selector == NULL )    		/* Need method name! */
                   m_call( MSM_METHOD );

                break ;

        case MSM_DONE :    	/**  MESSSAGE PARAMETERS COMPLETED  **/
                p = lex_text( LT_STOP, ']' );
                parms = newstrcat( parms, p );    /* concat with context */
                MFREE( p );

                /*  Change marker into parameter separator: */
                if( parms != NULL )
                {
                    for( p=parms ; *p ; ++p )
                    	if( (unsigned char)*p == CH_PARM_SEP )
                            *p = ',';
                }

                if( in_context & IC_VADCL )
                {
                   /* Only append "vaDcl:" if not already present (e.g. user
                    * wrote [self error: fmt vaDcl: a, b]; selector is already
                    * "error:vaDcl:" so do not append again. */
                   len = selector != NULL ? strlen( selector ) : 0;
                   if( len < 6 || strcmp( selector + len - 6, "vaDcl:" ) != 0 )
                    selector = newstrcat( selector, "vaDcl:" );
                   strremove( selector, ' ' );
                }
                if( selector == NULL )
                {
                    selector = newstring( "???" );
                }

                /* ------------------------------ */
                /*  Print the message invocation  */
                /* ------------------------------ */

#if 1
                /*  If unknown method, create it here as generic: */
                /*   Never trim off typecast  */
                if( (p=lu_sel_type(&selector)) == NULL )
                {
                   error_string = selector;
                   gwarn( GW_GENERIC_METHOD );

                   /*  Create entry for generic's list: */
                   add_generic( selector );

                   p = newstring( "id" );
                }
#else
                /* Try describing return type: */
                if( (p=lu_sel_type(&selector)) == NULL || STREQ(p,"id") )
                {
                   if( p == NULL || p[0] == EOS )
                   {
                   	error_string = selector;
                   	gwarn( GW_GENERIC_METHOD );

                   	/*  Create entry for generic's list: */
                   	add_generic( selector );
                   } else
                   {
                        /* Let the funct declaration stand: */
                   	MFREE( p );
                   }
                   /*  Use up right parens for typecast: */
                   target = newstrins( "((", target );

                } else
#endif
                {
                   /*  Attach return type in front of _msg() call: */
                   p = newstrins( "(*((", newstrcat( p, "(*)())" ) );
                   target = newstrins( p, target );
                   MFREE( p );
                }	/* else return type known */

                /* Convert from internal to external form: */
                p = asCRef_selector( selector );
                MFREE( selector );
                selector = newstring( p );

                p = newstrcat( newstrcat( target, "," ), selector );
                target = NULL;    		          /* We just free'd it! */

                if( parms && strlen(parms) > 2 )      /* more than ", " */
                {
                   p = newstrcat( p, parms );
                }

                if( rootc == curc && in_context & IC_VADCL )
                {
                   if( lastchr( p ) == ',' )
                   	p = newstrcat(p, "NULL");    /* Use up va-dcl comma */
                }

                p = newstrcat( p, ")" );


                /*  'selector', 'parms', 'target' cleaned up and
                //	free()'d by pop_context().
                */
                pop_context( );

                /*  Build text backwards...*/
                if( rootc == NULL )
                {
                   fprintf( yyout, " %s", p );
                   MFREE( p );
                   target = NULL;
                } else
                {
                   in_state |= IS_NEED_HLINE;
                   p = newstrins( " \n\t", p );   /* extend line,neatly */
                   if( in_context & IC_OUTPUT_CALL )
                   	parms = newstrcat( parms, p );
                   else
                   	target = newstrcat( target, p );
                   MFREE( p );
                }
                break ;

#if SW_ASSERT
        default :
                error_string = "m_call, bad state" ;
                gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
                /*NOTREACHED*/
#endif

    }	/* switch( starts ) */


}	/* m_call */

