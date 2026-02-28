/*   MDEF.C   Method definitions and declaration             Sept 89, bjw

/*****************************************************************
 *   This program is (c) COPYRIGHT 1989,1990,1992 by Brian J. Witt.
 *                  seer.UUCP!rael!aleks
 *
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responsibility of usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.  A
 *   small fee ( < $15 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *    Here are the routines that handle @interface and @implementation
 *    clauses.  The drive other routines.  They perform various checks.
 * 
 *    Function Dictionary:
 *  		struct mynode * dcl_method();
 *  		int   rd_method_dcl( int flags );
 *  		int   def_method();
 *  		void  do_td_defs( char *class_name );
 *  		int   do_protocol();
 *  		int   do_imp();
 *  		int   do_if( );
 * 
 *    06-sept-89	bjw 	Moved out of gram.c
 *    19-sept-89	bjw 	@imp may end on ";".
 *    26-oct-89     bjw 	Current token name now: curr_name.
 *    28-oct-89     bjw 	Add get_tok() before gram_typecast().
 *    29-oct-89     bjw 	Added do_abstract().
 *    15-apr-90     bjw 	Changed base_fname --> base_name.
 *    30-june-90    bjw 	Private methods have "_" prefix, OK.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    30-may-91     akl/bjw  Finally installed Symmetry mods!
 *    20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
 *    1-july-92    bjw      parse_structure() takes token hint.
 *    2-july-92    bjw      mk_mynode() always sets 'lineDefined.'
 */


#define  SW_DEBUG  0
#define  SW_WATCH  0
#define  SW_ASSERT  1


#include "oct.h"
#include <tree.h>
#include "gram.h"
#include "error.h"


char          * sel_array ;     	/* Selector array */

extern struct List 	category_list;		/* handles class categories */


/* --------------------  Our Private Variables  ---------------------- */


/* -------------------  Under-developed Helpers  --------------------- */


/*   must_token  --  Sets a return code RC_OK if 'curr_tok' matches
//					the expected token.  Otherwise an error message
//					is printed.
*/
	LOCAL int
must_token( who, tok )
    char    *who;
	int 	tok;
{
	int 	rc = RC_OK;

	if( curr_tok != tok )
	{
		if( isascii(tok) && isprint(tok) )
			printf( "error: (%s) wanted \"%c\", but found \"%s\"\n",
						who, tok, curr_name );

		gerr( ERROR_RESYNC, GERR_SYNTAX, er_curly, 0L );
		rc = RC_ERROR;
	}

	return( rc );
}	/* must_token */


/*   out_includes  --  Call our header file again here as all definition
//					   have been read.  Included again to xlat "METHOD_IS",
//					   "NULL", any macros, and provide external class
//					   definitions.
*/
	LOCAL void
out_includes()
{
	extern char     * base_name ;		/* base name of .m file */

	if( !(in_state & IS_INCLUDES) )
	{
		in_state |= IS_INCLUDES;
		sel_array = newstrjoin( "_ocSA", base_name );  /* make name */

		fprintf( yyout, "#include <objc/objc.h>\n" );
		fprintf( yyout, "FORWARD SEL  %s[];\n\n", sel_array );
	}   /* if not already outputted.. */

}	/* out_includes */



/* --------------------  Private Code Workings  ---------------------- */


/*   dcl_method2  --  handle selector items 2,3,4,...  Returns on '{' token.
 *                    Entered when first parameter has ":" trailing it.
 *                    The items and types are collected from lex_text()
 *                    and parsed by the caller.  We manually split the
 *          typedef and the variable names into two strings and return them
 *          in the node structure.  The variable names are space separated.
 *   BUGS:
 *          Needs more extreme testing, like "x:(STR)v1 y:v2 z" <-- error!
 */
	LOCAL struct mynode *
