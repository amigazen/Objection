/*   BUILDMF.C   Build up customized Makefile's             june 91, bjw */

/*
 *    This program builds Makefile's that are customized to a particular
 *    site.  Basically, it merges a site's specific definitions into a
 *    generic, but directory specific, site-indepedent Makefile.  An input
 *    file describes that actions necessary.  This replaces a 'makefile.cpp'
 *    file setup.
 *    
 *    5-june-91    bjw    Initial coding.
 */



#include <octhead.h>
#include <objc/release.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define  MALLOC(st)   ((st *)malloc(sizeof(st)))

#ifdef AZTEC_C
/*  Some ANSI rtl like functions:  */
# define  memset(m,v,c)   setmem(m,c,v)
# define  CONST
#else
# define  CONST  const
#endif


struct mynode
{
	struct Node		node;
	char			flags;		/* DF_xxx */
	char			order;		/* '+' or '-' */
	char   			*def;		/* Complete opening definition */
	char			*tag;		/* misc info  */
	long  			line;		/* Line method declared on */
} ;

#define  DF_GENERIC 	0x04		/* Referenced, but not declared */
#define  DF_DECLARED	0x08		/* Declared in @interface */



/* ----------------------  Externally, Yours  ------------------------ */


/* ---------------------  Globals Based Here  ------------------------ */

struct List 	method_list;				/* Public methods */

char		buff[ 200 ];

char		*this_class;				/* Class we're inspecting */
char		*superclass;


/* --------------------  Our Private Variables  ---------------------- */


/* -------------------  Under-developed Helpers  --------------------- */


#ifndef __STDC__

	char *
strdup( str )
	char	*str;
{
	char	*new;

	new = malloc( strlen(str)+2 );
	strcpy( new, str );
	return( new );
}	/* strdup */
#endif


/*   nxt_word  --  Move up to a word.  Remember this location.  Move past
//				   the word, then store a zero byte.  Returns pointer to
//				   word found.  Passed pointer is incremented to point
//				   just past the zero byte.
*/
	char *
nxt_word( pp )
	char	**pp;
{
	register char	*s;
	char	*first;

	/*  If case there is no string: */
	if( **pp == EOS )
		return "" ;

	/*  Skip upto next word:  */
	for( s=*pp ; isspace(*s) ; ++s )
		;;
	first = s;

	/*  Move to end of word:  */
	for( ++s; !isspace(*s) ; ++s )
		;;

	if( *s != EOS )
	{
		/*  if more to inspect.. */
		*s = EOS;
		*pp = ++s;
	} else
	{
		/*  Hit real end of string.. */
		*pp = s;
	}

	return( first );
}	/* nxt_word */


/* ------------------------ 'mynode' Support  -------------------------- */


/*   mk_mynode  --  Store a key-value pair onto a list node.  The string
//				    values are saved here, so both strings can be from
//				    static buffers.  If no memory, this routine aborts.
*/
	struct mynode *
mk_mynode( name, value )
	char	*name, *value;
{
	register struct mynode  *np;

	if( (np=MALLOC(struct mynode)) == NULL )
	{
		write( 2, "Core - mynode!\n", 15 );
		exit( 10 );
	}
	memset( np, 0, sizeof(struct mynode) );

	np->node.ln_Name = strdup( name );
	np->def          = strdup( value );

	return( np );
}	/* mk_mynode */



/* --------------------  Private Code Workings  ---------------------- */


/* ----------------------  Public Code Works  ------------------------ */


	void
filter( fp )
	FILE	*fp;
{
	struct mynode	*np;
	long	line_number;
	char	*p;
	int  	len_imp;

	superclass = "" ;
	line_number = 0;
	len_imp = strlen( imp_phrase );

	while( fgets( buff, sizeof(buff)-1, fp ) != NULL )
	{
		++line_number;
		if( strncmp( buff, imp_phrase, len_imp ) == 0 )
		{
			p = buff + len_imp; 			/* skip "@implementation" */
			this_class = strdup( nxt_word( & p ) );

			if( (p=strchr( p, ':' )) != NULL )
			{
				++p;
				superclass = nxt_word( & p );
				if( (p=strrchr( superclass, ';' )) != NULL )
					*p = EOS;
				superclass = strdup( superclass );
			}
		} else
		if( buff[0] == '-' || buff[0] == '+' )
		{
			if( (p=strrchr( buff, '\n' )) != NULL )
				*p = EOS;
			np = mk_mynode( buff, "" );
			np->order = buff[0];
			np->flags = DF_DECLARED;
			np->line  = line_number;
			AddTail( & method_list, np );
		}

	}	/* while not eof */

}	/* filter */


/* ----------------------  Output Formatting  ------------------------- */


/*   printMethods  --  Print external declarations for methods.  Private
//                     methods begin with an underscore.
*/
	void
printMethods( outf, methods, public_flag )
	FILE	*outf;
	struct List  *methods;
	int      public_flag;
{
	register struct mynode	*np;
	char	*name;

	if( public_flag )
		fprintf( outf, "\n%s%s  Public  Methods  %s%s\n\n",
				cmt_open, dash, dash, cmt_close );
	else
		fprintf( outf, "\n%s%s  Private Methods  %s%s\n\n",
				cmt_open, dash, dash, cmt_close );

	for( np=(struct mynode *)method_list.lh_Head ;
		 np->node.ln_Succ != NULL ;
		 np=(struct mynode *)np->node.ln_Succ )
	{
		name = np->node.ln_Name;
		if( name[0] == '_' ^ public_flag )
			fprintf( outf, "%s ;\n", np->node.ln_Name );
	}
	fprintf( outf, "\n" );
}	/* printMethods */


/*   printAll  --  Print the interface file.  A header is followed by the
 *                 public methods then the private methods.  If an
 *                 interface already exists, we don't read it :-(.
 *      Caller maintains 'outf'.
 *      An empty superclass turns in 'Object'.
 */
	void
printAll( outf )
	register FILE	*outf;
{

    if( superclass == NULL || superclass[0] == 0 )
	    superclass = "Object" ;

	fprintf( outf, "%s class %s:  %-40s%s\n",
				cmt_open, if_phrase, this_class, cmt_close );

	fprintf( outf, "\n#import  <objc/%s.h>\n", superclass );

	fprintf( outf, "\n%s%s : %s\n{\n\t%sinsert ivars here%s\n}\n\n",
				if_phrase, this_class, superclass, cmt_open, cmt_close );

	printMethods( outf, &method_list, 1 );
	printMethods( outf, &method_list, 0 );

	fprintf( outf, "\n@end\n\n" );

}	/* printAll */


/* ----------------------  Main and Drivers  ------------------------- */


	void
main( argc, argv )
	int 	argc;
	char	*argv[];
{
	FILE	*inf = NULL;

	fputs( "(mkif 0.05)\n", stderr );
	if( argc != 2 )
	{
		puts( RELEASE );
		printf( "usage: %s method_file\n", argv[0] );
		printf( 
			"  Reads method file to produce a skeleton %sfile on stdout.\n",
					if_phrase);
		exit( 5 );
	}

	if( (inf=fopen(argv[1], "r")) == NULL )
	{
		printf( "error: %s - can't read <%s>\n", argv[0], argv[1] );
		exit( 5 );
	}

	NewList( & method_list );
	filter( inf );
	if( this_class[0] == EOS )
	{
		printf( "error: %s - no %s clause found!\n", argv[0], imp_phrase );
		exit( 10 );
	}

	printAll( stdout );

	fclose( inf );
}	/* main */

