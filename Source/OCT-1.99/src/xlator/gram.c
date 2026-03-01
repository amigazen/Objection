/*   GRAM.C    Grammar driver for OCT                        July 89, bjw */
/*	:ts=8	%W%  %G%  */

/*****************************************************************
 *   This program is copywrite (c) 1989,1990,1992 by Brian J. Witt.
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
 *    This file contains the parsing grammar for Object Orientated "C".
 *    Originally this was a full-fledged "C" grammar with extensions,
 *    but that proved to be overkill as the grammar would detect
 *    imbalanced parens, and toss the cookies and die (error recovery
 *    still is shabby).  Because of this, the top routine is called
 *    yyparse().  The grammar is now procedure based, and it runs alot
 *    quicker now!
 *
 *    By studying the relations of text in a program source file, I've
 *    been able to "fake it" for parsing.  Ie, a typecast must contain
 *    either builtin types (int, long, float) or typedef'ed names.
 *    A typedef statment can have only one unknown symbol, and that
 *    symbol is the new symbol to add to the known-symbol-list.
 *
 *    A message expression is differentiated from an array subscript
 *    by noting the lexme in front of the open bracket ([).  If a
 *    general expression ends before the bracket, or a symbol is in
 *    front of the bracket, it's an array.  An expression separator
 *    (ack Pascal) like (;), ({) or (}), and also (+), (&&) and (/),
 *    in front of a bracket means message expression.  The array
 *    tokenkey[] holds classifications for each token type in the
 *    system.
 *
 *    If we do, however, take on token too many, there's a large
 *    and grousume if() statement that decides where to trim off
 *    the trailing idenifier, or just leave things as they are.
 *    If msg expressions are coming out right, this should be a
 *    suspect area.
 *
 *    As the expressions inside a message read, we call m_call()
 *    telling it what part of the expression we're on (receiver object,
 *    closing bracket).  We accept tokens, m_call() maintains state,
 *    pushing and poping state when necessary, and outputting the
 *    translated expression when a message expr completes.
 *
 *    03-july-89    bjw 	Initial coding.
 *    06-july-89    bjw 	Added "," message support.
 *    11-july-89    bjw 	Cognizant of instance variables.
 *    05-aug-89     bjw 	Pointer ops as target OK "[*p++ hash]".
 *    1-sept-89     bjw 	get_tok() handles token sequencing.
 *    6-sept-89     bjw 	Moved expr code to another file.
 *    26-oct-89     bjw 	Current token name now: curr_name.
 *    12-jan-90     bjw 	Illegal for @interface after @includes.
 *    31-jan-91     bjw     known_type() describes 1 token only.
 *    7-april-91    bjw     peek_tok() returns token type ONLY.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *    11-may-91     bjw     Required @includes/@imp before functions.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    20-april-92   bjw    @abstract --> @protocol (for NeXT v2).
 *    1-july-92     bjw      Help in parsing "typedef long MAT[8];"
 */

#define  SW_DEBUG  0
#define  SW_WATCH  1
#define  SW_ASSERT  1


#include "oct.h"
#include "gram.h"
#include "error.h"


/* ----------------------  Externally, Yours  ------------------------ */


/* ---------------------  Globals Based Here  ------------------------ */

int 	    	yydebug;	    /* Debugging level */

/*  Three level look back on tokens.  Maintained by get_tok() */
int          	curr_tok;	    /* Token read by last function */
int             prev_tok;	    /* Looking Back.. */
int             pp_tok;   	    /* Looking way back */



/* -------------------  Under-developed Helpers  --------------------- */


/*   stripPointers  --  Remove pointers that are outsie of a structure
 *                      define.  Ie, "{char *;} *" --> "{char *;}".
 *    NOTE:             Conversion performed in place.
 */
char *
stripPointers( typestr )
    char    *typestr;
{
    int      len;
    char     *p;

#if SW_ASSERT
    if( typestr == NULL )
    {
    	printf( "(stripPointers) handed NULL ptr!\n" );
    	return newstring( "" );
    }
#endif

    len = strlen( typestr );
    for( p = &typestr[len-1] ; p > typestr && *p && *p != CH_RCURLY ;
         --p )
    {
        if( *p == '*' )
	    *p = ' ' ;
    }

    return( typestr );
}	/* stripPointers */


