/*   OCT.H     O.O. "C" Translator Preprocessor Header        May 89, bjw */

/*****************************************************************
 *   This program is (c) COPYRIGHT 1989,1990,1991,1992 by Brian J. Witt.
 *                  rael.mystic.com!aleks
 *
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responsibility of usefulness or damage incurred arising
 *   from any use of this intellectual property.  Any distribution
 *   must include this header and copyright notice.
 *   There are no warrenties, either express or implied as to fitnes
 *   of this product to perform any functions,
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.  A
 *   small fee ( < $25 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *    This header file is for the Object C Translator (OCT).  It
 *    defines lots of externs, along with structures and constants.
 * 
 *    This file adapts itself to these environments.  Please ensure
 *    that an appropriate #define symbol has been defined.
 *  	    MSDOS      --  Microsoft DOS environment
 *  	    UNIX       --  Xenix, Unix, bsd, Posix, ..
 *  	    MCH_AMIGA  --  Commodore-Amiga, vers 1.2 +
 *          AZTEC_C    --  Manx Aztec C, Amiga version 3.6A.
 * 
 * 
 *    10-apri-89   bjw    Initial coding.
 *    27-july-89   bjw    Use only ANSI memory functions.
 *    03-aug-89    bjw    Separate context and state variables.
 *    13-aug-89    bjw    Removed update flag, added no output flag.
 *    24-aug-89    bjw    Updated to Objective-C version 4.
 *    26-oct-89    bjw    yytext[] no longer public.
 *    8-oct-90     bjw    Removed rootc, getenv() is special on Amiga.
 *                        We include <stdlib.h> for best results.
 *    14-dec-90    bjw    Acknowledge @public instance vars.
 *    12-april-91  bjw    changed /strdup/ --> /newstring/.
 *    28-May-91    bjw    <ctype.h> included here in case toupper('5')!='5'.
 *    20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
 */



/*  "GRAMMAR" is defined for the lexer and the grammar files only.
*/
/* Standard headers first so every TU gets prototypes (SAS/C Warning 100). */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifndef  GRAMMAR

/*  Our standard header usually in a /usr/local/include place.. */
#include <octhead.h>

#endif

#include <objc/private/config.h>

#ifndef DEBUG
#define DEBUG 1
#endif

/* Debug trace: define DEBUG=1 when building (e.g. DEFINE DEBUG=1). All output
 * goes to stdout so it is visible when stdout is redirected or buffered.
 */
#if defined(DEBUG) && DEBUG
# include <stdarg.h>
/* Full prototypes for strict compilers (e.g. SAS/C) that warn on K&R declarations. */
int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int fflush(FILE *stream);
static void _oct_dbg(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stdout, "[OCT] ");
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	fflush(stdout);
}
# define DBG(x) do { _oct_dbg x; } while(0)
#else
# define DBG(x) ((void)0)
#endif

/*  Some nice string handling:  */
#ifndef EOS
# define  EOS  '\0'
#endif

#define  STREQ(s,t)      ((s)[0] == (t)[0] && !strcmp(s,t))
#define  SKIP_SPACE(s)   { while(isspace(*(s))) ++(s); }


/* vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

/* ------------------------  Constant Control  ------------------------- */

/*  Enable Memory tracking...  Really only useful on systems that
 *  return a value from free() whether or not they were handed a
 *  bogus value...  Aztec C's free() returns a boolean value.
 *  Everyone else should set this define false.
 */
#define  SW_MEMTRACKER   0

/*  A way of declaring your intentions: */
#define  FORWARD  

/*  Mark those routines "local" to the source file (static): */
/*  Leaving this blank helps in debugging translator, though. */
#define  LOCAL
/* define  LOCAL  static  */


/*  Place holder to separate items in a message: */
#define  CH_PARM_SEP  ((unsigned char)0266)


/* ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */

#define  CH_OPEN_PAREN  '('
#define  CH_CLOSE_PAREN ')'

#define  CH_LCURLY	'{'
#define  CH_RCURLY	'}'


#ifdef AZTEC_C
/*  Aztec C specific... */
# undef SW_MEMTRACKER
# define  SW_MEMTRACKER   1
#endif


#if SW_MEMTRACKER
#  ifdef AZTEC_C
#    define MFREE(p)   myfree((char *)(p),__FILE__)
#  else
#    define MFREE(p)   free((char *)(p))
#  endif
#  define MALLOC(len)  myalloc(len,__FILE__)
  extern void    *myalloc();
  extern void    myfree();

#else
#define  MFREE(p)   free((char *)(p))
#define  MALLOC(len)  malloc(len)
#endif


#ifdef MAIN
#define  OC_EXTERN
#else
#define  OC_EXTERN  extern
#endif


#ifndef NO_PROTOTYPE
# define  PARMS(x)    x
#else
# define  PARMS(x)   ()
#endif

