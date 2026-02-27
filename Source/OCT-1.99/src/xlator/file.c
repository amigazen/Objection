/*   FILE.C    File system support for OCT processor...      July 89, bjw */
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
 *    General purpose routines to talk with the filing system.
 *    cpp_infile() exec()'s a program to change some file out there.
 *    This file is used by the translator and the pre-linker.
 *
 *    Includes Symmetry mods by Rob Tillotson (akl@mace.cc.purdue.edu)
 * 
 *    4-aug-89      bjw 	Added some comments...
 *    21-sept-89    bjw 	Added mk_base_name();
 *    27-oct-89     bjw 	Added my_maketemp();
 *    07-nov-89     bjw 	Using isudir( char * path );
 *    2-sept-90     bjw 	One function for path splicing.
 *    8-oct-90      bjw 	Added mygetenv() for Amiga Aztec version.
 *    14-dec-90     bjw 	Filesystem dependencies now in files.h.
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *    28-May-91     bjw     <ctype.h> included from "oct.h"
 *    30-may-91     akl/bjw  Finally installed SYmmtry mods!  Improved
 *                          Amiga's getenv() funct.
 *    1-june-92     bjw     EV_TEMPDIR now string list.
 */

/*
 *    This file adapts itself to these environments.  Please ensure
 *    that an appropriate #define symbol has been defined.
 *        MSDOS  --  Microsoft DOS environment
 *        UNIX   --  Xenix, Unix, bsd, Posix
 *        MCH_AMIGA  --  Commodore-Amiga, vers 1.2 +
 * 
 *    The function "isudir()" tests a path for special-ness (pipe or dir).
 *    Name is from "zoo" program.
 * 
 *    For MS-DOS, path strings are allowed to have forward and back-slashes.
 *    The shell (command.com) doesn't like "/", but MS-DOS understands it.
 */


#define  SW_ASSERT  1
#define  SW_WATCH  0
#define  SW_DEBUG  0


#include "oct.h"
#include <fcntl.h>

#ifdef MCH_AMIGA
# include <libraries/dos.h>
#endif
#ifdef UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#endif



/* -----------------------  Private Variables  ------------------------- */


/* ----------------------  Programmer Supporters  --------------------- */


#if SW_NEED_ISUDIR

/*   isudir  --  Return true if 'path' is directory path specifier. */
	int
isudir( path )
	CONST char	*path;
{
	struct stat	stbuf;
	int  	rc = FALSE;

	stbuf.st_mode = 0;
	stat( path, & stbuf );
	if( (stbuf.st_mode & S_IFMT) == S_IFDIR )
		rc = TRUE;

	return( rc );
}   /* unix - isudir */
#endif


#ifdef MCH_AMIGA

/*   isudir  --  Return true if 'path' is directory path specifier.
 *               This version is AmigaDOS specific.
 */
	int
isudir( path )
	CONST char	*path;
{
	extern long 	Lock PARMS(( char *, long ));
	extern long 	Examine PARMS(( long, void *));

	struct FileInfoBlock   *fib;
	long	dlock = NULL;
	int  	rc = FALSE;

	fib = (struct FileInfoBlock *)malloc( sizeof( struct FileInfoBlock ) );
	if( (dlock=Lock( path, ACCESS_READ )) != NULL )
	{
	    fib->fib_DirEntryType = 0;  /* Set default in case Examine() fails */
		Examine( dlock, fib );
		if( fib->fib_DirEntryType > 0 )
			rc = TRUE;
		UnLock( dlock );
	}
	free( fib );

	return( rc );
}   /* amiga - isudir */



/*   system  --  Run a program, wait for completion.  Returns 0 if OK.
//               Returns -1 if failure of some sort.
*/
	int
system( cmdline )
	CONST char	*cmdline;
{
	extern long 	Output PARMS(( void ));
	extern long 	Execute PARMS(( char *, long, long ));

	int  	ret = 0;    			/* Assume things worked */

	/* Execute() returns 0 if error */
	if( Execute( cmdline, (long)0, Output() ) == 0 )
		ret = -1;

	return( ret );
}   /* amiga - system */

#endif


/* -------------------  Examining the Environment  -------------------- */


/*   inspectEnv  --  If an EV is set, return its value, otherwise return
//                   the default string.  CALLER OWNS STRING.
*/
    char *
inspectEnv( envVar, defaultStr )
	char	*envVar;    	/* Environment variable name */
	char	*defaultStr;	/* Accept if EV unset */
{
    char	*str;

	if( (str=getenv(envVar)) == NULL )
		str = defaultStr;

    return( newstring(str) );
}   /* inspectEnv */


/* ------------------------  Path Operations  --------------------------- */


