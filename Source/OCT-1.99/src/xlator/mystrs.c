/*   MYSTRS.C   Additional, ANSI-like string routines       May 91, bjw */

/*
 *    This file contains extra string functions that may or may not
 *    be needed at your site, depending on how your runtime library
 *    is implemented.  The file <objc/private/config.h> controls which
 *    routines are actually compiled in.
 *
 *    Function dictionary:
 *        char * strstr( const char *buff, const char *target )
 *        char * strlower( char	*str );
 *        char tolower( int   ch );
 *        char toupper( int   ch );
 *
 *    30-may-91    bjw    Ripped out of strs.c file.
 */



#include "oct.h"



/* ------------------------  ANSI String Support  --------------------- */


#if SW_NEED_STRSTR

/*   strstr  --  Locate a string within a larger string.  Returns pointer
//				 to first occurance, or NULL if 'target' string is not
//				 found within 'buff'.  This routine is fully public domain.
*/
	char *
strstr( buff, target )
	CONST char	*buff;      		/* Search here */
	CONST char	*target;      		/* For this whole string */
{ 
	register char	*p, *t;
	register int 	first;		/* Save sign extension time */
	int 	len_t;

    if( buff != NULL && target != NULL )
    {
        t = target;
        first = t[0];
        len_t = strlen( t );

        for( p=buff ; (p=strchr(p, first)) != NULL ; ++p )
        {
        	if( strncmp( p, t, len_t ) == 0 )
        		return( p );
        }
    }
	return NULL;		/* string not found */
}	/* strstr */

#endif


#if SW_NEED_STRLOWER

/*   strlower  --  Convert a string, in place, into lower case.
 *                 Returns ptr to converted string.  Handles NULL string.
*/
	char *
strlower( str )
	char	*str;
{
	register char	*s;

    if( str != NULL )
    {
    	for( s=str ; *s ; ++s )
    		*s = tolower( *s );
    }
	return( str );
}	/* strlower */

#endif



/* ------------------------  ANSI ctype Support  ---------------------- */


#if  SW_NEED_2LOW_2UP

/*   tolower  --  Provide side-affect free ASCII tolower function that does
 *                tolower('5') == '5'.
 */
    char
tolower( ch )
    int   ch;
{
    return isupper(ch) ? (ch + ('a' - 'A' + 1)) : ch ;
}   /* tolower */


/*   toupper  --  Provide side-affect free ASCII toupper function that does
 *                toupper('5') == '5'.
 */
    char
toupper( ch )
    int   ch;
{
    return islower(ch) ? (ch - ('a' - 'A' + 1)) : ch ;
}   /* toupper */

#endif