dcl_method2( ret_type, method1 )
	char	*ret_type;		/* Return type of function */
	char	*method1;		/* First part of method name */
{
	enum {	ST_ERROR,
			ST_ID,
			ST_COLON,
			ST_TYPE,
			ST_VAR
	} state;      			/* What we just saw */

	struct mynode	*np;
	char	*name;			/* Selector components */
							/* Returns  "at:put:"		*/
	char	*type;			/* The type of method, then each variable */
							/* Returns  "(id)(int)(id)" */
	char	*var;			/* Variable names (selector names) */
							/* Returns  "offset object" */

	name  = newstrjoin( method1, ":" );
	var   = newstring( NULL );
	type  = ret_type;
	state = ST_COLON;			/* Call with curr_name == ":" */

	while( get_tok() != DONE )
	{
		switch( curr_tok )
		{
			case ':' :
						state = ST_COLON;
						name = newstrcat( name, curr_name );
						break ;

			case IDENTIFIER :
					switch( state )
					{
						case ST_VAR :
								state = ST_ID;
								name = newstrcat( name, curr_name );
								break ;

						case ST_COLON :
								type = newstrcat( type, "(id)" );
								/*- fall through -*/

						case ST_TYPE :
								var = newstrcat( var, curr_name );
								state = ST_VAR;
								break ;

						default :
							goto DM_ERR;
					}	/* Identifer switch(state) */
				 	break ;


			case CH_OPEN_PAREN :
						if( state != ST_COLON )
						{
							gerr( ERROR_RESYNC, GERR_MISSING_VAR, er_curly, 0L );
							return( NULL );
						}

						state = ST_TYPE;
						lex_text( LT_START );
						lex_text( LT_APPEND1, CH_OPEN_PAREN );
						get_tok();         /* Read ahead */
						if( gram_typecast() != RC_OK )
							goto DM_ERR;

						type = newstrcat( type, lex_text(LT_STOP, ' ') );
						break ;

			case OPEN_FACTORY_METHOD :
			case OPEN_INSTANCE_METHOD :
						gwarn( GW_MISSING_SEMI );
						unget_tok();
						/*-- fall through --*/

			case CH_LCURLY :
			case ';' :
						strremove( name, ' ' );
						np = mk_mynode( name, type );
						np->tag = var;

						MFREE( name );
						MFREE( type );
						return( np );				/*  --->  */

			default :
						goto DM_ERR;

		}	/* switch current token */

	} /* while forever.. */

 DM_ERR :
	gerr( ERROR_RESYNC, GERR_SYNTAX, er_curly, 0L );
	return( NULL );
}	/* dcl_method2 */


/*   dcl_method  --  Parse a method header.  Return a node as method name
 *                   and data of parameter types.  First type is the
 *                   return value.  Uses lex_text() to capture all the
 *                   pertainent information.  Returns on closing ";".
 */
	struct mynode *
dcl_method()
{
	char	*name, *type;
	struct mynode	*np = NULL;
	int   rc = RC_OK;				/* Any error value */

	/*  Parse return type of method: */
	lex_text( LT_START );
	switch( get_tok() )
	{
		case IDENTIFIER :
		case PARM_LABEL :
					lex_text( LT_STOP, ' ' );
					type = newstring( "(id)" );
					break ;

		case CH_OPEN_PAREN :
					get_tok();         /* Read ahead */
					rc = gram_typecast();
					type = lex_text( LT_STOP, ' ' );
					if( rc != RC_OK || get_tok() != IDENTIFIER )
						gwarn( GW_TYPECAST );

					break ;

		default :
					gwarn( GW_TYPECAST );
	}

	name = newstring( curr_name );

	/* Parse types of selector arguments: */
	switch( get_tok() )
	{
		case ':' :
					np = dcl_method2( type, name );
					break ;

		case CH_LCURLY :
		case ';' :
					np = mk_mynode( name, type );
					break ;

		default :
					rc = RC_ERROR;
	}

	if( np == NULL || rc != RC_OK )
	{
		gwarn( GW_BAD_SELECTOR );
		np = mk_mynode( "error_name", "(id)" ); 	/* Try to recover */
	} else
	{
		np->order = m_order;
#if SW_WATCH
		printf( "(dcl_method) name<%c %s>, vars<%s>, types<%s>\n",
		            np->order, np->node.ln_Name, np->tag, np->def );
#endif
	}
	return( np );
}	/* dcl_method */



/*  rd_method_dcl  --  Reads in method declares after @interface or
 *                     @protocol clause defines the instance variable
 *                     structure.  Caller gives flags, and each node
 *                     will look that way.
 */
	int
