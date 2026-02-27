/*   PreLinker.C    Prelink OOC modules, outputs data        Sept 89, bjw */
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
 *    This program creates the list used by the runtime substrate.
 *    When the application starts to run, all the classes are
 *    initialized.  The class (and module) names are simply
 *    read from the command line.  The only file read is MUST_INIT
 *    which includes indirect modules that aren't specified anywhere.
 *    Currently is hold the filename for the I/O portion of the
 *    substrate.
 *
 *    Build requires mystrs.c for strlower() function.
 *  
 *    The output file must be compiled and linked into the final
 *    application.
 *  
 *    29-sept-89   bjw    Initial coding.
 *    28-oct-89    bjw    Reads mustinit file.
 *    08-nov-89    bjw    Name changed to "OCT".
 *    12-apr-90    bjw    structures now named "_ocCB%s".
 *    19-aug-90    bjw    Generated file no longer references errno[1.06].
 *    30-may-91    akl/bjw  Finally installed Symmtry mods!  Uses mystrs.c
 */


static char copyright[]={" (c) copyright Brian Witt, 1989,1990,1991. ALL RIGHTS RESERVED. brian@babbage.ecs.csus.edu " };


#include <octhead.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <objc/release.h>
#include <objc/private/config.h>


/* --------------------  Defining Your Reality  ------------------------- */


#define  SW_NEED_STRDUP    1

#define  DEBUG  0


#define  VERSION  "(OCT prelink 1.07B)"

#ifndef  EOS
#  define  EOS  '\0'
#endif



/*  This name must jive with extern array in subbind.c/_oc_bind(): */
#define  CB_ARRAY	"_oc_allCCB"


#define  SIZ_BUFFER  120


/* ----------------------  Externally, Yours  ------------------------ */


/* ---------------------  Globals Based Here  ------------------------ */

FILE	*data_file; 					/* Prelinker output file */

struct List 	module_list;


/* --------------------  Our Private Variables  ---------------------- */

static char 	cmt_open[ ] =	{ '/', '*', ' ' , 0 } ;
static char 	cmt_close[ ] =	{ ' ', '*', '/' , 0 } ;


/* -------------------  Under-developed Helpers  --------------------- */


#if SW_NEED_STRDUP

	char	*
strdup( str )
	char	*str;
{
	char	*new;

    if( str == NULL )
	    str = "" ;

	new = malloc( strlen(str)+2 );
	strcpy( new, str );
	return( new );
}	/* strdup */

#endif




/*   mk_node  --  Create a node for our list that has an attached name.
//			If we fail, them we abort.  String ptr copied, not duplicated.
*/
	struct Node *
mk_node( name )
	char	*name;			/* String name that's already saved */
{
	register struct Node 	*np;

	if( (np=(struct Node *)malloc( sizeof(struct Node) )) == NULL )
	{
		fputs( "NO CORE!\n", stderr );
		exit( 10 );
	}
	np->ln_Name = name;
#if DEBUG
	printf( "(mk_node) for <%s>\n", name );
#endif

	return( np );
}	/* mk_node */


/* ---------------------  Paths and FileSpecs  ------------------------- */


/*   mk_base_name  --  Extract the base filename.  Strip any path spec,
//					   and remove the file extension.  Assumes there is
//					   a filename to extract.  Otherwise returns a nil
//					   string if nothing to extract.
//		Returns:  string of basename, caller owns string.
*/
	char *
mk_base_name( path )
	char	*path;
{
	register char	*base, *p;
	register unsigned	c;

	/*  Move down and remove path prefix: */
	base = path;
#ifdef MCH_AMIGA
	if( (p=strchr(base,':')) != NULL )
		base = p+1;
#endif

	p = base;
#ifdef MSDOS
	while( (p=strchr(p,'/')) != NULL || (p=strchr(p,'\\')) != NULL )
#else
	while( (p=strchr(p,'/')) != NULL )
#endif
	{
		base = ++p;
	}

	p = strdup( base );
	if( (base=strrchr(p, '.')) != NULL )
		*base = EOS;

	return( p );
}	/* mk_base_name */


