/*    HATE.C      Some that needs work                     Sept 89, bjw */

/*****************************************************************
 *   This program is copyright (c) 1989,1990,1991,1992 by Brian J. Witt.
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
 *    Function Dictionary:
 *        char * extract_typecast( char	*types );
 *        int    del_nodes( void *tree, struct mynode  *model_node );
 *        void   dump_m_header( struct mynode	*np );
 *     
 *    10-oct-89     bjw     Initial coding.
 *    4-nov-89      bjw     Removed 'PRIVATE' type, now struct _ClassName.
 *    03-mar-90     bjw     Added class delegates.
 *    06-mar-90     bjw     Moved class relevant code to class.c
 *    27-sept-90    bjw     "SHR" is now "Class".
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
 */


#define  DEBUG  0
#define  SW_WATCH  0
#define  SW_ASSERT  1


#include "oct.h"
#include <tree.h>
#include "gram.h"
#include "error.h"


/* -----------------  Externals We Take Advantage Of  ------------------ */


/* -----------------------  Private Things  ---------------------------- */


/*   extract_typecast  --  Copy out a typecast, removing the opening and
 *                         closing parens.  Stores result in global 'buff'.
 *                         The opening paren can be missing, that's OK.
 *      Return:  pointer just after last typecast.  if *s==0, End.
 */
	char *
extract_typecast( types )
	char	*types;
{
	register char	*s, *b;
	register int 	nest;

	b = buff;
	s = types;

	SKIP_SPACE( s );
	if( *s == CH_OPEN_PAREN )
		s++;

	for( nest=1 ; *s != EOS && nest > 0 ; ++s )
	{
		if( *s == CH_OPEN_PAREN )
			++nest;
		else
		if( *s == CH_CLOSE_PAREN )
			--nest;
		else
			*b++ = *s;
	}
	*b = EOS;

	return( s );			/* Return pointer to next "(" */
}	/* extract_typecast */



/* --------------------------  From MAIN.C  --------------------------- */


/*   del_nodes  --  Search tree for matching "name and order" nodes,
 *                  removing these.  Those that match only name are
 *                  removed when encountered, and then added back in
 *                  when the search is done.  Kludge since tree has
 *                  no notion of current node.
 *
 * Selector index:  All nodes with matching name are tested if
 *                  they have a selector index assigned.  This value is
 *                  propagated to the caller's 'model' node.
 * 
 *        Returns:  TRUE if exactly matching node found.
 */
	int
del_nodes( tree, model_node )
	void	*tree;
	struct mynode	*model_node;
{
	register struct mynode	*np;
	struct mynode	*other_order = NULL;    /* Method for other order */
	char	*name;
	char	*ret_type;		/* Return type of model method */
	char	*arg_type;  	/* Argument types of model method */
	char	*node_arg_type;
	int 	index;
	short 	found = FALSE, hasError = 0;

	name = extract_typecast( model_node->def );
	strremove( buff, ' ' );
	ret_type = newstring( buff );		/* The return type. */
	strcpy( buff, name );
	strremove( buff, ' ' );
	arg_type = newstring( buff );		/* The argument types. */

	name = model_node->node.ln_Name;
	index = model_node->sequence;

#if SW_WATCH
	printf( "(del_nodes) model: %s( %s )\n",
				model_node->node.ln_Name, model_node->def );
#endif

	while( (np=(struct mynode *)searchTree(tree,name)) != NULL )
	{
		Remove( (struct Node *)np );

		/*  Inherit the sequence number if it has already been assigned: */
		if( np->sequence != 0 )
		{
			if( index == 0 )
			{
				index = np->sequence;
			} else
			if( np->sequence != index )
			{
				error_string = "(del_nodes) bad sequence value" ;
				gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
				/*NOTREACHED*/
			}
		}	/* if dup node has sequence value */

		/*  For any given method, the return type must match always: */
		node_arg_type = extract_typecast( np->def );
		strremove( buff, ' ' );
		if( !(hasError&0x01) && ! STREQ( ret_type, buff ) )
		{
			error_string = np->node.ln_Name ;
			gwarn( GW_METHOD_RET_TYPE );
			hasError |= 0x01;			/* (a local flag) */
		}

		/*  For any given method, the argument types must match always: */
		strcpy( buff, node_arg_type );
		strremove( buff, ' ' );
		if( !(hasError&0x02) && ! STREQ( arg_type, buff ) )
		{
			error_string = np->node.ln_Name ;
			gwarn( GW_METHOD_ARG_TYPE );
			hasError |= 0x02;
		}


		/*  If everything matches, remove previous node: */
		if( np->order == model_node->order )
		{
#if SW_WATCH
			printf( "(del_nodes)   %s( %s )\n", np->node.ln_Name, np->def );
#endif
			found = TRUE;
            done_mynode( np );
		} else
		{
			other_order = (struct mynode *) np;
		}
	}