#if defined(__STDC__) && __STDC__
# define  CONST  const
#else
# define  CONST
#endif


/* -----------------------  Types and Structures  ---------------------- */


/*  Save the previous context values away from static vars: */
typedef struct _CONTEXT
{
    struct _CONTEXT  *parent_context;    /* or NULL if root */
    unsigned short   prev_context;       /* prev Context state */
    short	    ctx_depth;           /* 0 for base level */
    char	   *prev_target;         /* prev target object */
    char	   *prev_selector;       /* prev selector name */
    char	   *prev_parms;          /* prev parameters for method */
    char	   *prev_lext;           /* prev lex_text() string */
}  CONTEXT ;


#ifndef  GRAMMAR
/*  For construction of special words in class declaration: */
struct TYPECONNS
{
    char	*c_name;
    WORD	id_chr;
} ;


struct mynode
{
    struct Node	   node;
    char           flags;       /* DF_xxx */
    char           order;       /* 'F' or 'I' */
    int            sequence;	/* Index into selector array */
    long           lineDefined; /* Line defined in source file */

    char           *def;        /* Complete opening definition */
    char           *tag;        /* misc info  */
    char           *names;      /* Yet another string... */
    char           *publicNames;  /* Names for all to see */
} ;

#define  DF_CALLED   	0x01	    /* Method actually called */
#define  DF_IMP     	0x02	    /* We implement method */
#define  DF_GENERIC 	0x04	    /* Referenced, but not declared */
#define  DF_DECLARED	0x08	    /* Declared in @interface */

#define  DF_PROTOCOL	0x10	    /* No class attached to this method */
#define  DF_TWO_ORDER	0x20	    /* Factory and Instance method */
#define  DF_PUBLIC      0x40        /* Method/inst var is pulic access */
#endif


/*  These define a state machine for parsing a message:  */
enum MSM
{
    MSM_DONE,     	    	/* All done with parameters */
    MSM_START,   	    	/* Found message opening */
    MSM_OBJ_MSG,	    	/* Target object is a function */
    MSM_SUPER,	    	    /* Target is superclass */
    MSM_METHOD,	    	    /* Got Method Name */
    MSM_ID,       	    	/* Either method name or variable */
    MSM_VADCL,	    	    /* Encountered "," of variable dcl's */
    MSM__last
} ;


enum key_lex_text
{
    LT_START,     	    	/* Start capturing */
    LT_STOP,     	    	/* End capture, trim 'eatum' */
    LT_APPEND1,  	    	/* Append one char */
    LT_TRIM,    	    	/* Trim 'eatum' chars off end */
    LT_ADD,     	    	/* Add provided string */
    LT_QCAPTURE,	    	/* Return NULL if not capturing */
    LT__last
} ;


/* --------------------  External Statements  ------------------------- */


#ifdef AZTEC_C
/*  Install macros to convert your compiler's memory functions to
 *  ANSI conforming calls.  Non-overlapping copies. These are for MANX:
 */
#define  memcpy(dest,src,cnt)	movmem(src,dest,cnt)
#define  memset(dest,ch,cnt)	setmem(dest,cnt,ch)

#endif

/* strstr/strchr from <string.h> (included at top); do not redeclare. */


/* ---------------------------------------------------------------- */


/*  Our dynamic string library:  */
extern unsigned char   lastchr PARMS(( CONST char * ));
extern char	    *strremove  PARMS(( char *, char ));

extern char 	*newstring  PARMS(( CONST char *fix ));
extern char	    *newstrcat  PARMS(( char *old, CONST char *add  ));
extern char 	*newstrins  PARMS(( CONST char *fixed, char *dynamic  ));
extern char 	*newstrjoin PARMS(( CONST char *front, CONST char *rear  ));
extern char 	*newstr1chr PARMS(( char *front, char lastch ));

extern void	    addMethod PARMS((  char order, char *ret_type, char *name ));
extern void	    dump_m_header PARMS((  struct mynode * 	));
extern void	    m_call PARMS(( 	enum MSM 	));
extern struct mynode  *parse_structure PARMS(( int souoe ));

/*  Scanner communiction routines:  */
extern void     hash_line PARMS(( int      	));
extern char    *lex_text PARMS(( enum key_lex_text , ... ));

extern struct mynode	*methSearch PARMS(( char *colonName, int order ));
extern void	     addTypeName PARMS(( char * yyext 	));
extern char	    *asCRef_selector PARMS(( char *colon_name ));
extern char 	*asFunc_selector PARMS(( CONST char *colon_name ));
extern char  	*str_name_method PARMS(( CONST struct mynode * ));
extern void  	 enter_type PARMS(( CONST char *, CONST char *));

extern int       lu_instvar PARMS(( CONST char *target ));
extern char     *lu_classdef PARMS(( char *class_name ));
extern char     *publicClassVarSTR(char *class_name);


