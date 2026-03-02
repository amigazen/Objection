/*  CLASS.C      Some that needs work                     Sept 89, bjw */
/*****************************************************************
 *   This program is copywrite (c) 1989, 1990 by Brian J. Witt.
 *                  rael%aleks@seer.UUCP
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assume
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.  Any distribution
 *   must include this copyright notice.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $15 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
 *    This file contains code relevant to the printing of class
 *    information at the end of the output "C" file.
 * 
 *    Function Dictionary:
 *  		void  grab_class_vars();
 *  		void  out_externs( int end_flag );
 *  		void  dump_dict();
 * 
 *    06-mar-90     bjw 	Initial coding.
 *    15-apr-90     bjw 	Changed base_fname --> base_name.
 *    16-apr-90     bjw 	_SHARED.version[2], _SHARED.clsCount
 *    15-sept-90    bjw 	Aligned Class structure like NeXT stuff.
 *    27-sept-90    bjw     "_SHARED" now "objc_class".
 *    12-feb-91     bjw     All NULL's have typecast on them.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 */

#define  DEBUG  0
#define  SW_WATCH  0
#define  SW_ASSERT  1


#include "oct.h"
#include <tree.h>
#include "error.h"



/*  What should our instance variable type be?
//		(=0) Declared are "struct _ClassName *",
//		(=1) Just use "struct _PRIVATE"
*/
#define  SW_USE_PRIVATE 	1


/*  Class Structure Version ID:
//		0  =  original release
//		1  =  March 90 [Beta Green]
//		2  =  September 90.
*/
#define  CLASS_VERSION	2


/* -----------------  Externals We Take Advantage Of  ------------------ */

extern char   	 	stars[ ];
extern char      	opencmt [ ];
extern char        	closecmt[ ];

extern char   	    * sel_array ;			/* Selector array */
extern char			* base_name ;			/* base name of .m file */



/* -----------------------  Private Things  ---------------------------- */

static char 		ends [ ]  = { "\n} ;\n\n"  };
static char     	s_s  [ ]  = { " struct objc_class " } ;
static char     	tshr [ ]  = { " (struct objc_class *)" } ;
static char     	ssd  [ ]  = { "\nstatic struct objc_method   " } ;


/* ----------------------  Class Variables ----------------------------- */


/*   publicClassVarSTR  --  For this class, and all its parent classes,
 *                          return a string of all instance variables that
 *                          can be accessed by the given class.
 *   RETURNS:    String of public variables sub-classes can reference.
 *               Caller owns string.
 */
    char *
publicClassVarSTR( class_name )
    char    *class_name;
{
	register char	*vars = NULL;  /* Public class instance vars built here */
	register char	*class = class_name;
	struct mynode	*np;

	while( (np=(struct mynode *)searchTree( class_tree, class )) != NULL )
	{
        if( yydebug )
            printf( "class <%s> vars " , class );
		class = np->def;		/* advance to the superclass */

		np = (struct mynode *)searchTree( ivar_tree, np->node.ln_Name );
        if( vars == NULL )
            vars = newstring( np->names );       /* Access all in own class */
        else
            /* ObjC ivars are effectively 'protected' by default: subclasses
             * may reference inherited ivars by bare name (e.g. "contents").
             * Include superclass ivar names so lu_instvar() can rewrite
             * them to self->ivar in translated C.
             */
            vars = newstrcat( vars, np->names );
        if( yydebug )
            printf( "<%s>\n" , vars );
	}

	return( vars );
}   /* publicClassVarSTR */



/*   grab_class_vars  --  From the globals 'className' and 'superclassName',
 * 					  construct a structure declaration that reflects
 * 					  the instance variable for this class.  The name
 * 					  of the selector array is set here.
 */
	void
