/*   EXPR.C     Expression Evaluating code.                  Sept 89, bjw */
/*   :ts=8      */

/*****************************************************************
 *   This program is copyright (c) 1989,1990,1991 by Brian J. Witt.
 *                  rael%aleks@seer.UUCP
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.  Any distribution
 *   must include this copyright notice.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $15 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
 *     Holds expression evaluating code.  To evaluate an expr, call
 *     top_expr().  The translate a function body, call function_body().
 *     
 *     primary()/expr() now handle forms of:
 *      	[obj store:[obj hash] + [obj hash] + 5 ];
 *     and:	[obj store:(*func_ptr)(x, y, z) ];
 * 
 *      06-sept-89	bjw    	Remove from gram.c.
 *      07-oct-89	bjw 	Made primary()/expr() handle more exprs.
 *      26-oct-89	bjw 	Current token name now: curr_name.
 *      25-nov-89	bjw 	Improved if() / while() statements.
 *      6-april-91  bjw     Output commas inside function call.
 *      7-april-91  bjw     peek_tok() returns token type ONLY.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 */


#define  SW_DEBUG  0
#define  SW_ASSERT  1
#define  SW_EXPR_WATCH	1


#include "oct.h"
#include "gram.h"
#include "error.h"



/* ----------------------  Externally, Yours  ------------------------ */


/* --------------------  Our Private Variables  ---------------------- */

static int      	paren_deep;      	/* Paren nesting in exprs */


/* --------------------  Private Code Workings  ---------------------- */


/*   must_match  --  Make sure current token is what caller expects.
 *                   If not, output syntax error message and resynch.
 */
    LOCAL void
must_match( who, tok )
    char    *who;
    int 	tok;
{

    if( curr_tok != tok )
    {
        error_string = curr_name;
        gerr( ERROR_RESYNC, GERR_MISMATCH, er_msg, 0L );
#if SW_ASSERT
        printf(" %s: hoping for %c, paren_deep=%d\n", who,tok,paren_deep);
#endif
    }

    get_tok();
}   /* must_match */


/*   poke_self  --  Ensures the instance variable is referenced from
 *            	    "self".  If capturing, engages lex_text(), otherwise
 *            	    seeks the output stream.
 */
    void
poke_self( token )
    char	*token;
{
    char	*p;

    if( lex_text( LT_QCAPTURE ) )
    {
        	lex_text( LT_TRIM, (char) strlen( token ) );
        	p = newstrjoin( "(self->", token );
        	lex_text( LT_ADD, (char) 1, p );
        	lex_text( LT_APPEND1, ')' );

        	MFREE( p );
    } else
    {
#if SW_EXPR_WATCH
        if( yydebug )
            printf( "(poke self) backing up\n" );
#endif
        fseek( yyout, (long) - strlen( token ), 1 );	/* backup */
        fprintf( yyout, "(self->%s)", token );
    }

}   /* poke_self */



/*  exprFuncParm  --  Parse parmeters for a function call.  Expr's are
 *                    therefore seperated by commas.  Returns ")" token.
 */
    void
exprFuncParm()
{
    extern char    *target, *parms;
    int     future_tok;

    /* for each parameter */
  AGAIN :
    {
        expr( get_tok() );
    }
    if( curr_tok == ',' )
    {
	/*  I believe that a paren needs attachment cuz, trim_parents()
	 *  in select.c is trimming the paren off, and also trimming
         *  the comma off!
	 */
	future_tok = peek_tok( );
	if( (in_context & (IC_MESSAGE|IC_OUTPUT_CALL)) == (IC_MESSAGE|IC_OUTPUT_CALL)  )
	{
	    if( prev_tok == ']' && future_tok == '[' )
	    {
	       /**  fprintf( yyout, "** between][ **" );  **/
	        parms = newstrcat( parms, ",," );
	    } else
	    if( future_tok == '[' )
	  	 ;
	    else
	    if( future_tok == CH_CLOSE_PAREN )
		parms = newstrcat( parms, ")" );
	    else
		parms = newstrcat( parms, ")" );
	}
	else
	if( in_context & IC_MESSAGE )
	{
		/* Will this ever appear?  */
	    fprintf( yyout, "/* in_message */" );
	    parms = newstrcat( parms, "QQ" );
	}
	goto  AGAIN ;
    }   /* repeat while more parameters */

}   /* exprFuncParm */