#ifndef GRAMMAR
extern FILE	    *must_open PARMS(( char *base, char	*mode, char *ext  ));
extern char	    *mk_base_name PARMS(( CONST char *fname ));

extern struct mynode   *mk_mynode PARMS(( CONST char *, CONST char * ));
extern void           done_mynode PARMS(( struct mynode * ));

extern void init_types(void);
extern void init_parser(void);
extern void pre_ops(FILE *fp, int flag);
extern int yyparse(void);
extern void dump_dict(void);
extern void post_ops(void);
extern void cleanup(int rc);
extern void cpp_infile(char *prog_name, char *prog_args, char *infname, char *outfname);
extern char *strlower(char *s);

/*  These are generally known already to the grammar: */
extern int      yydebug;
extern FILE  	*yyin, *yyout;

extern unsigned    lineat;    	/* Which line in source file */


/* --------------  Variables and Such from Cmd Line  ------------------ */


OC_EXTERN char	    verbose_flag;
OC_EXTERN char	    list_flag;	    	/* TRUE produce .lst file */
OC_EXTERN char	    no_output_flag; 	/* TRUE to suppress .c output */
OC_EXTERN char	    keep_flag;	    	/* TRUE to preserve cpp output */

OC_EXTERN int 	    warn_level;	    	/* User set warning level */
#define  WARN_SILENT	-1
#define  WARN_NORMAL	0
#define  WARN_LOUD  	1

OC_EXTERN char	    *from_fname;	    /* True name of source file */
OC_EXTERN char	    *dest_fname;	    /* Output ".c" file */


#endif	 /* if grammar */

OC_EXTERN char     *curr_name;  	    	/* Current token name ptr */
OC_EXTERN char     *ProgName ;              /* From argv[0] */


/*  Various lookup trees (internal dataset): */
OC_EXTERN void	    *class_tree;	    /*  class <--> superclass */
OC_EXTERN void	    *method_tree;	    /*  methods <--> var types */
OC_EXTERN void	    *ivar_tree;  	    /*  class <--> instance structure */

OC_EXTERN void   	*symtab;	    /* Variables and typedefs */

OC_EXTERN char  	*className;
OC_EXTERN char  	*superclassName;
OC_EXTERN char  	*classVars;
OC_EXTERN char	    *all_classVars;	    /* Complete instance var list */


OC_EXTERN char   	m_order;	    	/* either 'F' or 'I' */
OC_EXTERN struct mynode  *curr_method;  	/* Method being declared */


OC_EXTERN short  td_nest;	    /* Typedef []{} nest counting */
OC_EXTERN short  save_text;	    /* TRUE if lex_text() save text */
OC_EXTERN char   *texts;	    /* Text string built up so far */
OC_EXTERN char	 *src_filename;	/* As deduced from #line directives */

OC_EXTERN int    cnt_errors;	/* How many errors we encountered */
OC_EXTERN int    cnt_warnings;	/* How many warnings we encountered */

OC_EXTERN char	*error_string;	/* Many be printed with error msg */
OC_EXTERN char	*category_name;	/* Our category name */


/*  Context information maintained by parsing logic and m_call(): */
OC_EXTERN unsigned     in_context;
#define  IC_VADCL      	    0x001	    /* Comma found in msg expr  */
#define  IC_OUTPUT_CALL	    0x002
#define  IC_MESSAGE	    	0x004
#define  IC_RECURSE	    	0x008	    /* Method needs method eval */
#define  IC_SUPERCLASS	    0x010
#define  IC_TYPEDEF_DEF	    0x020
#define  IC_INTERFACE	    0x040	    /* Inside @interface..@end block */


/*  State of translation (global to file): */
OC_EXTERN unsigned 	   in_state;
#define  IS_METHOD_DEF	    0x001
#define  IS_CLASS_DEF	    0x002
#define  IS_TYPECAST	    0x004	    /* Last paren was typecast  */
#define  IS_NEED_HLINE	    0x008	    /* Need #line to re-synch (char) */
#define  IS_INCLUDES	    0x010	    /* We output include statements */
#define  IS_SEEN_IF 	    0x020	    /* TRUE if inside @interface */
#define  IS_SEEN_IMP	    0x040	    /* TRUE if inside @implementation */
#define  IS_GOT_IMP    	    0x080	    /* TRUE if see @implementation */

#define  IS_PROTOCOL	    0x100	    /* Inside @protocol section */
#define  IS_CATEGORY	    0x200	    /* Encountered category clause */
#define  IS_SEEN_PUBLIC 	0x400   	/* See @public directive */

#define  IS_ERROR          0x2000	    /* Used by gerr() */
#define  IS_ABORT          0x4000


/* --------------------  Mildly Public Variables  ---------------------- */

OC_EXTERN  char 	buff[ 240 ];


#undef  OC_EXTERN

/* end of OCT.H */