grab_class_vars()
{
	extern void 	out_externs PARMS(( int end_flag ));	/* scanner */
	register struct mynode	*np;
	char		*ivars;
	char		*cname;
	int  		ndx;

	all_classVars = lu_classdef( className );
	if( all_classVars == NULL )
	{
		error_string = "(grab_class_vars) can't find class def" ;
		gerr( ERROR_ABORT, GERR_INTERNAL, NULL, 0L );
		/*NOTREACHED*/
	}

#if SW_USE_PRIVATE
	out_includes();
	fprintf( yyout, "struct _PRIVATE {  %s\n} ;\n\n",
					lu_classdef(className ) );
#else
	fprintf( yyout, "#define  OC_CLASS_NAME  _%s\n", className );
	out_includes();

	/*  For each known class, create a structure for that name: */
	ndx = -1;
	while( (np=(struct mynode *)enumTree( class_tree, &ndx )) != NULL )
	{
		cname = np->node.ln_Name;
		ivars = lu_classdef( cname );
		fprintf( yyout, "struct _%s {  %s  } ;\n",
					cname, ivars, cname );
		MFREE( ivars );
	}
#endif

	/*  Create forward definition for [super xx] messages: */
	fprintf( yyout, "FORWARD %s   factory%s, inst%s;\n",
				s_s, className, className );

	out_externs( 0 );
    trip_hline = TRUE;                  /* Request re-synch */
}	/* grab_class_vars */



/* -------------------------  Class Maintaince  ----------------------- */


/*   out_externs  --  Just after @implementation, we need to output
 *                    a list of references to external classes we
 *                    depend on.  Helps resolve our 'Class' structs.  All
 *                    classes are printed, not just our superclasses.
 *
 *   Called by:  grab_class_vars(), ooc_includes(), dump_dict().
 */
	void
out_externs( end_flag )
	int 	end_flag;
{
	register struct mynode	*np;
	char	*name;
	int 	j;

	j = -1;
	while( (np=(struct mynode *)enumTree( class_tree, &j )) != NULL )
	{
		if( !(np->flags & DF_IMP) )
		{
			name = np->node.ln_Name;
			if( end_flag )
			{
				/*  Anytime symbol used, replace it (nucense) */
				fprintf( yyout, "extern %s  inst%s;\n", s_s, name );
				fprintf( yyout, "#define %s (&inst%s)\n", name, name );
			} else
			{
				/*  have less symbol clashes (and stick to one namespace) */
				fprintf( yyout, "extern %s  * %s;\n", s_s, name );
			}

		}
	}	/* for each external class */

}	/* out_externs */


/* ------------------------  Trailing Remarks  --------------------------- */



/*   dd_dictionary  --  Print the dictionary structure.  For us, this
 * 				is actually two structures: a control structure
 * 				and an array.  Public name  "<name>Dict"
 * 		RETURNS:  count of instances with type 'id'.
 */
	int
dd_dictionary( cf, name, id, meth_tree )
	register FILE	*cf;
	CONST char		*name;  		/* String identifying type */
	CONST char 		id;     		/* Single char for type */
	void      		*meth_tree;
{
	register struct mynode 	*np;
	int 	endx = -1;
	int 	count = 0;

	/*  This structure name is private: */
	fprintf( cf, "%s da_%s[ ] = {", ssd, name );

	while( (np=(struct mynode *)enumTree( meth_tree, & endx)) != NULL )
	{
		/*	If no id, and method referenced,  OR
		 *  if id provided, and node matches..
		 */
		if( id == 0 )
		{
			if( !(np->flags & DF_IMP) && np->sequence > 0 )
			{
				++count;
				fprintf( cf, "\n    { (SEL)0, (IMP)NULL, \"%s\", %3d },",
						np->node.ln_Name, np->sequence );
			}
		} else
		if( ((np->flags & DF_IMP) && np->order == id ) ||
			 (np->flags & DF_TWO_ORDER) )
		{
			++count;
			fprintf( cf, "\n    { (SEL)0, (IMP) %c%s, \"%s\", %3d },",
					id, asFunc_selector(np->node.ln_Name),
					np->node.ln_Name, np->sequence );
		} 

	}	/* while more entries to examine */
	if( count == 0 )
		fprintf( cf, "\n   { (SEL)0, (IMP)NULL, (STR)0, 0 }" );


	/*  3b.  Print header structures for the dictionaries:
	 *       Some "C" compilers dislike empty arrays.  Output a dummy
	 *       NULL entry in leu of any entries:
	 */
	fprintf( cf,"%sstatic struct objc_method_list  %sDict =\n { (struct objc_method_list *)NULL, %d, %s, ",
				ends, name, count, sel_array );
	if( count == 0 )
		fprintf( cf, "(struct objc_method *)NULL  };\n\n" );
	else
		fprintf( cf, "&da_%s[0]  };\n\n", name );

	return( count );
}	/* dd_dictionary */


