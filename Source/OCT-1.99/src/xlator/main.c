/*   MAIN.C    YACC and LEX "C" syntatic sugar main driver    May 89, bjw */

/*****************************************************************
 *   This program is copyright (c) 1989,1990,1991,1992 by Brian J. Witt.
 *                 netcom.com!gentoo.com!rael!aleks
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.  Any distribution
 *   must include this copyright notice.  This is freeware.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.
 *   A small fee ( < $25 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *     This file contains the top level functions for starting up and
 *     finally finishing a translation from OOC to ordinary "C" code.
 *     Usage is outlined below in usage().
 *
 *     To facilitate ancillary (that's a DEC VAX/VMS word) processing
 *     of classes, translated output can be supresses with the -n
 *     option.  It basically aims 'yyout' at a null device.
 *
 *     Originally this programmed relied on both bison and flex (yacc
 *     and lex work-alikes).  The grammar is now hand coded (for the
 *     oft mentioned speed improvement), so flex is no longer needed.
 *     Flex scanner is still in place (replacement is on to-do list).
 *
 *     Version option sends the version string to stderr.  Processing
 *     continues from there...
 *
 *     27-july-89	bjw 	Initial commenting...
 *     03-aug-89	bjw 	Separate state and context vars [1.24].
 *     13-aug-89	bjw 	Removed update flag, added no output flag.
 *     27-aug-89	bjw 	Adapting to Objective-C version 4 [1.40].
 *     2-sept-89	bjw 	Trying better error recovery... [1.43].
 *     6-sept-89	bjw 	Retooling [1.50].
 *     18-sept-89	bjw 	More fields in "struct _SHARED".
 *     24-sept-89	bjw 	"T-race" flag [1.60].  Happy B-day Shari!
 *     30-sept-89	bjw 	Removed yyerror(), instead call gerr().
 *     7-oct-89  	bjw 	strdup() instead of STRSAVE() [1.68].
 *     19-oct-89	bjw 	options -za and -zp formed.
 *     21-oct-89	bjw 	Release AO.1, [1.71]
 *     26-oct-89	bjw 	Current token name now: curr_name.
 *     07-nov-89	bjw 	Exec file name "OCT" [1.82]
 *     14-feb-90	bjw 	type "STR" is "*" [1.88, BG1].
 *     03-mar-89	bjw 	Added delegate support, -E option.
 *     2-april-90	bjw 	Better ivar parsing [1.91].
 *     9-april-90	bjw 	Copy file after @implementation's @end.
 *     29-apr-90	bjw 	BADGE release #1 [1.91.7].
 *     31-may-90	bjw 	-I doesn't include I. use -zd for yydebug=1.
 *     29-july-90	bjw 	Removed INCLUDE environment variable.
 *     15-sept-90	bjw 	Aligned structures towards NeXT. [1.92].
 *     15-Nov-90	bjw 	trace now -zt [1.92.5].
 *     14-dec-90 	bjw 	Filesystem dependencies now in files.h.
 *     2-march-91	bjw 	Real UNIX release [1.93.5B].
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    20-april-92   bjw     @abstract --> @protocol (for NeXT v2).[1.93.14]
 *     1-june-92    bjw     EV_TEMPDIR now string list.
 *    19-sept-93    bjw     Last 1.99 release (#import cleanups), -Wall add.
 */

static char identstring[]={"@(#) OCT system 1.90 series/Amiga" };
static char internal_copyright[]=
   {" copyright (c) 1989,1990,1991,1992,1993 by Brian Witt. ALL RIGHTS RESERVED." };

static char copyright[]={"copyright (c) 1989-1993 by Brian Witt. ALL RIGHTS RESERVED." };
static char  copyright_2[]={" Inquires to: brian@babbage.ecs.csus.edu " };

#define  MAIN  1
#define  SW_WATCH  0
#define  SW_ASSERT  1


#include "oct.h"
#include "error.h"

#include <tree.h>
#include <objc/release.h>