	model_node->sequence = index;
	if( other_order != NULL )
	{
		/*  All nodes for this method name have been removed.
		//	Mark the callers node as being of two spheres.
		*/
		if( other_order->flags & (DF_TWO_ORDER | DF_IMP | DF_PROTOCOL) )
		{
			model_node->flags |= DF_TWO_ORDER;
			found = TRUE;
		}

		MFREE( other_order );
	}

	MFREE( ret_type );
	MFREE( arg_type );
	return( found );
}	/* del_nodes */


/* --------------------  Function Header Code  ----------------------- */


/*   dump_m_header  --  From the node, dump function name, variable list,
 *                      and type definitions for variables.  Function name
 *                      is from np->node.ln_Name.  Types are from np->defs.
 *                      The variable names are from np->tag.
 *      ie, "at:put:", "(id)( int )(id)",  "offset object".
 *      
 *      cmt - this routine should re-parse the arguments.  It should
 *            have the grammar divide them up beforehand.
 */
	void
dump_m_header( np )
	struct mynode	*np;
{
	register char	*pt, *pv;
	char	*front;
	char	*name;			/* Function name in "C" syntax */
	char	*var;			/* Parameter list of function */
	char	*type;			/* List of variable definitions */

	if( verbose_flag )
		puts( str_name_method( np ) );
	if( yydebug )
	{
		printf( "vars=<%s>,types=<%s>\n", np->tag, np->def );
	}
#if SW_ASSERT
	if( np->sequence == 0 )
	{
		printf( "method %s has no sequence!\n", np->node.ln_Name );
	}
#endif

	/*  Change external name into internal "C" function name: */
	name = asFunc_selector( np->node.ln_Name );		/* we'll own string */

	/*  Build parameter list and declarations of vars: */
	var = newstring( "self,_cmd" );		/* Space for first comma */
	pv  = newstrjoin( np->tag, " " );
	if( np->order == 'F' )
		type = newstring( "REG_FACT_SELF Class self;   SEL _cmd" );
	else
		type = newstring( "REG_INST_SELF id  self;   SEL _cmd" );
	pt = newstring( np->def );
	pt = extract_typecast( pt );		/* Skip method's return value */

	while( *pt != EOS )
	{
		pt = extract_typecast( pt );
		if( (front=strchr(pv,' ')) == NULL )
		{
			error_string = "(dump_m_header) type/var count mismatch" ;
			gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
			/*NOTREACHED*/
		}

		*front = EOS;
		type = newstrcat( type, ";   " );
		type = newstrcat( type, buff );
		type = newstrcat( type,  pv );

		var =  newstrcat( var, "," );
		var =  newstrcat( var,  pv );

		pv = front+1;
		SKIP_SPACE( pv );
	}	/* while more types to parse */


	/* ------------------------------------------------------ */
	/*  Now output the three portions of the "C" declaration: */

	extract_typecast( np->def );
	fprintf( yyout, "  METHOD_IS %s\n%c%s(%s)  %s;\n",
				buff, np->order, name, var, type );

	/*  Since we've already parsed the opening curly for this function,
	//  but haven't read the newline after it, technically we're right
	//  at the curly.  Thus, we can output the #line directive before
	//  echoing out the curly brace.
	*/
#if 0
	hash_line( 0 ); 			/* Declare source file position */
#else
    trip_hline = 1;
#endif
	putc( CH_LCURLY , yyout );

	yywhitespace();

	MFREE( name );
	MFREE( var );
	MFREE( type );
}	/* dump_m_header */