/*   joinDirFname  --  Returns a dynamic string consisting of the pathname,
//                     a separator (applied if needed), and the filename.
//                     Concentrates slash conventions in one place!
//          RETURNS:   Caller owns string.  Can't use global buffer.
*/
    char *
joinDirFname( dirname, filname )
    CONST char	*dirname, *filname;
{
    char	*whole;     	/* complete filespec.. */
	char	c;
	char	*glue = ""; 	/* How dirname and filname are joined */
	int 	len;

    if( dirname == NULL )
		dirname = "";	/* Current Directory */

	len = strlen( dirname ) + strlen( filname ) + 2;

	/*  Try glueing together path and fname.   */
	/*  Contort thru for each system, appending a slash if needed:     */
	/*  The 'strlen(dirname)' protects lastchr().  */
	/*  Adjust 'len' to hold directory-filename separator. */

#ifdef MCH_AMIGA
	if( strlen(dirname) > 0 )
	{
		c = lastchr( dirname );
		len += 1;
		if( c != ':' && c != '/' )
			glue = "/" ;
	}
#endif
#ifdef MS_DOS
	if( strlen(dirname) > 0 )
	{
		c = lastchr( dirname );
		len += 1;
		if( c != ':' && c != '/' && c != '\\' )
			glue = "\\" ;
	}
#endif
#ifdef UNIX
	if( strlen(dirname) > 0 )
	{
		c = lastchr( dirname );
		len += 1;
		if( c != '/' )
			glue = "/" ;
	}
#endif

	if( (whole = malloc( len )) == NULL )
	{
		write( 2,"joinDirFname - CORE\n", 20 );
		abort();
	}
	sprintf( whole, "%s%s%s", dirname, glue, filname );

#if SW_WATCH
	printf( "(joinDirFname) <%s>\n", whole );
#endif

	return( whole );     	/* The Complete picture */
}   /* joinDirFname */




/* ------------------------  File Operations  --------------------------- */


/*   file_indir  --  Returns a complete filespec (name and dir parts) as
//				     a dynamic string.  Take 'path' which is either a
//				     path or a filespec.  First goal is to find a
//			directory.  If the string isn't a dir, then return it as
//			a filename.  Otherwise, append a dir separator, concat the
//			filename, and return that.
*/
	char	*
file_indir( path, fname )
	char	*path;			/* Path, either directory name or filename */
	char	*fname;			/* Use this filename is path==directory */
{
	char	*filespec = NULL;

    /*  1.  If path is empty, then we're in the current directory: */
	if( path == NULL || strlen( path ) == 0 )
	{
#if SW_WATCH
		printf( "(file_indir) fname <%s>\n", fname );
#endif
		return( newstring( fname ) );
	}

	/*  2.  If path is a complete filespec, return that: */
	if( !isudir( path ) )
	{
#if SW_WATCH
		printf( "(file_indir) complete <%s>\n", path );
#endif
		return( newstring( path ) );
	}

	/*  3.  We know there is path.  Try path+"/"+fname.  */
	filespec= joinDirFname( path, fname );

#if SW_WATCH
	printf( "(file_indir) both <%s>\n", filespec );
#endif

	return( filespec );     	/* The Complete picture */
}   /* file_indir */



/*   must_open  --  Using the user provided base filename, try to open it.
 * 			        If not found, append a type extension and try again.
 * 			        The opened filename is stored in buff[].  Always
 * 			returns with open file pointer.  The extension must include
 *          the period, ie ".c" string.
*/
	FILE *
must_open( fname, mode, type )
	char	*fname;				/* User provided filename */
	char	*mode;				/* fopen() open mode */
	char	*type;				/* Desired extention */
{
	FILE	*fp;
	char	*s;

	strcpy( buff, fname );
	if( type != NULL && *type != EOS )
	{
		if( (s=strrchr( buff, '.' )) == NULL )
            strcat( buff, type );
		else
		{
		    strcpy( s, type );			/*  Force change extension.  */
		}
	}   /* if extension important */

	if( (fp=fopen( buff, mode )) == NULL )
	{
		if( mode[0] == 'r' )
			s = "read";
		else
			s = "update";
		fprintf( stderr, "Can't %s <%s>\n", s, buff );
		exit( 10 );			/* Fail big time! */
	}

	return( fp );
}   /* must_open */



/*   my_maketemp  --  Create a temp filename in some temp location.
 *                    The filename is returned always, and has a trailing
 *                    directory separator attached to it.  If there is
 *                    no environment variable to select a temp dir, then
 *         use nothing, which should put the file in the current directory.
 *  NOTE:  The EV_TEMPDIR is a string list.  We try each one in order.
 */
	char *