#ifdef MCH_AMIGA
/* write/unlink prototypes when unistd.h not used */
extern int write(int fd, const void *buf, unsigned n);
extern int unlink(const char *path);
#endif
#ifndef MCH_AMIGA
#include <unistd.h>
#endif


#ifndef __DATE__
# define __DATE__	"19 Sep 93"
#endif


/* -----------------------  Variables and Such  ----------------------- */


CONST char    version [ ] = { "OCT Translator 1.99.0" } ;

char       	* base_name ;			/* base name of .m file */


/* ---------------------  Private Externals  ------------------------ */

extern char 	*my_maketemp PARMS(( void ));

char      		*temp_name ;			/* CPP output file */

static char     *out_fname;         	/* Final Result output filename */
static char     *preproc_opts;		    /* User requested cpp options */

static char     pipe_flag = FALSE;
static char     run_cpp = TRUE;
static char  	only_cpp = FALSE;		/* TRUE for -E flag */

static FILE     *phasefp;               /* "Phase n" messages.. */

/* Forward declarations (ANSI C). */
static void usage(int rc);
static void pcl_needfile(int optchr);
static int parse_opts(int argc, char *argv[]);
static int parse_cmdline(int argc, char *argv[]);
static void aim_input(char *filename);


/* ----------------------  Private Support Code  ----------------------- */


/*   usage  --  Give user some hints about our usage.  If rc < 0, then
 *              just a simple line of help is printed.  Otherwise a
 *              little instructional information is displayed.
 */
static void
usage(int rc)
{
   FILE   *f = stdout;
   static char   *trypaths[] = { EV_TEMPDIR, NULL } ;
   int      i;

   if( rc >= 0 )
   	fputs( RELEASE, f );

   fprintf( f,
 "?usage: OCT [-Ehln][-i dir][-o file][-p opts][-w#][-z{adkpt}] Mfile\n");
   /* Also to stderr so it appears when stdout is not connected (e.g. Amiga Workbench). */
   fprintf( stderr,
 "?usage: OCT [-Ehln][-i dir][-o file][-p opts][-w#][-z{adkpt}] Mfile\n");

   if( rc < 0 )
   	exit( -rc );

   fputs("Accepts a '.m' file producing a '.c' file.\n", f );
   fputs("  -E       --  Only run 'cpp', file to stdout.\n", f );
   fputs("  -h       --  Print out usage (help, this message).\n", f );
   fputs("  -i dir   --  An include directory for 'cpp' to search.\n",
   			f );
   fputs("  -l       --  class Listing information file.\n", f );
   fputs("  -n       --  No translated output (-l OK).\n", f );
   fputs("  -o outfile -- Specify output file dir/name.\n", f );
   fputs("  -p cppflags -- Adds 'cppflags' string to 'cpp' call.\n", f );
   fputs("  -v       --  Display version information only.\n", f );
fputs("  -w#      --  Set warning level: 0/1/2 --> silent/normal/noisy.\n",
   			f );
   fputs("  -Wall    --  Display all warnings (like -W3).\n", f );
   fputs("  -za      --  Don't invoke 'cpp' on source file (accept).\n", f );
   fputs("  -zd      --  Output obnoxious internal debugging.\n", f );
/* Avoid my_maketemp() here; it can block on Amiga (getenv/mktemp). Use env name. */
   fprintf(f,"  -zk      --  Keep (don't delete) output of 'cpp' in " EV_TEMPDIR " (or current dir).\n");
   fputs("  -zp      --  Send translated source to stdout (piping).\n",f);
   fputs("  -zt      --  Trace grammar's progress.\n", f );

   fputs("These environment variables are used:\n", f );
   for( i=0 ; trypaths[i] ; ++i )
	   fprintf(f, " %9s --  Store 'cpp' output here.\n", trypaths[i] );
   fprintf(f, " %9s --  Pass args to 'cpp'.\n", EV_CPP );
   fprintf(f, " %9s --  Option string for translator.\n", EV_OPTS );
   fprintf(f, " %9s --  Addtional include directories.\n", EV_INCLUDE );

   fflush( stdout );
   fflush( stderr );
   exit( rc );
   /*NOTREACHED*/
}	/* usage */


