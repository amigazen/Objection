/*   INFO.C   Information datakeepers                     sept 89, bjw */
/*****************************************************************
 *   This program is copywrite (c) 1989, 1990 by Brian J. Witt.
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
 *    Routines to handle to pre- and post- headers for the .c output
 *    file are here.  Also code to manage the selector codes and
 *    offsets.
 * 
 *    06-sept-89    bjw    	Initial coding.
 *    23-sept-89    bjw 	Promote method to va-dcl type.
 *    7-oct-89      bjw 	strdup() instead of STRSAVE().
 *    3-nov-89      bjw 	Removed 'impclass_list', mk_class_list(),
 *                          mk_encoding(); not really used.
 *    7-april-90	bjw 	Added some lint-ings.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 */

/*LINTLIBRARY*/

#define  SW_DEBUG  1
#define  SW_WATCH  0
#define  SW_ASSERT     1


#include "oct.h"
#include <tree.h>
#include "error.h"


/*  TRUE to perform @alias checking on method names: */
#define  SW_ALIAS	0



/* ----------------------  Externally, Yours  ------------------------ */

extern struct List 	alias_list;    	/* handles @alias() */

extern char       version[];


/* ---------------------  Globals Based Here  ------------------------ */


char        stars [ ]   = { "******************" } ;
char        opencmt [ ] = { '/', '*', ' ', 0 } ;
char        closecmt[ ] = { ' ', '*', '/', 0 } ;



/* --------------------  Our Private Variables  ---------------------- */


/* ----------------------  Public Code Works  ------------------------ */


/*   encode_type  --  NeXT-style type encoding for @encode(type).
 *  type_str is the text between the parens (may contain spaces).
 *  Returns pointer to static encoding string; "?" for unknown types.
 */
const char *
encode_type(const char *type_str)
{
	static char out[32];
	char norm[64];
	char *p;
	const char *s;

	if ( type_str == NULL || *type_str == '\0' )
		return "?";
	/* Normalize: remove spaces into norm */
	p = norm;
	s = type_str;
	while ( p < norm + sizeof(norm) - 1 && *s != '\0' )
	{
		if ( *s != ' ' && *s != '\t' )
			*p++ = (char)*s;
		s++;
	}
	*p = '\0';

	/* Object and pointer to object */
	if ( strcmp( norm, "id" ) == 0 )
		return "@";
	if ( strcmp( norm, "id*" ) == 0 )
		return "^@";
	/* Integral */
	if ( strcmp( norm, "int" ) == 0 )
		return "i";
	if ( strcmp( norm, "unsignedint" ) == 0 )
		return "I";
	if ( strcmp( norm, "long" ) == 0 )
		return "l";
	if ( strcmp( norm, "unsignedlong" ) == 0 )
		return "L";
	if ( strcmp( norm, "short" ) == 0 )
		return "s";
	if ( strcmp( norm, "unsignedshort" ) == 0 )
		return "S";
	if ( strcmp( norm, "char" ) == 0 )
		return "c";
	if ( strcmp( norm, "unsignedchar" ) == 0 )
		return "C";
	if ( strcmp( norm, "float" ) == 0 )
		return "f";
	if ( strcmp( norm, "double" ) == 0 )
		return "d";
	if ( strcmp( norm, "void" ) == 0 )
		return "v";
	/* char * */
	if ( strcmp( norm, "char*" ) == 0 )
		return "^c";
	/* Unknown: return ? */
	out[0] = '?';
	out[1] = '\0';
	return out;
}


/*   expose_bracket  --  Strip off one level of curly braces from the
 *                       string.  Callers owns returned string.
 *   Return:  string without outter "{..}" pair.
 */
    LOCAL char *
expose_bracket( struc_def )
    char	*struc_def;
{
    register char	*p;
    char	*just_struc;        	/* Return:  string w/out {..} */

    if( strlen( struc_def ) <= 4 )
    {
        just_struc = newstring( "" );  	/* at least "{x y;}" */
    } else
    {
        p = struc_def + 1;
        SKIP_SPACE( p );
        just_struc = newstring( p );
        p = just_struc + strlen( just_struc );
        while( --p > just_struc && (isspace(*p) || *p == CH_RCURLY ) )
        {
            *p = EOS;
        }
    }

    return( just_struc );
}	/* expose_bracket */