/*   known_type  --  Returns +1 if token is a known type.  For introducers
 *                   like "struct", "union", "enum", we return -1.  Storage
 *                   modifiers, like "const", return -2.  The pointer symbol
 *                   (*) returns -3. For everthing else, returns 0.
 *           To do "unsigned char", you have to call here twice.  Keep
 *           calling until this routine returns a zero.
 */
    int
known_type( tok, text )
    int 	tok;
    char	*text;
{

    switch( tok )
    {
        case '*' :
                    return -3;

        case T_REGISTER :
        case T_AUTO :
        case T_CONST :
        case T_VOLATILE :
        	        return -2;

        case T_STRUCT :
        case T_UNION :
        case T_ENUM :
                    return -1;

        case IDENTIFIER :
                    if( lu_type( text ) )
                    	return 1;

    }	/* switch on easy cases */

    if( typetoken(tok) || tok == TYPE_NAME )
    {
        return 1;
    }

    return 0;
}	/* known_type */


/* --------------------  Private Code Workings  ---------------------- */


/*   digest_block  --  Parser just read an open bracket or curly brace.  We
 *                     just collect text until the block is done.  Very
 *                     useful to call lex_text(LT_START) first.  Uses 
 *                     'td_nest' to catch last '}' or ']'.  Returns with
 *                     'last_token' on closing symbol (]) or (}).
 */
    void
digest_block()
{
    short	save_nest = td_nest;

    while( get_tok() != DONE && save_nest <= td_nest )
        ;;

}	/* digest_block */


/* -----------------------  Types and Structures  ----------------------- */


/*   gram_typecast  --  Parse a type casting.  Basically everything 
 *                      up to a close paren or punctuation must be a known
 *                      type.  Returns 0 if parsed OK.  Handles "(int *)".
 *         Assumes caller has read first token inside typecast.
 *         Does "unsigned long int **".
 *   Bugs:
 *         Doesn't handle nested paren's.
 */
    int
gram_typecast()
{
    int   type_kind;

    in_state |= IS_TYPECAST;

    do
    {
        if( curr_tok == CH_CLOSE_PAREN )
        {
        	/*  The way out, found closing paren for typecast: */
            /*  The Good Way to Return: */
        	in_state &= ~IS_TYPECAST;
        	return( RC_OK );
        }

        switch( known_type(curr_tok, curr_name) )
        {
            case -1 :               /* Read the struct/enum/union name */
                   (void) get_tok();	    	/* Accept its name */
                    break ;

            case  0 :
                    /*  Optional protocol list after type: (id<Protocol>) or (id<P1,P2>) */
                    if ( curr_tok == '<' )
                    {
                        get_tok();
                        while ( curr_tok == IDENTIFIER )
                        {
                            get_tok();
                            if ( curr_tok != ',' )
                                break;
                            get_tok();
                        }
                        if ( curr_tok == '>' )
                            get_tok();
                        break;
                    }
                    /*  Something went wrong, didn't find closing paren: */
                    in_state &= ~IS_TYPECAST;
                    return( RC_ERROR );

        }	/* switch on if known type */

    }
    while( get_tok() != DONE ) ;;

    /*  Something went wrong, didn't find closing paren.. */
    in_state &= ~IS_TYPECAST;
    return( RC_ERROR );
}	/* gram_typecast */


/*   parse_structure  --  Digest a structure definition.  Return string
 *             	          of types used to define this type.
 * 
 *   Input Parameters:
 *     	Token type of T_STRUCT, T_UNION, or T_ENUM.
 * 
 *   Input Environment:
 *     	curr_name -- '{'.
 * 
 *   Function References:
 *     	known_type(), get_tok(), peek_tok(), stripPointers().
 * 
 *   Returned Environment:
 *     	curr_name = '}'.
 *     	Does not change lex_text() setting.
 *     	
 *   Returned Values:
 *     	vars = string of variables from structure.
 * 
 *   Psuedo-Code:
 *     	end while.
 *     	return RC_OK.
 * 
 *   BUGS:
 *      Skips over array sizes.  char foo[5+240/8] yields "char foo[]".
 */
    struct mynode *