static void
pcl_needfile(int optchr)
{

	printf( "?%s: -%c requires filename.\ntype \"%s -h\" for help\n",
			ProgName, optchr, ProgName );
	usage( -1 );
	/*NOTREACHED*/
}	/* pcl_needfile */



/*   addInclude  --  Another directory to have 'cpp' search inside.
 *                   String them all together.
*/
void
addInclude(char *include_fname)
{
	char	*junk;

	junk = newstrjoin( " -I", include_fname );
	preproc_opts = newstrcat( preproc_opts, junk );
	MFREE( junk );
}	/* addInclude */



/*   parse_opts  --  Parse the options separately.  Can accept input from
 *                   command line or environment variables.  Aborts through
 *                   pcl_needfile() and usage() if there are problems.
 *                   Returns position in argv[] where options stopped.
 *                   Var 'letter' hold un-case converted for UNIX freaks.
 *   Side Affects:  Sets many flag variables.
 */
static int
parse_opts(int argc, char *argv[])
{
#define  THE_PARM(cp)   (((cp)[1] == EOS) ? ap=argv[++index] : ++(cp) )

    extern int  yydebug;
    char	*cp;
	char   *ap;
	char    letter;
	char    *thisArg;
    int 	index = 1;

    /* ---------------------------------- */
    /*   Parse only the user's options    */
    for( ; index < argc && (ap=argv[index])[0] == '-' ; ++index )
    {
#if SW_WATCH
        printf( " >>%s<<\n", ap );
#endif
      	for( cp = & ap[1] ; *cp ; ++cp )
        {
            letter = *cp;               /* true case of option letter */
            switch( tolower(letter) )
            {
                case 'h' :            /* Display full help text */
                    usage( 1 );
                    /*NOTREACHED*/

                case 'v' :            /* Display version information */
                    fputs( RELEASE, stderr );
                           /* version[] and DATE printed on startup. */
                           /* 'RELEASE' ends with newline */
                    break ;

                /***  FLAGS  ***/
                case 'e' :
                    only_cpp = TRUE;
                    break ;

                case 'l' :
                    list_flag = TRUE;
                    break ;

                case 'n' :
                    no_output_flag = TRUE;
                    break ;

                    /*  Warning Level  */
                case 'w' :
                    if( isdigit(cp[1]) )
                    {
                    	/* Advance arg pointer only if digit: */
                    	/*  Only values {0,1,2} are defined (now) */
                    	warn_level = *++cp - '0' + WARN_SILENT;
                    } else
					if( strncmp( &cp[1], "all", 3 ) == 0 )
					{
						cp += 3;		/* Advance past "all" */
						warn_level = WARN_LOUD;
					} else
                    {
                    	warn_level = WARN_SILENT;
                    }

                    break ;

               /***  SETTINGS  ***/
                case 'z' :
                    letter = *++cp;
                    switch( tolower(letter) )
                    {
                    	case 't' :
                    		verbose_flag = TRUE;
                    		break ;
                    	case 'p' :
                    		pipe_flag = TRUE;
							phasefp = stderr;	/* No phase 2 in outfile */
                    		break ;
                    	case 'k' :
                    		keep_flag = TRUE;
                    		break ;
                    	case 'd' :
                    		yydebug = TRUE;
                    		break ;
                    	case 'a' :
                    		run_cpp = FALSE;
                    		break ;

                    	default :
                    		goto  OPT_ERR;
                    }
                    break ;

                /***  FILE SETTINGS  ***/
                case 'o' :
                    if( (thisArg=THE_PARM(cp)) == NULL )
                    {
                    	pcl_needfile( letter );
                    	/*NOTREACHED*/
                    }
                    out_fname = thisArg;
                    continue ;

                    /*  Include directories  */
                case 'i' :
                    if( (thisArg=THE_PARM(cp)) == NULL )
                    {
                    	pcl_needfile( letter );
                    	/*NOTREACHED*/
                    } 
                    addInclude( thisArg );
                    continue ;

                case 'p' :
                    if( (thisArg=THE_PARM(cp)) == NULL )
                    {
                    	/*  Ran out of cmdline args! */
                    	fputs( "Bad form for -p option\n", stderr );
                    	usage( 5 );
                    	/*NOTREACHED*/
                    }

                    preproc_opts = newstrcat( preproc_opts, thisArg );
                    continue ;

                case '?' :
                    usage( 1 );
                    /*NOTREACHED*/

                default :
        OPT_ERR :
                    printf( "?%s: unknown option <%c> in <%s>\n",
                                  ProgName, letter, ap );
                    usage( -1 );
                    /*NOTREACHED*/

            }	/* switch on option char */

        }	/* for each char in this argv[] element */

    }	/* while options to process */

    return( index );
}   /* parse_opts */



