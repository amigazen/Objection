/*   STRS.C    String support for OCT processor...          June 89, bjw */
/*****************************************************************
 *   This program is copywrite (c) 1989, 1990, 1991 by Brian J. Witt.
 *                  aleks@rael.mystic.com
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $25 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
 *    This is the string support library.  These functions are pretty
 *    much independent of the application using them.
 * 
 *    Memory tracker can also be provided here.  Basically we watch
 *    that every free() returns without errors (like free'ing NULL,
 *    or memory not previously allocated).  Both malloc() and free()
 *    are trapped for completeness.  Helps when debugging code...
 * 
 *    The newstrcat() and newstrins() allow strings to be built with
 *    no real limits on length (other than memory).  They're designed
 *    specifically for gathering tokens, and try to decide for
 *    themselves when to add a space separator or not.
 * 
 *    The bulk of this file is really the typename lookup functions
 *    that are quite dependent on the rest of the code.  These
 *    functions try to encode the instance vars into a string where
 *    each char represents another variable in the instance.
 * 
 *    20-june-89	bjw 	Initial coding.
 *    15-july-89	bjw 	Removed file operations to their own file.
 *    09-aug-89    	bjw 	Removed symbol table handlers.
 *    24-sept-89	bjw 	Added newstrjoin().  Happy B-day Shari!
 *    25-nov-89    	bjw 	myalloc/myfree no longer take line numbers.
 *    6-april-91    bjw     Don't use NULL ptrs anymore (sparc happy).
 *    12-april-91   bjw     Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    30-may-91     bjw     Moved string functs to common file.
 */


#include "oct.h"


#define  SW_WATCH	0


/*  Define the environment variable seperator tester:         */
/*  AmigaDOS uses ":" and "!" for volumes; space OK in fname  */
/*  MS-DOS uses ":" for volume specifiction.                  */
/*  UNIX uses ":" to separator, usually.                      */

#ifdef MCH_AMIGA
# define  IS_EV_SEP(ch)  ((ch) == ';' || (ch) == '!')
#endif
#ifdef MSDOS
# define  IS_EV_SEP(ch)  ((ch) == ';' || (ch) == '!' || (ch) == ' ')
#endif

#ifndef IS_EV_SEP
# define  IS_EV_SEP(ch)  ((ch) == ';' || (ch) == '!' || (ch) == ':')
#endif


/* -----------------------  Private Variables  ------------------------- */

#define  LEN_NO_CORE  16
static char 	msg_no_core[ ] = { "newstring:CORE!\n" } ;


/* --------------------  Custom Memory Management  --------------------- */


#if SW_MEMTRACKER

	void *
myalloc( len, file )
	int 	len;
	char	*file;
{
	return( (void *) malloc(len) );
}


#  ifdef AZTEC_C
/*   myfree  --  Manx Aztec's runtime library version 3.6 has free()
 *               actually returning a non-zero if the memory to be
 *               release wasn't allocated with malloc().  This is an error.
 *         ANSI C states free() is type void.
 */
	void
myfree( p, file )
	void	*p;
	char	*file;
{
	if( free(p) != 0 )
	{
		printf( "* ERROR: free(%lx) %s *\n", p, file );
	}
}	/* myfree */

#  endif
#endif


/* ------------------------  String Supports  -------------------------- */


/*   lastchr  --  Return the last most non-space character.  If string all
 *                spaces or empty, returns 0.
 */
	unsigned char
lastchr( str )
    CONST char	*str;
{
    register char	*p;

    if( str != NULL )
        for( p=str+strlen(str) ; --p >= str ; )
            if( !isspace(*p) )
                return( (unsigned char) *p );

    return 0;
}	/* lastchr */



/*   strremove  --  Extract all occurances of a character from the string.
 *                  String is changed (shorted) in-place.
 */
	char *
strremove( str, outch )
    char	*str;
    char	outch;			/* Remove this char */
{
    register char	*t;

 top:
    for( t=str; *t != EOS ; ++t )
    {
    	if( *t == outch )
    	{
    		/*  Move string down to cover-up removed char: */
    		while( t[0] != EOS )
    		{
    			t[0] = t[1];
    			++t;
    		}
    		goto  top;
    	}
    }

    return( str );		/* Return pointer to passed string */
}	/* strremove */


/* ----------------------  Dynamic String Library  --------------------- */


/*   str_glue  --  Allocate some new space and place the two strings into
 *                 that space.  A one space separator is included if the
 *                 types of characters that might be joined require the
 *                 separation.  Usually identifiers and cmd line options
 *        are separated.  Always returns successfully.  Either (or both)
 *        pointers may be NULL.
 */
	static char *