parse_structure( souoe )
    int    souoe;                    /* Token: Struct Or Union Or Enum */
{
    struct mynode  *retnode;
    char	*typestr, *varstr, *junkstr, *pubstr;
    short	save_nest = td_nest;
    int 	junkid, rc;
    char    again;

    typestr = newstring(NULL);
    pubstr  = newstring(NULL);
    varstr  = newstring(" ");     /* lu_instvar() requires leading space */

 RESTART :
    while( get_tok() != DONE && save_nest <= td_nest )
    {
        again = 0;
        while( (rc=known_type( curr_tok, curr_name )) )
        {
            ++again;
            switch( rc )
            {
                case -2 :     /* Ignore storage qualifiers */
                        break ;

                case -1 :
                        junkid = curr_tok;
                        typestr = newstrcat( typestr, curr_name );

                    	/* Eat structure/union/enum name */
                    	if( get_tok() == IDENTIFIER )
                    	{
                        	/*  Form of "struct name .."  */
                            typestr = newstrcat( typestr, curr_name );
                        	get_tok();      /* Find that curly */
                        }

                    	if( curr_tok == CH_LCURLY )
                    	{
                            /*  Form of "struct {  } .." or
                             *   "union every {  } .."
                             */
                            typestr = newstrcat( typestr, " {" );

                            retnode = parse_structure( junkid );
                            junkstr = retnode->node.ln_Name;
                            pubstr = newstrcat(pubstr, retnode->publicNames);

                            if( junkid == T_ENUM )
                            {
                                junkstr = "" ;
                            }
                            junkstr = newstrjoin( junkstr, " } " );
                            typestr = newstrcat( typestr, junkstr );
			    done_mynode( retnode );
                    	}
                        else
                        {
                            unget_tok();      /* Reuse whatever it is! */
                        }

                    	break ;

                       /*  Handle known type identifiers and such... */
               default :
#if 0
                     printf("(parse_strct default) name=$%lx -><%s> <%s>\n",
                                 curr_name, curr_name, typestr );
#endif
                        typestr = newstrcat( typestr, curr_name );
                        break ;

            }	/* switch */

            get_tok();
        }	/* while */

        /*  Optional protocol qualifier: id<ProtocolName> or id<P1, P2> */
        if ( curr_tok == '<' && again != 0 )
        {
            get_tok();
            if ( curr_tok == IDENTIFIER )
            {
                typestr = newstrcat( typestr, " /* conforms to " );
                typestr = newstrcat( typestr, curr_name );
                get_tok();
                while ( curr_tok == ',' )
                {
                    typestr = newstrcat( typestr, ", " );
                    get_tok();
                    if ( curr_tok == IDENTIFIER )
                    {
                        typestr = newstrcat( typestr, curr_name );
                        get_tok();
                    }
                }
                typestr = newstrcat( typestr, " */" );
                if ( curr_tok == '>' )
                    get_tok();
            }
        }

        /*  Check if we found any type tokens: */
        if( again == 0 )
        {
           error_string = curr_name;     /* thanx to Ramsey 12-apr-91 */
           gerr( 0, GERR_NOT_TYPENAME, NULL, 0L );
           get_tok();     /* Pretend it's an undefined typedef name */
        }

        /*  Step through the tokens.  Skip pointers and array size
         *	declarations.  The element names are extracted and
         *	strung together in 'varstr'.
         */ ;
        for( again=TRUE ; again ; )
        {
            again = FALSE;
            /*  1.  Collect pointers in front of element name: */
            while( curr_tok == '*' )
            {
                typestr = newstrcat( typestr, curr_name );
                get_tok();
            }
#if SW_WATCH
	if( yydebug )
            printf( " (parse_struct) element=%s. ", curr_name );
#endif
            /*  2.  Store the element name: */
            varstr = newstrcat( varstr, curr_name );
            if( (in_state & IS_SEEN_PUBLIC) )
            {
                pubstr = newstrcat( pubstr, curr_name );
            }

            /*  3.  Kinda skip array declaration.  If source code reads
             *      "char foo[10+(4+5)/2];" we could choke... :-( 
             */
             /*  Gobble up arrays:  */
             while( get_tok() == '[' )       /* Reads "," ";" or "[" */
             {
                /*  Need some sort of constant folding for array size! */
                while( get_tok() != ']' )
                    ;;
                typestr = newstrcat( typestr, "[ ]" );   /* KLUDGE ] */
             }

            /*  4.  What is after that name: */
            if( curr_tok == ',' )
            {
                /*  If a comma out there, strip off all pointers and
                 *  duplicate the type string for the next variable...
                 *  "int  x,y;"  -> "int ; int ;" and "x y".
                 */
                char   *p1, *p2;

                get_tok();         /* Gee, what's next? */
                again = TRUE;      /* Repeat tag for() loop */

                /*  Find the last type-spec stored: */
                if( (p2=strrchr( typestr, ';')) != NULL )
                {
                    junkstr = newstring( p2+1 );
                } else
                {
                    junkstr = newstring( typestr );
                }
      
                stripPointers( junkstr );
#if SW_WATCH
    if( yydebug )
        printf( " again type <%s>\n", junkstr );
#endif
                typestr = newstrcat( typestr, ";" );
                typestr = newstrins( typestr, junkstr );
            }
            else
            if( curr_tok == ':' )
            {
                /*  Bitfield found!  */
                if( get_tok() != CONSTANT && curr_tok != IDENTIFIER )
                {
                    error_string = curr_name;
                    gwarn( GW_BITFIELD_LENGTH );
                }
                typestr = newstrcat( typestr, " :" );
                typestr = newstrcat( typestr, curr_name );

                get_tok();         /* Gee, what's next? */
            } else
            if( curr_tok != IDENTIFIER )
                typestr = newstrcat( typestr, curr_name );
            else
            { ;
#if SW_ASSERT
		if( yydebug )
                    printf( "NOTE: (parse_struct) not adding <%s>\n",
		                                        curr_name );
#endif
            } ;

        }   /* for each token after type-spec to parse */

    }	/* for each token after structure to inspect.. */


    /*  If nothing here, just return some space.. */
#if SW_DEBUG
    if( yydebug || verbose_flag )
        printf( "(parse_struct) return names=<%s>, ret=<%s>\n",
                                  varstr, typestr );
#endif

    retnode = mk_mynode( typestr, varstr );
    retnode->publicNames = pubstr;
    MFREE( varstr );
    MFREE( typestr );
    return( retnode );
}	/* parse_structure */