rd_method_dcl( flags )
	int 	flags;
{
	register struct mynode	*np;

	while( curr_tok != CLOSE_END )
	{
		switch( curr_tok )
		{
			case OPEN_FACTORY_METHOD :
			case OPEN_INSTANCE_METHOD :
						np = dcl_method( );
						np->flags = flags;
						addTree( method_tree, (struct Node *)np );
						break ;

			case ';' :
						break ;

			case DONE :
						gerr( ERROR_ABORT, GERR_SUDDEN_EOF, NULL, 0L );
						/*NOTREACHED*/

			default :
#if SW_ASSERT
						printf( "(do_if) bad <%s>\n", curr_name );
#endif
						return RC_ERROR;

		}
		in_state &= ~IS_METHOD_DEF; 	/* undo what lexer gives us */

		if( curr_tok != ';' )
			return RC_ERROR;

		get_tok();
	}	/* while methods being declared */

	return( RC_OK );
}	/* rd_method_dcl */



/* ----------------------  Public Code Works  ------------------------ */


/*   def_method  --  Handle function header, translation body next.
//					 Called inside of @implementation block.
//
//	Input Parameters:
//			none.
//
//	Input Environment:
//			yyout -- opened output plain C file stream.
//			curr_name -- text of recent token.
//
//	Function References:
//			dcl_method(), del_nodes(), dump_m_header().
//
//	Returned Environment:
//			
//	Returned Values:
//			RC_OK = successful translation.
//			RC_ERR = error, last token wasn't '{'
//
//	Psuedo-Code:
//			return RC_OK.
*/
	int
def_method()
{
	extern int   	del_nodes PARMS(( void *tree, struct mynode *model ));

	struct mynode	*methodExtrnDecl;

	in_state |= IS_METHOD_DEF;
	if( className[0] == '\0' )
		gwarn( GW_NO_CLASS );

	methodExtrnDecl = dcl_method();

	/*  Delete any other (possibly mismatching) forward declares: */
	if( ! del_nodes( method_tree, methodExtrnDecl ) )
	{
		/*  If no match found, then method announced is its definition. */
		/*  Private class methods begin with an underscore: */
		if( methodExtrnDecl->node.ln_Name[0] != '_' )
		{
			if( warn_level >= WARN_NORMAL )
				printf("Note: Private method %s\n",str_name_method(methodExtrnDecl));
		}
	}	/* if previouusly unknown method */

	addTree( method_tree, (struct Node *)methodExtrnDecl );

    methodExtrnDecl->lineDefined = lineat;
	methodExtrnDecl->flags |= DF_IMP;
	asCRef_selector( methodExtrnDecl->node.ln_Name );	/* Assign a selector */
	curr_method = methodExtrnDecl;

	dump_m_header( methodExtrnDecl );

	in_state &= ~IS_METHOD_DEF;
	return( must_token( "def_method/finish", CH_LCURLY ) );
}	/* def_method */



/* ----------------------  Inputter Support  ------------------------- */


/*   see_public  --  The scanner saw "@public".  Check for context, and
 *                   if OK, set some flags.
 */
	void
see_public()
{

	if( !(in_context & IC_INTERFACE) )
	{
		error_string = "@public" ;
		gerr( ERROR_RESYNC, GERR_DIRECTIVE_CONTEXT, er_semi, 0L );
	}	/* if @public out of context */

	in_state |= IS_SEEN_PUBLIC ;

}	/* see_public */



/*   do_td_defs  --  Handle Translator Directive "@def()".  Basically,
//					 just forces string to yyout.  The translator needn't
//					 see the substituted text.
*/
	void
do_td_defs( class_name )
	char	*class_name;
{
	char	*defs;

	if( (defs=lu_classdef( class_name )) == NULL )
	{
		error_string = class_name;
		gerr( ERROR_RESYNC, GERR_BAD_DEFS, er_semi, 0L );
	} else
	{
		fputs( defs, yyout );
		MFREE( defs );
	}

}	/* do_td_defs */



/* ----------------------  Main and Drivers  ------------------------- */