/*   dump_dict  --  After translator, output all the tables and structures
 *                  that are used when the application starts up, and
 *                  also for message passing.  Order of dump:
 *      2) Factory and Instance Dictionaries.
 *         Method selector from index array.
 *      3) Class control block
 *      4) Factory and Instance shared structures.
 *      5) Global factory id pointer.
 */
	void
dump_dict()
{
	extern char		*asTypeString PARMS(( char * m_vars ));
	extern int  	max_sel PARMS(( void ));

	register struct mynode	*np;
	register FILE	*cf = yyout;		/* For faster access.. */
	char	*super;
	int  	endx, count;

	hash_line( 0 );     /* Leave where we stoped reading input file */

	/* -------------------------------------- */
	/*  1.  Prepare missing data structures:  */
	/*  Change name to output file:           */
    lineat *= 7;           /* Space way out...*/
	MFREE( src_filename );
	src_filename = newstring( dest_fname );
	hash_line( 0 );

	if( superclassName[0] == EOS )
		super = newstring( "Object" );
	else
		super = newstring( superclassName );


	fprintf( cf, "\n/%s  @end  %s/\n\n", stars, stars );
	out_externs( 1 );

	/* -------------------------------- */
	/*  2.  Print method dictionaries:  */
	/*  The "resolved selector code" array is filled in at runtime: */
	fprintf( cf, "SEL  %s [ %d ] ;\n", sel_array, max_sel() );

	/*  Print dictionary of instance and factory methods: */
	if( in_state & IS_GOT_IMP )
	{
		count = dd_dictionary( cf, "instance", 'I', method_tree );
		endx  = dd_dictionary( cf, "factory",  'F', method_tree );
		if( verbose_flag )
			printf( "Method count: %d factory, %d instance.\n",endx,count); 
	}
	endx = dd_dictionary( cf, "nonimp", (char) 0, method_tree );
	if( verbose_flag )
	{
		printf( "Method analysis: %d external references.\n", endx );
	}


	/* -------------------------------- */
	/*  3.  Print Class Control Block:  */

	fprintf( cf, "\nstatic%s * _depends_list [] = {\n", s_s );
	endx = -1;
	while( (np=(struct mynode *)enumTree( class_tree, &endx )) != NULL )
	{
		if( !(np->flags & DF_IMP) )		/* This class NOT implemented */
			fprintf( cf, "  & inst%s,\n", np->node.ln_Name );
	}
	fprintf( cf, "  %s NULL\n} ;\n", tshr );

	/* -------------------------------- */

	fprintf( cf, "struct class_cb  _ocCB%s = {\n", base_name );
	if( in_state & IS_GOT_IMP )
	{
		fprintf( cf, "  & factoryDict, & instanceDict,\n" );
		fprintf( cf, "  & inst%s,\n", 	className );
	} else
	{
		fprintf(cf,"  (struct objc_method_list *)NULL, (struct objc_method_list *)NULL,\n");
		fprintf(cf, "  %s NULL,\n", tshr );
	}
	fprintf( cf, "  _depends_list,\n" );	/* class_cb.depends */

	/*  Category name: */
	if( in_state & IS_CATEGORY )
	{
		fprintf( cf, "  \"%s\" %s(%s)%s,\n", className,
					opencmt, category_name, closecmt );
	} else
	{
		fprintf( cf, "  (char *)0,\n" );
	}

	/*  Reference methods (referenced, but not defined here): */
	fprintf( cf, "  & nonimpDict, %d%s", max_sel()-1, ends );

	if( !(in_state & IS_GOT_IMP) )
		goto DD_DONE;

	/* ----------------------------- */
	/*  4.  Print Factory Instance:  */
	fprintf( cf, "extern%s  factory%s;\n", s_s, super );
	fprintf( cf, "\n%s  factory%s =\n{\n", s_s, className );

	/*  isa, super_class, name  */
	fprintf( cf, "  &factoryObject, " );
	if( superclassName[0] == EOS )
		fprintf( cf, "&instObject," );
	else
		fprintf( cf, "&factory%s,", super );
	fprintf( cf, "  \"_%s\",\n", 	className );

	/*  version, memory_version, info, clsCount, clsSizeInstance  */
	fprintf( cf, "   %d, 0, (CLS_SETF_SHARED | CLS_META), 0, sizeof(%s),\n",
				CLASS_VERSION,   s_s );

	/*  clsTypes  */
	fprintf( cf, "  \"%s\",  %sclass type%s\n",
					"##*CCSsS*^^^^^",  opencmt,closecmt );

	/*  clsSelects, clsCache, clsControl  */
	fprintf( cf, "  &factoryDict, (struct obj_cache *)NULL, &_ocCB%s,\n",
                  base_name );

	/*  clsCatList, clsDelegate  */
	fprintf( cf, "  %s 0, (IMP) 0%s", tshr,ends );   /* class delegation */


	/* ------------------------------ */
	/*  4.  Print Instance Instance:  */
	fprintf( cf, "\n%s  inst%s =\n{\n", s_s, className );

	/*  isa, super_class, name  */
	fprintf( cf,"  & factory%s, ", className );
	if( superclassName[0] == EOS )
		fprintf( cf, "(Class) NULL," );
	else
		fprintf( cf, "&inst%s,", super );
	fprintf( cf, "  \"%s\",\n", className );

	/*  version, memory_version, info, clsCount, clsSizInstance  */
	fprintf( cf,"  %d, 0, CLS_SETF_SHARED, 0,", CLASS_VERSION );
#if SW_USE_PRIVATE
	fprintf( cf, "  sizeof( struct _PRIVATE ),\n" );
#else
	fprintf( cf, "  sizeof( struct _%s ),\n", className );
#endif

	/*  clsTypes  */
	fprintf( cf, "  \"%s\",\n",  	asTypeString( all_classVars ) );

	/*  clsSelects, clsCache, clsControl  */
	fprintf( cf, "  &instanceDict, (struct obj_cache *)NULL, &_ocCB%s,\n",
                   base_name );

	/*  clsCatList, clsDelegate  */
	fprintf( cf, "  %s 0, (IMP) 0%s", tshr, ends );   /*  delegation */


	/* --------------------------------- */
	/*  5.  Print Trailing Information:  */
	if( in_state & IS_CATEGORY )
	{
		fprintf( cf, "extern %s  inst%s;\n", s_s, className );
		fprintf( cf, "static %s *_oc_PullCatClass = & inst%s;\n\n",
					s_s, className );
	} else
	{
		fprintf( cf, "%sUsable by ordinary \"C\" files:%s\n",
					opencmt, closecmt );
		fprintf( cf, "%s *%s = & inst%s;\n\n", s_s, className, className );
	}

 DD_DONE :

	MFREE( super );

}	/* dump_dict */