/*   exprIndirect  --  When we could have indirect reference after some
 *                     expression, then call here!  If we can't indirect,
 *                     then no big deal.
 */
    void
exprIndirect( prevToken, startToken )
    int    prevToken, startToken;
{

    switch( startToken )
    {
        case PTR_OP :
        case '.' :
                get_tok();
                if( curr_tok == TYPE_NAME )
                {
                    curr_tok = IDENTIFIER;
#if SW_ASSERT
                    printf( "making into identifer <%s>\n", curr_name );
#endif
                }
                else
                if( curr_tok != IDENTIFIER )
                {
                    error_string = curr_name;
                    gerr( 0, GERR_NOT_IDENTIFIER, NULL, 0L );
                }
		expr( curr_tok );

                break ;

        case CH_OPEN_PAREN : 	/* function call */
                if( peek_tok() != CH_CLOSE_PAREN )
                {
                    exprFuncParm();
                } else
                {
                    get_tok();    	/* Read close paren */
                }
                must_match( "indirect/funct-call", CH_CLOSE_PAREN );
                break ;

        case '[' :            	/* array */
                while( curr_tok == '[' )
                {
                    expr( get_tok() );
                    must_match( "primary/id", ']' );
                }

                break ;

    }   /* switch on following token */

}   /* exprIndirect */


    int