/*   do_protocol  --  Handle protocol method defintions, @protocol.
*/
int
do_protocol(void)
{

	in_state |= IS_SEEN_IF ;
	/* Skip class name and such: */
	while( curr_tok != DONE &&
		   curr_tok != CLOSE_END &&
		   curr_tok != OPEN_FACTORY_METHOD &&
		   curr_tok != OPEN_INSTANCE_METHOD )
	{
		get_tok();
	}


	/*  Digest methods this class declares:
	 *  Basically they're external references without having to specify
	 *  a class the methods belong to.  Creates protocol without clutter.
	 */
	if( rd_method_dcl( DF_DECLARED | DF_PROTOCOL ) != RC_OK )
	{
		gerr( ERROR_RESYNC, GERR_METHOD_DECLARE, er_end, 0L );
	}

	in_state &= ~IS_SEEN_IF ;
	return RC_OK;
}	/* do_protocol */



/*   do_imp  --  Handle class implementation.
//
//	Input Parameters:
//			none.
//
//	Input Environment:
//			Parser just read "@implementation"
//			yyout -- opened output plain C file stream.
//			curr_name -- text of recent token.
//
//	Function References:
//			get_tok(), grab_class_vars(), lex_text().
//
//	Returned Environment:
//			"id" added to known types list.
//          'className' set to implemented class name.
//          'superclassName' set to implemented super class' name.
//			Caller must evaluate 'last_token'.
//			
//	Returned Values:
//			RC_OK = successful translation.
//
//	Psuedo-Code:
//			read token().
//			if( token != IDENTIFIER )
//				error <- "syntax error".
//			endif
//			className <- token.
//
//			if( token == ':' )
//				superclassName = read_token().
//			elseif
//			if( token == '(' )
//				skip over category name.
//			endif
//
//			if( token == '{' )
//				ignore the instance variables.
//			endif.
//			decode_class_vars().
//	
//			add "id" to known types list.
//			return RC_OK.
*/
	int
do_imp()
{
	struct mynode	*thisClass;
	int   rc;				/* Any error value */

	rc = RC_OK;
	in_state |= IS_CLASS_DEF;

	if( className[0] != EOS )
	{
		gerr( ERROR_ABORT, GERR_DUP_CLASS, NULL, 0L );
				/*NOTREACHED*/
	}

	/*  Class Name: */
	if( get_tok() != IDENTIFIER )
	{
		gerr( ERROR_ABORT, GERR_DEF_METHOD, NULL, 0L );
				/*NOTREACHED*/
	}

#if SW_WATCH
	printf( "@implementation %s ;\n", curr_name );
#endif

	thisClass = (struct mynode *) searchTree( class_tree, curr_name );
	if( thisClass == NULL )
	{
		gerr( ERROR_ABORT, GERR_NO_INTERFACE, NULL, 0L );
				/*NOTREACHED*/
	}

	/*  Set some globals from the database: */
	className      = thisClass->node.ln_Name;
	superclassName = thisClass->def;

	thisClass->flags |= DF_IMP;			/* This is implemented class */
/**  enter_type( "id", "struct objc_class *" );  -- Removed 2/15 - bjw */

	/*  Output class instance structure quickly before too many
	 *  declarations appear in the user source file!
	 */
	grab_class_vars();

	/*  See if extraneous information included, like superclass and
	//  instance variables.  If so, eat them up (yum yum).
	*/
	get_tok( );
	if( curr_tok == ':' )
	{
		get_tok( );				/* Eat superclass name */
		if( ! STREQ( curr_name, superclassName ) )
			gwarn( GW_DIFFERENT_SUPER );
		get_tok( );				/* Skip to next token */
	} else
	if( curr_tok == '(' )
	{
		get_tok( );				/* find category name */
		category_name = newstring( curr_name );
		in_state |= IS_CATEGORY;

		get_tok( );				/* Eat closing paren */
		get_tok( );				/* Get whatever is next */
	}

	if( curr_tok == CH_LCURLY )
	{
		printf( "Note: Old style i-vars ignored.\n" );
		digest_block();
		get_tok( ); 			/* First token after ivar structure */
	}

	if( curr_tok == ';' )		/* Yes, the "@imp" clause ended */
		get_tok( );

#if 0
    if( curr_tok = '\n' )
        unget_newline();
#endif

	/*  The current token must be retried for a match! */
	in_state &= ~IS_CLASS_DEF;
	in_state |= IS_GOT_IMP;

	return( rc );
}	/* do_imp */