/* -------------------------  Mainline Code  --------------------------- */


/*   parse_cmdline  --  Feed parameter settings into a parser to set
 *                      ourselves up the way the user wants us.  First
 *                      the environment var is checked for settings.  Then
 *                      the command line is examined.
 *      Returns:  offset of possible file in argv[].
 */
static int
parse_cmdline(int argc, char *argv[])
{
    extern char   **tokenizer  PARMS(( CONST char  *plainStr ));
    extern int    yydebug;

    char	**user_argv;
    char 	*cp;
    int     ap;

    DBG(("parse_cmdline: argc=%d\n", argc));

    /*  Help can be obtained with no args, or first arg containing a '?'.
    //  Ie, "oct -?" will print usage.
    */

    if( strchr( argv[1], '?' ) )
    {
	    usage( 1 );
	    /*NOTREACHED*/
    }

    /* ------------------------------- */
    /*  Setup some flags and settings: */
    warn_level   = WARN_NORMAL;
    run_cpp      = TRUE;
    yydebug      = FALSE;
    out_fname    = "";      		/* Last parameter for outfile */
#if SW_WATCH
    yydebug      = TRUE;
#endif
    sprintf( buff, " %s", CPP_ARGS );   /* Must have leading space */
    preproc_opts = newstring( buff );

    /* -------------------------------- */
    /*  Read include directories, then  */
    /*  general args for 'cpp':         */
    cp = getenv( EV_INCLUDE );
    user_argv = tokenizer( cp != NULL ? cp : "" );
    if( user_argv[1] != NULL )
    {
	    cp = user_argv[0];     	/* Whole text (dynamic) string */
	    for( ap=1 ; user_argv[ap] != NULL ; ++ap )
		    addInclude( user_argv[ap] );

	    MFREE( cp );
	    MFREE( user_argv );
    }


    /* --------------------------- */
    /*  Find args for 'cpp' only:  */
    cp = getenv( EV_CPP );
    if( cp != NULL )
    {
        /*  Feed these args to 'cpp' directly, no parsing! */
        preproc_opts = newstrcat( preproc_opts, cp );
    }

    /* -------------------------------------- */
    /*  Parse options stored in environment:  */
    /*  then parse options from command line. */
    cp = getenv( EV_OPTS );
    user_argv = tokenizer( cp != NULL ? cp : "" );
    if( user_argv[1] != NULL )
    {
	    cp = user_argv[0];     	/* Whole text (dynamic) string */
	    user_argv[0] = argv[0]; 	/* Program name for error reporting */
	    for( ap=1 ; user_argv[ap] != NULL ; ++ap )
		    (void)0;  /* count args for parse_opts */
	    parse_opts( ap, user_argv );

	    MFREE( cp );
	    MFREE( user_argv );
    }

    /*  Now examine command line stuff:  */
    ap = parse_opts( argc, argv );

#if SW_WATCH
	printf ("total args for CPP <%s>\n", preproc_opts );
#endif


    /* --------------------------------- */
    if( phasefp != NULL && (warn_level > WARN_NORMAL || verbose_flag) )
	    fputs( "Phase 1\n", phasefp );

    /*  Arrange for conforming flag combinations: */
    if( no_output_flag )
    {
	    pipe_flag = FALSE;
    }
    if( verbose_flag )
    {
	    /*  If user wants to keep temp file, then use -k option */
	    if( warn_level < WARN_NORMAL )
		    warn_level = WARN_NORMAL;
    }

    return( ap );
}	/* parse_cmdline */