primary( tok )
    int 	tok;
{
    extern void 	whole_msg_body( );
    int         temp;

    /*  Qualify what we found. 'default' handles difficult stuff..
     *  Each case must have 'last_token' set properly on exit
     */
    switch( tok )
    {
        case IDENTIFIER :
                if( prev_tok != PTR_OP && lu_instvar( curr_name ) )
                {
                    /*  If identifer is unqualified instance var..     *
                    //  First trim idenifier, then add "self->" before *
                    //  the identifer that's appended on.              *
                    */
#if SW_EXPR_WATCH
                    if( verbose_flag || yydebug )
                        printf( "primary(self->%s)\n", curr_name );
#endif
                    poke_self( curr_name );
                }

                temp = curr_tok;
                exprIndirect( temp, get_tok() );

                break ;

        case CONSTANT :
        case STRING_LITERAL :
                get_tok( );
                break ;

        case SIZEOF :
                (void) get_tok( );  	/* Move upto paren.. */
                must_match( "primary/sizeof", CH_OPEN_PAREN );

                /* Check tokens after paren (read by must_match()): */
                if( curr_tok != '*' && known_type(curr_tok, curr_name) )
                {
                    gram_typecast();
                } else
                {
                    expr( curr_tok );   	/* Size of( variable ) */
                }

                must_match( "primary/sizeof", CH_CLOSE_PAREN );
                break ;

        case CH_OPEN_PAREN :    	/* Nested Expression */
                tok = get_tok( );
                if( tok != '*' && known_type(tok,curr_name) )  /* Typecast */
                {
                    gram_typecast();
                    expr( get_tok() );         	/* Now the casted expr */
                } else
                {
                    temp = paren_deep++;
                    expr( tok );
                    --paren_deep;
#if SW_ASSERT
                    if( paren_deep < 0 )
                    {
                    	error_string = "paren underflow" ;
                    	gwarn( GW_INTERNAL );
                    }
#endif
                    {
                        temp = curr_tok;
                    	must_match( "primary/open_paren", CH_CLOSE_PAREN );
                        exprIndirect( temp, curr_tok );
                    }
                }

                break ;

        case '[' :            		/* OCT Message Expr */
                whole_msg_body( );
                must_match( "primary/message", ']' );

                /*  Allow expr after msg to be parsed: */
                if( monadic( curr_tok ) || dyadic( curr_tok ) )
                {
        	        extern CONTEXT  * rootc;
#if SW_DEBUG
                    if( yydebug )
                    {
                         printf( "after msg expr: <%s>", curr_name );
                         fflush( stdout );
                    }
#endif
                    unget_tok();
                    if( rootc != (CONTEXT *)NULL )
                    {
#if SW_DEBUG
                    	printf( " More Text!\n" );
#endif
                    	/*  Force current token as first in save string: */
                    	lex_text( LT_START );
                    	lex_text( LT_ADD, (char) 1, curr_name );
                    }
                } else
                if( (in_context & IC_OUTPUT_CALL) &&
                    curr_tok == CH_CLOSE_PAREN )
                {
                    /*  Force current token as first in save string: */
                    lex_text( LT_START );
                    lex_text( LT_ADD, (char) 1, curr_name );
                }

                break ;

        case ';' :               /* ";"  -- An empty expression.. */
                get_tok();
                break ;

        default :
#if SW_ASSERT
                /*  Symbol requires detailed study: */
                if( tok != '*' && known_type(tok, curr_name) )
                {
                    /*  We have a typecast!  */
                    printf( "(primary) default taking typecast\n" );

                    if( gram_typecast() != RC_OK )
                    	gerr( ERROR_RESYNC, GERR_EXPRESSION, er_msg, 0L );
                } else
#endif
                if( monadic( tok ) || dyadic( tok ) )
                {
                    expr( get_tok() );
                } else
                {
                    /***  IDLE SYMBOL  ***/
                    /*  This occured when a object wasn't included in
                    /*  a msg expr, ie "[newSTR: argv[1]]".
                    */
                    error_string = curr_name;
                    gerr( ERROR_RESYNC, GERR_EXPRESSION, er_msg, 0L );
                }
                break ;

    }   /* switch on easily spotted lexmes, returning last_token */


    /*	If ever "++" or "--" appear, just gloss over it. 
     *	Big time confusion with "a++ b", so surround it with paren's!!
     */
    if( curr_tok == INC_OP || curr_tok == DEC_OP )
    {
        get_tok( );         	/* Read Ahead */
    }

    return curr_tok;
}   /* primary */


/*   expr  --  Starting point for expr parsing.  Calls primary() to
 *             handle tighly bound operators.  Returns latest token.
 */
    int
expr( tok )
    int 	tok;
{

 	while( primary(tok) != DONE )
    {
        /*  If expression just ended, or impossible to go forward, done: */
        if( trailing( curr_tok ) || !dyadic( curr_tok ) )
        {
        	/*  Should we trim? */
        	if( curr_tok == IDENTIFIER &&
                (trailing(prev_tok) || prev_tok == IDENTIFIER ||
                 prev_tok == PTR_OP || prev_tok == '.') &&
                ((in_context & (IC_MESSAGE|IC_OUTPUT_CALL)) == IC_MESSAGE) )
        	{
                /*  Trim IDENTIFIER away, it's really a selector: *
                 *	Trimming of PARM_LABEL's are done for us by   *
                 *  scanner code in all_count().                  *
                 */
                if( lex_text( LT_QCAPTURE ) )
                    lex_text( LT_TRIM, strlen(curr_name) );

                prev_tok = T_DUMMY;    	/* don't trim again! */
        	}	
            /*  else don't trim!  */

        	return( curr_tok );
        }   /* if expression ended */

        tok = get_tok();
    }   /* while not EOF */
    
    gerr( ERROR_RESYNC, GERR_SHORT_MESSAGE, er_msg, 0L );
}   /* expr */



/* --------------------------  Lists of Exprs  ------------------------ */