/*   do_if  --  Handle class interfaces, adds to our knowledge base.
//
//	Input Parameters:
//			none.
//
//	Input Environment:
//			+ "(int)searchSTR:(STR)name excluding:aCollection ;"
//			yyout -- opened output plain C file stream.
//			curr_name -- text of recent token.
//
//	Function References:
//			parse_structure();
//
//	Returned Environment:
//			
//	Returned Values:
//			RC_OK = successful translation.
//
//	Psuedo-Code:
//    		if( @public seen )
//    			mark node as public instance variable.
//			return 0.
*/
	int
do_if( )
{
    extern void     enter_instance PARMS(( CONST char *, CONST char *, \
	                           CONST char *, CONST char *, CONST char * ));
	struct mynode	*np;
	char	*class;
	char	*super = NULL;
	char	*type_str = NULL;
	short 	found_category = FALSE;
	int   rc = RC_OK;				/* Any error value */

	if( get_tok() != IDENTIFIER )
	{
		gerr( ERROR_ABORT, GERR_SYNTAX, NULL, 0L );
		/*NOTREACHED*/
	}

#if SW_WATCH
	printf( "@interface %s :\n", curr_name );
#endif

	class = newstring( curr_name );

	switch( get_tok() )
	{
		case '(' :
			get_tok();			/* Read category name */
			found_category = TRUE;
			printf( "..class category(%s)\n", curr_name );

			get_tok();			/* Close paren */
			get_tok();			/* ..and whatever is next */
			if( curr_tok == ':' || curr_tok == CH_LCURLY )
			{
				gerr( ERROR_RESYNC, GERR_CATEGORY_SYNTAX, er_end, 0L );
				goto  IF_ABORT;
			}
			break ;

		case ':' :
			if( get_tok() != IDENTIFIER )
			{
				gerr( ERROR_ABORT, GERR_SYNTAX, NULL, 0L );
				/*NOTREACHED*/
			}
			super = newstring( curr_name );
			get_tok();
			break ;

		case CH_LCURLY :
			break ;

		default :
			gerr( ERROR_RESYNC, GERR_SYNTAX, er_end, 0L );
			goto  IF_ABORT;

	}	/* switch on token after interface class name */


	/*  If duplicate interface encountered, we can display warning: */
	if( searchTree( class_tree, class ) != NULL )
	{
		struct mynode	*newClass;

		if( ! found_category )
		{
			if( verbose_flag )
				printf( "Duplicate interface for %s..", class );
			er_end();
	IF_ABORT :
			MFREE( class );
			return( RC_OK );
		}

		/*  Remember all category's we reference: */
		newClass = mk_mynode( class, category_name );
		newClass->flags = DF_DECLARED;
		AddTail( & category_list, & newClass->node );

	}	/* if duplicate class name */
	else
	{
		/*  Remember class<-->superclass relation:  */
		np = mk_mynode( class, super );
		np->flags = DF_DECLARED;
		addTree( class_tree, (struct Node *)np );
		if( super != NULL )
			MFREE( super );
	}
    super = NULL;      /* This var available */

	/*  By now we must be at either:
	 *  	'{',
	 *  	first method of interface,
	 *      a structure definition of sorts,
	 *  	or '@end'.
     */
	if( curr_tok == CH_LCURLY )
	{
        char    *elements, *publicstr, *varstr;

		lex_text( LT_START );
		lex_text( LT_APPEND1, CH_LCURLY );
		np = parse_structure( 0 );
		type_str = np->node.ln_Name;
        publicstr = np->publicNames;
        elements = np->def;         /* Extract the element names ONLY */

		if( curr_tok != CH_RCURLY )
		{
			gerr( ERROR_RESYNC, GERR_SYNTAX, er_curly, 0L );
			rc = RC_ERROR;
		}	/* if not end of instance structure */
        else
		{
		    varstr = lex_text( LT_STOP, ' ' );
			enter_instance( class, varstr, type_str, elements, publicstr );
		    get_tok( );
		}
        done_mynode( np );
	} else
	{
		/*  Fake an i-var entry.  lu_instvar() requires node!
		 *   This is _not_ an error condition, rather this class
		 *   isn't defining any instance variables..
		 */
		enter_instance( class, " ", "", "", "" );
	}

	/*  Digest methods this class declares: */
	if( rc != RC_OK || rd_method_dcl( DF_DECLARED ) != RC_OK )
	{
		gerr( ERROR_RESYNC, GERR_METHOD_DECLARE, er_end, 0L );
	}

	return( rc );
}	/* do_if */