/*   type_def  --  Expand our dictionary of known types.  If it is an
 *         	   identifer, then it's being defined here.  Called after
 *         	   "typedef" spotted.
 * 
 * Input Parameters:
 *     	none.
 * 
 * Input Environment:
 *     	yyin -- opened source OOC file stream.
 *     	yyout -- opened output plain C file stream.
 *     	curr_name -- text of token just after "typedef".
 * 
 * Function References:
 *     	parse_structure(), stripPointers().
 * 
 * Returned Environment:
 *     	
 * Returned Values:
 *     	RC_OK = successful translation.
 * 
 * Psuedo-Code:
 *     	while( read_token() != ';' || nested )
 *         	if( token == IDENTIFIER && next_token in {,;)} )
 *             	addTypeName( token ).
 *     	end while.
 *     	return RC_OK.
 * 
 * Notes:
 *     	Handles "typedef struct { char *_bp; } FILE, *FILEPTR;"
 *     	Handles "typedef struct _file { char *_bp; } FILE;"
 *     	and "typedef int  (*PFI)( char * );"
 *      and "typedef long  MAT[8];"
 */
    int
type_def( )
{
    register int     tok;	        	/* Current token */
    struct mynode   *structnode;
    int         opentok;
    char        setup[ 2 ];
    char    	*equiv = NULL;
    short    	save_nest = td_nest;
    int         parenPairCnt = 0;

    setup[1] = 0;

    while( (tok=get_tok()) != ';' || save_nest < td_nest )
    {
        /*  If a structure is named, then gobble the next symbol.
        //	If structure begin defined, then eat up element list.
        //	Otherwise continue with curr_name after structure name,
        //	ie, "typedef struct Food FOOD".
        //	ie, "typedef union { char that; } ;".
        */
        if( tok == T_STRUCT || tok == T_UNION )
        {
	    opentok = tok;
            equiv = newstring( curr_name );   /* Grab "struct" */
            tok = get_tok();
#if SW_DEBUG
            if( yydebug )
            {
                printf( "(type_def) \"%s %s\" line #%u\n",
                                        equiv,curr_name,lineat);
            }
#endif
            if( tok == IDENTIFIER )   /* typedef struct x {} */
            {
                equiv = newstrcat( equiv, curr_name );   /* struct _foo.. */
                tok = get_tok();
            }

            if( tok == CH_LCURLY )    /* { */
            {
                MFREE( equiv );      /* "struct x" drops on the floor... */
                structnode = parse_structure( opentok );  /* Ends on '}' */
                equiv = newstrjoin( opentok == T_STRUCT ? "" : "union ",
                                    structnode->node.ln_Name );

                /*  Enclose the structure in curlies { } to protect it. */
                setup[0] = CH_RCURLY;
                strcat( equiv, setup );
                setup[0] = CH_LCURLY;
                equiv = newstrins( setup, equiv );

		done_mynode( structnode );   /* Frees struct string */
                continue ;
            }
        }

     AGAIN :
        if( tok == DONE )
            return( RC_ERROR );

        if( tok == CH_CLOSE_PAREN )
        {
            if( ++parenPairCnt > 0 )
            {
	        /*  Skip over any stuff inside the second set of parens, */
                /*  which would be prototype information in:             */
                /*   typedef int  (*PFI)( char *) ;                      */
                while( (tok=get_tok()) != CH_CLOSE_PAREN )
                    ;;

                equiv = newstrcat( equiv, ")()" );
                continue ;
            }
	}

        if( (tok == IDENTIFIER && save_nest == td_nest) &&
        	(strlen(curr_name) < 40) )
        {
            strcpy( buff, curr_name );
            if( (tok=get_tok()) == ',' || tok == ';' )
            {
#if SW_WATCH
                if( yydebug )
                    printf( "enter_type(%s,%s)\n", buff, equiv );
#endif
                enter_type( buff, equiv );

                parenPairCnt = 0;
                stripPointers( equiv );      /* if type used again, then */
                             /* .. move to base type without indirection */
            } else
            {
#if SW_WATCH
                if( yydebug )
                    printf( "(typdef) skipping <%s>\n", curr_name );
#endif
                goto AGAIN ;	     /* Try test again with this token */
            }

        } else
        {
        	equiv = newstrcat( equiv, curr_name );
        }

        if( tok == ';' )
        	break ;

    }	/* while still in typedef.. */

    if( prev_tok == ']' || prev_tok == CH_CLOSE_PAREN )
    {
        if( yydebug )
            printf( "enter_type(%s,%s)\n", buff, equiv );
        enter_type( buff, equiv );	/* Found "typedef long MAT[8];" */
    }

    if( equiv )
        MFREE( equiv );

    return( RC_OK );
}	/* type_def */