/*   top_expr  --  Sets up an "expression context."  Resets paren nesting.
*/
    int
top_expr( tok )
    int 	tok;
{
    paren_deep = 0;
    return( expr(tok) );
}


    void
m_arg_list2( depth )
    int 	depth;
{

    while( curr_tok != DONE )
 	{
        m_call( MSM_METHOD );

        /*  If get_tok() reads ']', it will just pass through.. */
        top_expr( get_tok() );
        if( depth > td_nest || curr_tok != PARM_LABEL )
        	break ;

        /*  The PARM_LABEL token will be over-ridden next loop iteration */
    }

    if( curr_tok == ',' )
    {
        m_call( MSM_VADCL );
#if 0
        printf( "(m_arg_list2) Comma Spotted!\n" );
#endif

        /* copy rest of message */
        while( curr_tok == ',' && depth == td_nest )
        	top_expr( get_tok() );
    } else
    if( curr_tok == IDENTIFIER )
    {
        gerr( ERROR_RESYNC, GERR_EXPRESSION, er_msg, 0L );
    }

}   /* m_arg_list2 */



/*   whole_msg_body  --  The opening '[' of a message expression was
 *                       found.  If first time through, this char has
 *                       already been outputted.
 */
    void
whole_msg_body( )
{
    extern  char	*target;    /* for select.c:m_call() */

    short	save_parens = paren_deep;    	/* Cuz we'll call top_expr() */
    short	depth;
    char	*p;

    depth = td_nest;
    m_call( MSM_START );

    lex_text( LT_START );        	/* Catch target */
    top_expr( get_tok() );
    if( lex_text( LT_QCAPTURE, 0 ) )
    {
        p = lex_text( LT_STOP, '[' );
        if( strlen(p) == 0 )        	/* message target was function */
        {
            MFREE( p );
        } else
        {
            target = p;
        }
    }

#if 0
    printf( "((whole_msg:%d, %s(( ", td_nest, target );
#endif

    /*  m_call() trims string to exclude last token; OK */
    m_call( MSM_OBJ_MSG );

    switch( curr_tok )
    {
        case PARM_LABEL :
                    m_arg_list2( depth );
                    break ;

        case IDENTIFIER :
                    m_call( MSM_METHOD );
                    if( get_tok() != ']' )    	/* Eat ']' */
                    	gerr( ERROR_RESYNC, GERR_LONG_MESSAGE, er_msg, 0L );

                    break ;
        default :
#if SW_ASSERT
                    printf( "(whole_msg_body) %d:<%s> Default\n",
                            	curr_tok, curr_name );
#endif
        case DONE :
        case ']'  :
        case ','  :
                    gerr( ERROR_RESYNC, GERR_SHORT_MESSAGE, er_msg, 0L );
                    break ;

    }   /* switch */

    m_call( MSM_DONE );
    paren_deep = save_parens;        	/* Restore nesting */
}   /* whole_msg_body */


/* ----------------------  Main and Drivers  ------------------------- */


/*   function_body  --  Parse function body looking for message expressions.
 *
 *	Input Parameters:
 *        	none.
 *
 *	Input Environment:
 *        	Can be called for an ordinary function or for a method
 *        	definition
 *        	yyin -- opened source OOC file stream.
 *        	yyout -- opened output plain C file stream.
 *        	curr_name -- '{', text of recent token.
 *        	td_cnt -- Balance of {} and [].
 *        	m_order -- 0 if ordinary function, else 'I' or 'F'.
 *
 *	Function References:
 *        	whole_msg_body()
 *          top_expr().
 *
 *	Returned Environment:
 *        	
 *	Returned Values:
 *        	RC_OK = successful translation.
 *
 *	Psuedo-Code:
 *        	while not end of function
 *            	read next token.
 *        	return RC_OK.
 */
    int