/*   aim_input  --  Secure an input file.  Give us the string (possibly
 *                  from the command line).  Eventually this program will
 *                  be extended to include 'cpp' functions.  Until then,
 *                  the preprocessor will be run from here.
 *	Side Effects:  sets global 'from_fname', 'yyin'.
 */
static void
aim_input(char *filename)
{
    extern char   *file_indir PARMS (( char *path, CONST char *name ));

    char 	*cp;

    DBG(("aim_input: filename=%s\n", filename ? filename : "(null)"));
    cp = filename;
    if( (yyin=fopen(cp, "r" )) != NULL )
    {
        from_fname = newstring( cp );  		/* cmdline name valid */
    } else
    {
        yyin = must_open( cp, "r", ".m" );	/* We supply extension */
        from_fname = newstring( buff );			/* Save filename */
    }

    if( run_cpp || only_cpp )
    {
        /*  Just check for existance: */
        fclose( yyin );
        yyin = NULL;

        /* --------------------------- */
        /*  Pre-process the input file */
        if( ! only_cpp )
        {
        	DBG(("aim_input: calling my_maketemp\n"));
        	temp_name = my_maketemp();
        }
        DBG(("aim_input: calling cpp_infile\n"));
        cpp_infile( CPP_PNAME, preproc_opts, from_fname, temp_name );
        if( only_cpp )
        	return ;

        if( (yyin = fopen( temp_name, "r" )) == NULL )
        {
        	printf( "?%s: can't reach processed file!\n", ProgName );
        	exit( 10 );
        	/*NOTREACHED*/
        }
    }

    /* ------------------------------------------------------------ */
    /*  The file has now been preprocessed, perpare for input here. */
    errno = 0;
    if( no_output_flag )
    {
        yyout = fopen( NULL_FNAME, "w" );
        strcpy( buff, NULL_FNAME );
    } else
    if( pipe_flag )
    {
        yyout = stdout;
        strcpy( buff, "(pipe)" );
    } else
    {
        if( strlen(out_fname) > 0 )
        	out_fname = file_indir( out_fname, mk_base_name(cp) );
        else
        	out_fname = cp;

        yyout = must_open( out_fname, "w", ".c" );
    }

    dest_fname   = newstring( buff );
    src_filename = newstring( from_fname );

    /* Create a name that's basename only; no path, no extension: */
    base_name   = mk_base_name( from_fname );
    strlower( base_name );

}   /* aim_input */



/*   cleanup  --  Close files and tell user results of running program.
 *                Insure 'cnt_errors' is non-zero if setting 'rc' yourself.
 *                If no errors, then return code is changed to 0.
 *	Returns: never.
 */
void
cleanup(int rc)
{

    /*  Cleanup a little bit after ourselves: */
    fclose( yyin );
    fclose( yyout );
    if( run_cpp && ! keep_flag )
        unlink( temp_name );


    if( rc > 0 || cnt_errors > 0 )
    {
            /* ----------------------- */
            /*  Errors in User Source  */
            /* ----------------------- */

        if( (rc > 0 && !keep_flag) || no_output_flag )
            unlink( dest_fname );            /* Err'ed ".c" file created */
        else
            rc = 10;

        printf( "*** %d translator errors\n", cnt_errors );

    } else
    {
        rc = RC_OK;
        if( cnt_warnings > 0 )
        {
            /* -------------------- */
            /*  Only Warnings       */
            /* -------------------- */
            if( warn_level >= WARN_NORMAL )
            {
            	printf( "Notice: check your output: %s.\n", dest_fname );
            }
        } else
        {
            /* -------------------- */
            /*  Successful Run      */
            /* -------------------- */
            if( warn_level >= WARN_NORMAL )
            	printf( "%s: Translation successful for: %s.\n",
            				ProgName, from_fname );
        }
    }

    exit( rc );
    /*NOTREACHED*/
}	/* cleanup */



