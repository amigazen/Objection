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
 *               to first occurrence, or NULL if 'target' is not found in 'buff'.
 */
char *
strstr(const char *buff, const char *target)
{
	char	*p;
	const char	*t;
	int 	first;
	int 	len_t;

	if( buff != NULL && target != NULL )
	{
		t = target;
		first = (unsigned char)t[0];
		len_t = strlen( t );
		p = (char *)buff;
		for( ; (p = strchr(p, first)) != NULL ; ++p )
		{
			if( strncmp( p, t, len_t ) == 0 )
				return p;
		}
	}
	return NULL;
}	/* strstr */

#endif


/*   strlower  --  Convert a string, in place, into lower case.
 *                 Returns ptr to converted string.  Handles NULL string.
 */
char *
strlower(char *str)
{
	char	*s;

    if( str != NULL )
    {
    	for( s=str ; *s ; ++s )
    		*s = tolower( *s );
    }
	return( str );
}	/* strlower */



/* ------------------------  ANSI ctype Support  ---------------------- */


#if  SW_NEED_2LOW_2UP

/*   tolower  --  Provide side-effect free ASCII tolower; tolower('5') == '5'. */
char
tolower(int ch)
{
	return (char)(isupper(ch) ? (ch + ('a' - 'A' + 1)) : ch);
}   /* tolower */


/*   toupper  --  Provide side-effect free ASCII toupper; toupper('5') == '5'. */
char
toupper(int ch)
{
	return (char)(islower(ch) ? (ch - ('a' - 'A' + 1)) : ch);
}   /* toupper */

#endif