function_body()
{
    register int   tok;            		/* Current token */
    int   pp_tok;            	/* Previous previous token.. */
    int   prev_token;        	/* Previous token (hides global var) */
    short	save_nest;      	/* nesting at this level */

    save_nest  = td_nest;
    prev_token = curr_tok;
    pp_tok     = 0;
    while( (tok=get_tok()) != DONE && td_nest >= save_nest )
    {
        if( tok == CH_LCURLY && warn_level > WARN_NORMAL )
        {
        	trip_hline = 1;
        }

        if( prev_token == CH_OPEN_PAREN && tok != '*' &&
        	known_type(tok, curr_name) )
        {
        	if( gram_typecast() != RC_OK )
        	{
                gerr( ERROR_RESYNC, GERR_EXPRESSION, NULL, 0L );
                unget_tok();	/* Parse again what typecast() choked on */
        	}
        	tok = curr_tok;
        }
        else
        /***************************************************************
         *  For the reserved words "for", "while", and "if" we ensure there
         *  is a parenthesis pair.  Since we own the parens, then we must
         *  check that they're there after top_expr() returns.  This way we
         *  can also use must_match().  If we didn't call must_match(),
         *  then top_expr() would own the parens and we won't know if they
         *  were there or not.
         */
        if( tok == FOR )
        {
            get_tok();     /* Read that opening paren */
       	    must_match( "function/for", '(' );
#if 0
            ++paren_deep;
            while( curr_tok != CH_RCURLY && paren_deep > 0 )
	    {
	        expr( curr_tok );
		if( get_tok() == ')' )
		{
		    if( --paren_deep == 0 )
		        unget_tok();
		}
	    }   /* until closing paren */
#else
            do
            {
                if( curr_tok != ';' )
                    (void) top_expr( curr_tok );
            }
            while( (curr_tok == ';' || curr_tok == ',' ) &&
	           get_tok() != DONE && curr_tok != CH_CLOSE_PAREN ) ;;

            must_match( "function/for", ')' );
#endif

            unget_tok();        	/* Parse again token after for() */
            tok = ';' ;
        }
        else
        if( tok == WHILE || tok == IF || tok == SWITCH )
        {
#if 1
            get_tok();
        	must_match( "function/while-if-switch", '(' ); 

            /* Handle expr after keyword */
        	do {
        	   (void) top_expr( curr_tok );
        	}  while( curr_tok != ')' && get_tok() != DONE ) ;;
        	must_match( "function/while-if-switch", ')' );
#else
            get_tok();
        	must_match( "function/while-if-switch", '(' ); 

            /* Handle expr after keyword */
        	(void) top_expr( curr_tok );
        	must_match( "function/while-if-switch", ')' );
#endif
        	unget_tok();        	/* Parse again token after if() */
        	tok = ';' ;
        }
        else
        if( tok == '[' && prev_token != IDENTIFIER &&
            ( (!trailing(prev_token) || separator(prev_token)) ||
              (pp_tok != IDENTIFIER && pp_tok != CONSTANT &&
        	    prev_token == CH_CLOSE_PAREN) ) )
        {
        	whole_msg_body( );    	 /* definite message expr */

        	trip_hline = in_state & IS_NEED_HLINE;
        	in_state &= ~IS_NEED_HLINE;

        	tok = curr_tok;
        }
        else
        if( m_order != 0 &&
            (prev_token != PTR_OP && tok == IDENTIFIER && lu_instvar(curr_name)) )
        {
        	/*  If in method code, and token doesn't have explicit ptr
        	 *	reference, ie, fail on "->isa", force 'self' reference.
        	 *  Another unqualifed instance variable reference:
        	 */
#if SW_EXPR_WATCH
            if( yydebug )
                printf( "function_body(self->%s)\n", curr_name );
#endif
        	poke_self( curr_name );
        }

        /* Advance: */
        pp_tok     = prev_token;
        prev_token = tok;

    }   /* while within expression */

    m_order = 0;            	/* Leave method context */
    return RC_OK;
}   /* function_body */