/* -------------------------------------------------------------------- */


int
main(int argc, char *argv[])
{
    extern int 	  yyparse();
    extern char   	* sel_array ;			/* Selector array */
    int   rc;

    /* Earliest possible output: confirm we reached main (no stdio, no globals). */
    (void) write( 2, "[OCT] main entered\n", 18 );

    /* No-args case first so we never touch init or stdio state; output and exit. */
    if( argc < 2 )
    {
	    fputs( "OCT: no input file. Use 'oct -h' for usage.\n", stderr );
	    fflush( stderr );
	    usage( 1 );
	    /*NOTREACHED*/
    }

    fprintf( stderr, "(%s, %s)\n%s\n", version, __DATE__, copyright );
    fflush( stderr );
    DBG(("main: start argc=%d argv[0]=%s\n", argc, argv[0] ? argv[0] : "(null)"));

    /* ----------------------- */
    /*  Init some reality (phasefp must be set before parse_cmdline uses it). */
    ProgName = argv[0] ;
    phasefp = stdout;
    preproc_opts = NULL;
    td_nest = 0;
    in_context = 0;
    className  = "" ;
    error_string = "" ;
    category_name = "" ;
    superclassName = "" ;
    DBG(("main: calling init_types\n"));
    init_types( );
    DBG(("main: calling init_parser\n"));
    init_parser( );

    /* ----------------------------- */
    DBG(("main: calling parse_cmdline\n"));
    rc = parse_cmdline( argc, argv );
    DBG(("main: parse_cmdline returned rc=%d argv[rc]=%s\n", rc, argv[rc] ? argv[rc] : "(null)"));
    aim_input( argv[ rc ] );
    DBG(("main: aim_input done\n"));
    if( only_cpp )
        cleanup( 0 );

    DBG(("main: calling pre_ops\n"));
    pre_ops( yyout, 0 );

    /* ----------------------------- */
    if( phasefp != NULL && (warn_level > WARN_NORMAL || verbose_flag) )
        fputs( "Phase 2\n", phasefp );

    /* There may be text after the @end directive.  Read until EOF: */
    DBG(("main: entering yyparse loop\n"));
    while( (rc = yyparse()) == RC_OK )
        ;;
    DBG(("main: yyparse loop done rc=%d\n", rc));

    fflush( stdout );
    fflush( stderr );
    if( cnt_errors > 0 )
        cleanup( RC_ERROR );

    /* ----------------------------- */
    if( phasefp != NULL && (warn_level > WARN_NORMAL || verbose_flag) )
        fputs( "Phase 3\n", phasefp );

    if( sel_array == NULL )
    {
        /*  For some reason 'cpp' didn't complete: */
        gerr( ERROR_ABORT, GERR_SUDDEN_EOF, NULL, 0L );
        /*NOTREACHED*/
    }
    if( className[ 0 ] == EOS )
    {
    	if( cnt_errors == 0 && warn_level > WARN_NORMAL )
            fprintf(stderr, "Note: no class defined in %s\n", from_fname);

    	if( !(in_state & IS_INCLUDES) )
            gwarn( GW_NO_INCLUDES );
    } 

    DBG(("main: calling dump_dict, post_ops, cleanup\n"));
    dump_dict( );
    post_ops( );

    /* ----------------------------- */
    cleanup( rc );
    /*NOTREACHED*/
}	/* main */


#ifdef MCH_AMIGA
/*   abort  --  For some reason, Manx 3.6A doesn't have abort().
 */
void
abort(void)
{
     write( 2, "Abort\n", 6 );
     exit(255);
}
#endif