str_glue( first, last )
    CONST char    *first;
    CONST char    *last;
{
    register char	*new;
    int 	len;

    if( first == NULL )
	    first = "" ;
    if( last == NULL )
	    last = "" ;

    len = strlen(first) + strlen(last) + 4;
    if( (new = MALLOC( len )) == NULL )
    {
    	write( 2, msg_no_core, LEN_NO_CORE );
    	abort();
    }

    strcpy( new, first );
    if( (len=strlen(first)) > 0 )
    {
        if( (isalnum(first[len-1]) || isalnum(last[0]) || last[0] == '_' ) ||
            (last[0] == '-' || last[0] == '+') )
        {
            strcat( new, " " );
        }
    }
    strcat( new, last );

    return( new );
}	/* str_glue */


/*   newstrjoin  --  Concat to static strings into one dynamically
 *                   alloc'ed string.  No separators between strings.
 *                   We can accept two NULL ptrs.
 */
	char *
newstrjoin( front, back )
	CONST char	*front;
	CONST char	*back;
{
    char	*new;
    int   len;

    /*  Society for the prevention of "references thru NULL ptrs"  */
    if( front == NULL )
	    front = "" ;
    if( back == NULL )
	    back = "" ;

    if( (new=malloc( strlen(front) + strlen(back) + 4)) == NULL )
    {
    	write( 2, msg_no_core, LEN_NO_CORE );
    	abort();
    }
    strcpy( new, front );
    strcat( new, back );

    return( new );
}	/* newstrjoin */


/*   newstring  --  Functionally like strdup(), but can handle NULL ptrs.
 *                  Allocates a few extra bytes, too!
 */
    char *
newstring( tocopy )
    CONST char   *tocopy;
{
    return( newstrjoin( NULL, tocopy ) );
}   /* newstring */



/*   newstr1chr  --  Join STR + CHR.  Nothing passed in is free'd.
 *                   OK if front == NULL.
 */
    char *
newstr1chr( front, lastchr )
    char   *front;
    char    lastchr;      /* append this char afterwards */
{
    char    mybuff[2];

    mybuff[0] = lastchr;
    mybuff[1] = 0;
    if( front == NULL )
    {
        front = newstring( mybuff );
    } else
    {
        strcat( front, mybuff );     /* Use that extra allocated byte */
    }
    return front;
}   /* newstr1chr */


/*   newstrcat  --  Given a (possibly) existing string, append another
 *                  string to it.  The 'old' string is result of calling
 *                  this function previously.  We can accept two NULL ptrs.
*/
	char *
newstrcat( old, fixed )
	char		*old;    	/* Will be free'd */
	CONST char	*fixed;
{
    char	*new;

    new = str_glue( old, fixed );
    if( old != NULL )
    	MFREE( old );
    return( new );
}	/* newstrcat */


    char *
newstrins( fixed, old )
    CONST char	*fixed;	    	/* String constant to prepend */
    char    	*old;	    	/* Dynamic string; will be free'd */
{
    char	*new;

    new = str_glue( fixed, old );
    if( old != NULL )
    	MFREE( old );
    return( new );
}	/* newstrins */


/* --------------------------------------------------------------------- */


/*   tokenizer  --  Accept an plain old string.  We separate it into
//		   tokens stored in argv[] style.  The array and string
//                  returned are owned by the caller.
//	    argv[0] == whole string,
//	    argv[1] == first token,
//	    argv[2] == second token, ...
//	    argv[n] == NULL (terminator).
*/
    char **
tokenizer( plainStr )
    CONST char      *plainStr;
{
    register char	**argv;  		/* Working array pointer */
    register char	*p;
    register int 	tokenCnt;
    char	**the_place;    		/* Array returned to caller */
    char	*the_string;

    the_string = newstring( plainStr );
    for( tokenCnt=3, p=the_string ; *p != EOS ; ++p )
    {
    	if( IS_EV_SEP(*p) )
    		++tokenCnt;
    }

    /*  Create an array to store tokens:  */
    the_place = (char **)calloc( sizeof(char *), tokenCnt );
    argv = the_place;
    *argv++ = the_string;	/* Store whole string at [0] */

    /*  Chop string into parts: */
    for( p=the_string ; *p != EOS ; )
    {
    	*argv++ = p;		/* The beginning of the token */
    	while( *p != EOS && !IS_EV_SEP(*p) )
    		++p;
    	if( *p != EOS )  	/* Separate directories */
    		*p++ = EOS ;
    }
    *argv = NULL;		/* Terminate array */

#if SW_WATCH
    printf (" (tokenizer) %dx <%s>\n", tokenCnt-2, the_place[0] );
    for( tokenCnt=1 ; the_place[tokenCnt] != NULL ; ++tokenCnt )
        printf( " (tokenizer) %d:%s.\n", tokenCnt, the_place[tokenCnt] );
    printf( "---\n" );
#endif
    return( the_place );
}	/* tokenizer */