/*   lu_classdef  --  For the named class, return the complete instance
 *        		 	  variable definition.  The string is owned by the
 *        	    	  caller, and consists of all instance variable
 *        	    	  portions for the named class and all its superclasses.
 *          Climbs from named class to all superclasses.
 *
 *    	RETURNS:  NULL if class not found, else struct elements.
 *    	     Ie:       "Class isa;\nshort capacity; id *datum;"
 *
 *     	GLOBAL:    all_classVars
 *
 *     	CALLED BY: mdef.c:do_td_defs()
 *     	           class.c:grab_class_vars()
 *     	           symbol.c:asTypeString()
 */
    char *
lu_classdef( class_name )
    char	*class_name;
{
    register char	*acv = NULL;        	/* All class vars built here */
    register char	*class = class_name;
    struct mynode	*np;
    char	*defs;      	   /* Instance Var definition for each class */

    while( (np=(struct mynode *)searchTree( class_tree, class )) != NULL )
    {
        class = np->def;    	/* advance to the superclass */

        np = (struct mynode *)searchTree( ivar_tree, np->node.ln_Name );
        defs = expose_bracket( np->def );
        acv = newstrins( defs, acv );
        acv = newstrins( "\n", acv );

        MFREE( defs );
    }

    return( acv );
}	/* lu_classdef */



/* -------------------------  File Processors  -------------------------- */


/*   pre_ops  --  Any presetting or file conditioning that needs doing
 *                before parsing.  Can't do much here since nothing defined.
 *        		  Outputs a quickie for Lint.
*/
    void
pre_ops( fout, stat_flag )
    FILE	*fout;
    int 	stat_flag;        	/* TRUE if dumping statistics */
{

    fprintf( fout, "%s \"%s\" compiled by %s %s\n/%cLINTLIBRARY%c/\n\n",
                opencmt, from_fname, version, closecmt, '*', '*' );
    if( ! stat_flag )
    {
        fprintf( yyout, "#define _OOC_PASS2  1\n" );
    }
}	/* pre_ops */



/*   post_ops  --  Cleanup the file stream processing.  Can prints stats.
*/
    void
post_ops( )
{
    extern void  *symtab;

    register struct mynode	*np;
    register FILE	*lf;
    char	   *str;
    auto int 	j;

    if( list_flag && className[0] != EOS )
    {
        lf = must_open( from_fname, "w", ".lst" );

        if( superclassName[0] == EOS )
            superclassName = newstring( "<none>" );

        pre_ops( lf, 1 );
        fprintf(lf, "%s%s\n", stars, stars );
        fprintf(lf, "Class name:    %s.\n", className );
        fprintf(lf, "Super Class:   %s.\n", superclassName );
        fprintf(lf, "Complete instance vars: { %s }\n", all_classVars );

        np = (struct mynode *) searchTree( ivar_tree, className );
        str = expose_bracket( np->def );
        fprintf( lf, "\nLocal instance vars:\n%s\n\n", str );
        MFREE( str );

        fprintf( lf, "These methods are implemented:\n" );
        j = -1;
        while( (np=(struct mynode *)enumTree( method_tree, & j)) != NULL )
        {
            if( np->flags & DF_IMP )
                fprintf( lf, "%-28s - %s\n", str_name_method(np), np->def );
        }

        if( yydebug || verbose_flag )
        {
            /*  Print typenames, 3 columns: */
            fprintf( lf, "\nTypedef names..." );
            if( td_nest > 0 )
                fprintf( lf, " typedef nest cnt=%d", td_nest );
    
            j = -1;
            while( (np=(struct mynode *)enumTree( symtab, & j)) != NULL )
            {
#if SW_DEBUG
                fprintf( lf, "\n%18s : %s", np->node.ln_Name, np->def );
#else
                if( (j % 3) == 0 )
                    fputc( '\n', lf );
                fprintf( lf, " %-20s", np->node.ln_Name );
#endif
            }
        }

        fputs( "\n\n", lf );

        fprintf( lf, "These classes were referenced: (class: .super.)\n" );
        j = -1;
        while( (np=(struct mynode *)enumTree( class_tree, & j)) != NULL )
        {
            fprintf( lf, "\t%s: .%s.\n", np->node.ln_Name, np->def );
        }

    }	/* listing info file requested */

}	/* post_ops */



/* -------------------------  Selector Support  ----------------------- */


/* Index into _oc_sa_ array, resolved into selector at run-time. */
static int      	nxt_sel_code = 1;


/*   max_sel  --  Hide our counter.  Returns current value.
*/
    int
max_sel()
{
    return( nxt_sel_code );
}



/*   oo_code  --  Convert text declared name into an internal symbol that
 *        		  references the method.
 *    	Return:  string name in global 'buff'.
*/
    LOCAL char *