my_maketemp()
{
	extern char 	*mktemp PARMS(( char * ));

	static char   *tempvars[] = { EV_TEMPDIR, NULL } ;
	char	*tpath, *tdir = NULL;			/* Temp Dir */
	int     i;

    for( i=0 ; tempvars[i] != NULL ; ++i )
	{
		if( (tdir=getenv(tempvars[i])) != NULL )
			break ; 		/* Found a specifier */
    }

	if( tdir == NULL )    	/* Still didn't get any.. :-( */
	{
#ifdef P_tmpdir
		tdir = P_tmpdir;			/* Grab from <stdio.h> */
#else
#ifdef UNIX
        tdir = "/tmp/" ;            /* Common UNIX temp place */
#else
#ifdef MCH_AMIGA
		tdir = "T:" ;
#else
#ifdef MSDOS
		tdir = ".\\" ;   	/* Current directory */
#else
		tdir = "" ;         /* generic current directory */
#endif
#endif
#endif
#endif
	}

	tpath = joinDirFname( tdir, "ot-XXXXX" );      /* Splice */
	tdir = newstring( mktemp( tpath ) );
	if( strlen(tdir) == 0 )
    {
        /* When mktemp() fails, it returns a zero length string. */
	    printf( "?%s: can't create temp file in %s.\n", ProgName, tpath );
		exit(20);
    }
	MFREE( tpath );

	return( tdir );
}   /* my_maketemp */


/* ------------------------  External Workers  -------------------------- */


/*    cpp_infile  --  Run the "C" Pre-processor on an input file.  The
 *                    environment variable INCLUDE is examined.  The
 *                    directories are used as options to "cpp".  Any
 *          options should be passed in 'proc_args'.  If there are any
 *          troubles, we abort through exit().
 */
	void
cpp_infile( prog_name, prog_args, infname, outfname )
	char	*prog_name; 			/* File to exec() */
	char	*prog_args; 			/* Args to pass file (can be NULL) */
	char	*infname, *outfname;
{
	extern int    system();

	char	*cmdline;				/* Cmd Line to exec */

	cmdline = newstrjoin( prog_name, prog_args );
    cmdline = newstr1chr( cmdline, ' ' );

	/*  Now assign names.  'outfname' always last on line: */
	cmdline = newstrcat( cmdline, infname );
	cmdline = newstrcat( cmdline, outfname );

	if( yydebug )
	{
		puts( cmdline );         /*  Print out command line */
	}

	/*  Run the pre-processor on the thing: */
	if( system( cmdline ) < 0 )
	{
		fprintf( stderr, "Can't system() `%s'\n", prog_name );
		exit( 20 );
	}

	MFREE( cmdline );
}   /* cpp_infile */



/* ---------------------  Paths and FileSpecs  ------------------------- */


/*   mk_base_name  --  Extract the base filename.  Strip any path spec,
//			   and remove the file extension.  Assumes there is
//			   a filename to extract.  Otherwise returns a nil
//			   string if nothing to extract.
//		Returns:  caller owns string.
*/
	char *
mk_base_name( path )
	CONST char	*path;
{
	register char	*base, *p;

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

	p = newstring( base );    /* <-- Create a dynamic string; caller owns */
	if( (base=strrchr(p, '.')) != NULL )
		*base = EOS;

	return( p );
}   /* mk_base_name */



/* --------------------  Hash-define'd Cover Ups  ----------------------- */


#ifdef MCH_AMIGA

/****  NO MORE CALLS TO  getenv()  BEYOUND THIS POINT  ****/
#undef  getenv

#include <workbench/startup.h>

/*   mygetenv  --  Check shared library and filesystem based environment
 *                 variables for a requested value.  Checks for info
 *                 tooltypes before any global environments.  Returns ptr
 *          to staic string storage (save yourself before calling a
 *          second time).  Returns NULL if nothing found.
 */
    char *
mygetenv( envstr )
    CONST char   *envstr;
{
	extern struct WBStartup  *WBenchMsg;	/* Iconic launch.. */
	static char   envbuff[ 200 ];
	int 	len, fd;
	char	*value = NULL;

	if( (value=getenv( envstr )) == NULL )
	{
		sprintf( envbuff, "ENV:%s", envstr );
		if( (fd=open( envbuff, O_RDONLY )) >= 0 )
		{
			envbuff[ 0 ] = 0;
			value = & envbuff[0];
			if( (len=read( fd, envbuff, 199 )) >= 0 )
			{
			    envbuff[ len ] = 0;
			}	/* value retrieved OK from ENV: */

			close( fd );
		}	/* env value exists */

	}	/* better search filesystem.. */

	return( value );
}   /* amiga - getenv */

#endif