/*   extern_ref  --  Quickly eat an external reference.
//
//	Input Parameters:
//	    	none.
//
//	Input Environment:
//	    	yyin -- opened source OOC file stream.
//	    	yyout -- opened output plain C file stream.
//	    	curr_name -- text of recent token.
//
//	Function References:
//	    	int get_tok() -- Fetches next token.
//
//	Returned Environment:
//	    	yyin -- positioned at ';'.
//	    	
//	Returned Values:
//	    	RC_OK = successful translation.
//
//	Psuedo-Code:
//	    	while( (tok != ';' OR we're nested) AND not done )
//	        	read_next_token().
//	    	return OK if tok is ';'.
*/
    int
extern_ref()
{
    short	nesting;

    nesting = td_nest;
    while( ( get_tok() != ';' || nesting < td_nest ) && curr_tok != DONE )
        ;;

    return( curr_tok != ';' );
}	/* extern_ref */


/*   haveFunctYet  -- Checks if either @include or @implementation see
 *                    yet.  If neither, then user isn't allowed to
 *                    define any functions yet.
 */
    void
haveFunctYet()
{

    if( ! (in_state & (IS_GOT_IMP | IS_INCLUDES)) )
	{
        gerr( ERROR_ERROR, GERR_MISSING_INCLUDES, NULL, 0L );
    }
}   /* haveFunctYet */


/*   yyparse  --  Main driver to translate OCT .m file into plain C.
//
//	Input Parameters:
//	    	none.
//
//	Input Environment:
//	    	yyin -- opened source OOC file stream.
//	    	yyout -- opened output plain C file stream.
//	    	curr_name -- text of recent token.
//
//	Function References:
//	    	int get_tok() -- Fetches next token.
//	    	int type_def() -- Accumulate user created types.
//	    	int do_if()    -- Superclass definition encountered.
//	    	int do_imp()   -- Class definition spotted.
//	    	int extern_ref() -- Quickly handle extern statements.
//	    	int method_def() -- Method defintion spotted.
//	    	int function_body() -- Parse out body of function.
//          int peek_tok()  -- Reveal next token type.
//          void haveFunctYet() -- Test is @includesor @impl seen yet.
//
//	Returned Environment:
//	    	yyin -- positioned at EOF.
//	    	
//	Returned Values:
//	    	RC_OK = successful translation.
//	    	RC_ERR >= error somewhere.
//
//	Psuedo-Code:
//	    	init_parser().
//	    	read token.
//	    	while not eof do
//	        	switch( token )
//	            	case "typedef" : call type_def().
//	            	case "extern" : call extern_ref().
//	            	case "@protocol" : call do_protocol().
//	            	case "@interface" : call do_if().
//	            	case "@implementation" : call class_def().
//	            	case "^+" :
//	            	case "^-" : call def_method().
//	            	default   : yyout <- token.
//	        	end switch.
//	        	read token.
//	    	end while.
//	    	return RC_OK.
*/
    int