oo_code( colon_name )
    char	*colon_name;
{
    extern char 	*sel_array;
    extern char  	opencmt [ ];
    extern char  	closecmt[ ];
    struct mynode	*np;

    if( (np=(struct mynode *)searchTree(method_tree, colon_name)) == NULL )
    {
        sprintf( buff, "unknown method: %s.", colon_name );
        error_string = buff;
        gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
        /*NOTREACHED*/
    }
    if( np->sequence == 0 )
        np->sequence = nxt_sel_code++;

    sprintf( buff, "(%s%s%s%s[%d])", opencmt, colon_name, closecmt,
                sel_array, np->sequence );

#if SW_WATCH
    if( verbose_flag )
    {
        printf( "%s\n", buff );
    }
#endif

    return( buff );
}	/* oo_code */


/*   asFunc_selector  --  Change a colon name (at:put:) into a "C"
 *        	    	      function name.  Caller should append I or F
 *        	    	      for function class.  Creates new string.
 */
    char *
asFunc_selector( colon_name )
    CONST char	*colon_name;
{
    register char	*s;
    char	*mysel;         	/* Returned; User owns copy */

    mysel = newstring( colon_name );
    for( s=mysel ; *s != EOS ; ++s )
    {
        if( *s == ':' )
            *s = '_' ;
    }

    return( mysel );
}	/* asFunc_selector */


/*   asCRef_selector  --  From a colon name string, convert to a string that
 *        	    	      is "C" code to reference that selector.
 */
    char *
asCRef_selector( colon_name )
    char	*colon_name;
{
#if SW_ALIAS
    register struct mynode	*np;
    register char    	*new_name;

    new_name = colon_name;
    while( (np=(struct mynode *)FindName( &alias_list, new_name )) != NULL )
    {
        new_name = np->def;
    }
# if SW_ASSERT
    if( new_name == NULL || new_name[0] == EOS )
    {
        printf( "(asCRef_selector) empty selector name for <%s>\n",
                    colon_name );
        clean( 10 );
        /*NOTREACHED*/
    }
# endif

    return( oo_code( new_name ) );
#else
    return( oo_code( colon_name ) );
#endif
}	/* asCRef_selector */



/*   mk_meth_alias  --  Create an alias binding for method names.
 *   Implements the @alias(real, alias) compiler directive.  When SW_ALIAS
 *   is set, asCRef_selector() resolves alias names to the real method.
 */
    void
mk_meth_alias(const char *real_name, const char *alias_name)
{
    AddTail( & alias_list, & mk_mynode( real_name, alias_name )->node );
}	/* mk_meth_alias */



/*   lu_sel_type  --  Given a colon-using selector name, return a string
//        	    	  to the return type.  Returns NULL if the method
//        	    	  hasn't been defined.  Returned string is owned
//        	by the caller, and is the type name without surrounding "(..)"
//        	If a method name with "vaDcl:" attached to its end is
//        	found when the supplied name isn't found, then the method
//        	is promoted!
//        	NOTE: we can't use out_meth_string() since no node has been
//        		  allocated yet (caller will search *next*).
*/
    char *
lu_sel_type( psel )
    char	**psel;      	/* I/O: colon method name */
{
    extern char 	*extract_typecast PARMS(( char *str ));
    extern void     *method_tree;

    struct mynode	*np;
    char	*sel2;

    if( ! (np=(struct mynode *)searchTree( method_tree, *psel )) )
    {
        if( strstr( *psel, "vaDcl:" ) != NULL )
        {
            return NULL;	/* String already has var-dcl within it... */
        }
        sel2 = newstrjoin( *psel, "vaDcl:" );
        if( ! (np=(struct mynode *)searchTree(method_tree,sel2)) )
        {
            MFREE( sel2 );
            return NULL;    	/* Not even var-dcl found... :-( */
        }

        if( warn_level > WARN_NORMAL )
            printf( "Note: promoting [ %s ] to va-dcl\n", *psel );

        MFREE( *psel );
        *psel = sel2;                /* Promote to va-dcl */
        in_context |= IC_VADCL;      /* Tell caller what we did */
    }

    extract_typecast( np->def );
    return( newstring(buff) );          /* return "type" of method */
}	/* lu_sel_type */



/*   add_generic  --  The routine m_call() encountered a method that
 *        	    	  isn't in our list.  Fake definition that returns
 *        	    	  type "id".  Generic is added to known lists.
 */
    void
add_generic( colon_name )
    char	*colon_name;
{

    register struct mynode	*np;

    np = mk_mynode( colon_name, "(id)" );
    np->flags = DF_GENERIC;
    np->order = lineat;
    addTree( method_tree, (struct Node *)np );

}	/* add_generic */


