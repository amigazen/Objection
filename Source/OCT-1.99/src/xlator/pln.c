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

#define NULL 0

#define  VERSION  "(OCT prelink 1.07B)"

#ifndef  EOS
#  define  EOS  '\0'
#endif



/*  Base name for class table; suffix added for library build (subbind walks _oc_registry). */
#define  CB_ARRAY_BASE	"_oc_allCCB"
#define  OC_REGISTRY	"_oc_registry"

#define  SIZ_BUFFER  120
#define  SIZ_SUFFIX  32
#define  SIZ_WITH    120

/* ----------------------  Externally, Yours  ------------------------ */


/* ---------------------  Globals Based Here  ------------------------ */

FILE	*data_file; 					/* Prelinker output file */

struct List 	module_list;

/*  -suffix <name>: library build; output array _oc_allCCB_<name> (no _oc_registry).  */
static char 	array_suffix[ SIZ_SUFFIX ];
/*  --with <list>: app build; emit externs for lib tables and _oc_registry[] pointing at them.  */
static char 	with_libs[ SIZ_WITH ];


/* --------------------  Our Private Variables  ---------------------- */

static char 	cmt_open[ ] =	{ '/', '*', ' ' , 0 } ;
static char 	cmt_close[ ] =	{ ' ', '*', '/' , 0 } ;


/* -------------------  Under-developed Helpers  --------------------- */


#if SW_NEED_STRDUP

char *
strdup(const char *str)
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
 *                If we fail, we abort.  String ptr copied, not duplicated.
 */
struct Node *
mk_node(char *name)
{
	struct Node 	*np;

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
 *                     and remove the file extension.  Returns basename; caller owns string.
 */
char *
mk_base_name(char *path)
{
	char	*base, *p;

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
 *                  list of modules.  If file not found, a message is printed.
 */
void
must_list(void)
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


/*   mk_table  --  Create extern class table(s).  Library build (-suffix): single array
 *                 _oc_allCCB_<suffix>.  App build: _oc_allCCB plus optional _oc_registry
 *                 that points at library arrays (--with base,amiga) so runtime can find them.
 */
void
mk_table(void)
{
	struct Node 	*np;
	short 	isFirst;
	char 	*cb_array;
	char 	*p;
	char 	*q;

	fprintf( data_file, "%sCreated by %s for class initialization%s\n\n",
				cmt_open, VERSION, cmt_close );

	{
		static char  array_name_buf[64];
		if ( array_suffix[0] != EOS )
			{
				sprintf( array_name_buf, "%s_%s", CB_ARRAY_BASE, array_suffix );
				cb_array = array_name_buf;
			}
		else
			cb_array = (char *)CB_ARRAY_BASE;
	}

	/*  Extern refs for library tables when app uses --with  */
	if ( with_libs[0] != EOS )
		{
			p = with_libs;
			while ( *p != EOS )
				{
					q = p;
					while ( *q != EOS && *q != ',' )
						q++;
					if ( q > p )
						fprintf( data_file, "extern char *_oc_allCCB_%.*s[];\n", (int)(q - p), p );
					p = ( *q == ',' ) ? q + 1 : q;
				}
			fputs( "\n", data_file );
		}

	/*  1.  External References for this table  */
	isFirst = TRUE;
	fprintf( data_file, "extern char " );
	for( np=module_list.lh_Head ; np->ln_Succ != NULL ; np=np->ln_Succ )
		{
			if( ! isFirst )
				fputs( ",\n        ", data_file );
			fprintf( data_file, "_ocCB%s", np->ln_Name );
			isFirst = FALSE;
		}

	/*  2.  Array of pointers to classes  */
	fprintf( data_file, ";\n\nchar  *%s [] = {\n", cb_array );
	for( np=module_list.lh_Head ; np->ln_Succ != NULL ; np=np->ln_Succ )
		fprintf( data_file, "    & _ocCB%s,\n", np->ln_Name );
	fprintf( data_file, "   (char *)0\n} ;\n\n" );

	/*  3.  App only: registry so runtime walks lib tables then app table  */
	if ( array_suffix[0] == EOS )
		{
			fprintf( data_file, "char  **%s [] = {\n", OC_REGISTRY );
			if ( with_libs[0] != EOS )
				{
					p = with_libs;
					while ( *p != EOS )
						{
							q = p;
							while ( *q != EOS && *q != ',' )
								q++;
							if ( q > p )
								fprintf( data_file, "    _oc_allCCB_%.*s,\n", (int)(q - p), p );
							p = ( *q == ',' ) ? q + 1 : q;
						}
				}
			fprintf( data_file, "    %s,\n   (char **)0\n} ;\n\n", CB_ARRAY_BASE );
		}
}	/* mk_table */



void
usage(int rc)
{
	FILE	*uf = stdout;

	fprintf( uf, "%s", RELEASE );
	fprintf( uf, "usage: prelink [ -suffix <name> ] [ --with <lib,list> ] <outCfile> <inMfile>..\n" );
	fprintf( uf, "  -suffix <name>   library build: output _oc_allCCB_<name> (no _oc_registry)\n" );
	fprintf( uf, "  --with <list>    app build: emit _oc_registry[] referencing lib tables (e.g. base,amiga)\n" );
#ifdef MUST_FNAME
	fprintf( uf, "uses file: %s\n", MUST_FNAME );
#else
	fprintf( uf, "No forced includes..\n" );
#endif

	exit( rc );
}	/* usage */


void
parse_cmd(int argc, char *argv[])
{
	char	*name;
	int 	j;

	array_suffix[0] = EOS;
	with_libs[0] = EOS;

	if( argc <= 2 )
		usage( 1 );

	j = 1;
	while ( j < argc && argv[j][0] == '-' )
		{
			if ( strcmp( argv[j], "-suffix" ) == 0 )
				{
					j++;
					if ( j >= argc )
						usage( 1 );
					strncpy( array_suffix, argv[j], SIZ_SUFFIX - 1 );
					array_suffix[SIZ_SUFFIX - 1] = EOS;
					j++;
				}
			else if ( strcmp( argv[j], "--with" ) == 0 )
				{
					j++;
					if ( j >= argc )
						usage( 1 );
					strncpy( with_libs, argv[j], SIZ_WITH - 1 );
					with_libs[SIZ_WITH - 1] = EOS;
					j++;
				}
			else
				usage( 1 );
		}

	if ( j >= argc )
		usage( 1 );

	/*  Output filename  */
	name = argv[j];
	j++;
	if( (data_file=fopen(name, "w")) == NULL )
		{
			printf( "Can't write <%s>!\n", name );
			exit( 10 );
		}

	/*  Remaining args: .m files (or module names)  */
	for( ; j < argc ; ++j )
		{
			name = mk_base_name( argv[j] );
			strlower( name );
			AddTail( & module_list, mk_node( name ) );
		}
}	/* parse_cmd */


/* ----------------------  Main and Drivers  ------------------------- */



int
main(int argc, char *argv[])
{

	puts( VERSION );
	NewList( & module_list );
	must_list();

	parse_cmd( argc, argv );

	mk_table();

	fclose( data_file );

	exit( 0 );
	return 0;  /* not reached */
}	/* main */