yyparse( )
{
    extern int    type_def PARMS(( void ));
    extern int    function_body PARMS(( void ));
    extern int    do_if PARMS(( void ));
    extern int    do_imp PARMS(( void ));
    extern int    do_class_forward PARMS(( void ));
    extern int    def_method PARMS(( void ));

    int   rc;	        	/* Any error value */

    rc = RC_OK;
    curr_tok = T_DUMMY;	    /* Start somewhere */

    get_tok( );
    while( rc == RC_OK && curr_tok != DONE )
    {
        switch( curr_tok )
        {
        	case T_TYPEDEF :
                    	rc = type_def();
                    	break ;

        	case T_EXTERN :
                    	rc = extern_ref();
                    	break ;

        	case OPEN_INTERFACE :
                    	if( in_state & IS_INCLUDES )
                    	    gerr( ERROR_RESYNC, GERR_CLASS_LATE, NULL, 0L );

                    	in_state |= IS_SEEN_IF;
                    	in_context |= IC_INTERFACE;
                    	rc = do_if();
                    	curr_method = (struct mynode *)NULL;
                    	in_state   &= ~(IS_SEEN_IF | IS_SEEN_PUBLIC | IS_SEEN_PROTECTED | IS_SEEN_PRIVATE);
                    	in_context &= ~IC_INTERFACE;
                    	break ;

        	case OPEN_IMPLEMENTATION :
                    	in_state |= IS_SEEN_IMP;
                    	rc = do_imp();
                    	in_state &= ~(IS_SEEN_IF | IS_SEEN_IMP | IS_SEEN_PUBLIC | IS_SEEN_PROTECTED | IS_SEEN_PRIVATE);
						in_state |= IS_GOT_IMP;

                    	if( !ocspecial( curr_tok ) )
                        {
                            hash_line(0);
                    	    fputs( curr_name, yyout );
                        }

                    	continue ;

        	case OPEN_PROTOCOL :
                    	in_state |= IS_PROTOCOL;
                    	rc = do_protocol();
                    	in_state &= ~(IS_PROTOCOL | IS_SEEN_PUBLIC | IS_SEEN_PROTECTED | IS_SEEN_PRIVATE);
                    	break ;

        	case OPEN_CLASS :
                    	rc = do_class_forward();
                    	break ;

        	case CLOSE_END :
                    	if ( in_state & IS_GOT_IMP && className != NULL && className[0] != '\0' )
                    	    check_protocol_conformance( className );
                    	goto  END_IMP;   	/* Make system stop */

        	case OPEN_FACTORY_METHOD :
        	case OPEN_INSTANCE_METHOD :
					    if( !(in_state & IS_GOT_IMP) )
						{
                            gerr( ERROR_ERROR, GERR_NO_IMP, NULL, 0L );
						}

                    	rc = def_method();
                    	if( rc == RC_OK )
                    	{
                    	    rc = function_body( );
                    	    curr_method = (struct mynode *)NULL;
                    	    m_order = EOS;
                    	}
                    	break ;

        	case ')' :
                    	/* Found a function with no parameters.. */
                    	if( prev_tok == '(' && get_tok() == CH_LCURLY )
                        {
                            haveFunctYet() ;
                    	    rc = function_body( );
                        }
                    	break ;

        	case ';' :
                    	/* Found function after parameters defined.. */
                    	if( peek_tok() == CH_LCURLY )
                        {
                            haveFunctYet() ;
                    	    rc = function_body( );
                        }
                    	break ;

        	default :
                    	break ;
        }

        /* Advance: */
        get_tok( );

    }	/* while not errors */

  END_IMP :
    /*  Tell caller if we hit end of file, nicely: */
    if( rc == RC_OK && curr_tok == DONE )
        rc = RC_EOF;
    return( rc );
}	/* yyparse */