/*   must_list  --  Reads in 'must init' module list.  Appends to current
//					list of modules.  If file not found, a message is
//					printed.
*/
	void
must_list()
{
	static char 	buff[ SIZ_BUFFER ];
	FILE	*fp;
	char	*name;

#ifndef MUST_FNAME
	fprintf( stderr, "No forced includes..\n" );
#else
	if( (fp=fopen( MUST_FNAME, "r" )) == NULL )
	{
		printf( "can't access <%s>\n", MUST_FNAME );
	} else
	{
		while( fgets( buff, SIZ_BUFFER-1, fp ) != NULL )
		{
			if( buff[0] != '#' && strlen(buff) > 4+1 )
			{
				name = mk_base_name( buff );
				strlower( name );
				AddTail( & module_list, mk_node( name ) );
			}
		}	/* while not EOF */

		fclose( fp );
	}
#endif
}	/* must_list */


/* ----------------------  Public Code Works  ------------------------ */


/*   mk_table  --  Create the two extern class tables in the output file,
//				'data_file'.  First list is extern definitions, second is
//				the array that the runtime binding will use.  We extern
//				them as characters; they really are class structures.
//		Because of the laziness, this file requires no header files!
*/
	void
mk_table()
{
	register struct Node 	*np;
	short 	isFirst;

	fprintf( data_file, "%sCreated by %s for class initialization%s\n\n",
				cmt_open, VERSION, cmt_close );

    /*  1.  External References:  */
	isFirst = TRUE;
	fprintf( data_file, "extern char " );
	for( np=module_list.lh_Head ; np->ln_Succ != NULL ; np=np->ln_Succ )
	{
		if( ! isFirst )
			fputs( ",\n        ", data_file );
		fprintf( data_file, "_ocCB%s", np->ln_Name );
		isFirst = FALSE;
	}

    /*  2.  Array of pointers to classes:  */
	fprintf( data_file, ";\n\nchar  *%s [] = {\n", CB_ARRAY );
	for( np=module_list.lh_Head ; np->ln_Succ != NULL ; np=np->ln_Succ )
	{
		fprintf( data_file, "    & _ocCB%s,\n", np->ln_Name );
	}

	fprintf( data_file, "   (char *)0\n} ;\n\n" );

}	/* mk_table */



	void
usage( rc )
	int 	rc;
{
	register FILE	*uf = stdout;

	fprintf( uf, "%s", RELEASE );
	fprintf( uf, "usage: prelink <outCfile> <inMfile>..\n" );
#ifdef MUST_FNAME
	fprintf( uf, "uses file: %s\n", MUST_FNAME );
#else
	fprintf( uf, "No forced includes..\n" );
#endif

	exit( rc );
}	/* usage */


	void
parse_cmd( argc, argv )
	int 	argc;
	char	*argv[ ];
{
	char	*name;
	int 	j;

	if( argc <= 2 )
		usage( 1 );

    /*  1.  Argv[1] == Output filename:  */
	name = argv[ 1 ];
	if( (data_file=fopen(name, "w")) == NULL )
	{
		printf( "Can't write <%s>!\n", name );
		exit( 10 );
	}

    /*  2.  Argv[2+] == Class names..  */
	for( j=2 ; j < argc ; ++j )
	{
		name = mk_base_name( argv[ j ] );
		strlower( name );
		AddTail( & module_list, mk_node( name ) );
	}

}	/* parse_cmd */


/* ----------------------  Main and Drivers  ------------------------- */



    void
main( argc, argv )
	int 	argc;
	char	*argv[ ];
{

	puts( VERSION );
	NewList( & module_list );
	must_list();

	parse_cmd( argc, argv );

	mk_table();

	fclose( data_file );

    exit( 0 );
}	/* main */

