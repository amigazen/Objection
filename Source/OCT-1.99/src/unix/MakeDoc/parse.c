/************************************************************************/
/* Copyright the Real-Time and Distributed Systems Group,		*/
/* Department of Systems and Computer Engineering,			*/
/* Carleton University, Ottawa, Ontario, Canada. K1S 5B6		*/
/*									*/
/* October 1989.							*/
/* 									*/
/*									*/
/* This software is published on an as-is basis. There is ABSOLUTELY NO	*/
/* WARRANTY for any part of this software to work correctly or as	*/
/* described in the manuals. We do not accept any liability for any	*/
/* kind of damage caused by use of this software, such as loss of data,	*/
/* time, money, or effort.						*/
/*									*/
/* Permission is granted to use, copy, modify, or distribute any part	*/
/* of this software as long as this is done without asking for		*/
/* charge, and provided that this copyright notice is retained as	*/
/* part of the source files. You may charge a distribution fee for	*/
/* the physical act of transferring a copy, and you may at your		*/
/* option offer warranty protection in exchange for a fee.		*/
/*									*/
/* Please send comments or suggestions to:				*/
/*	Greg Franks							*/
/* 	Department of Systems and Computer Engineering,			*/
/* 	Carleton University, Ottawa, Ontario, Canada. K1S 5B6		*/
/*	(613) 788-5736							*/
/*									*/
/*	BITNET: greg@sce.carleton.ca					*/
/*	UUCP: uunet!mitel!sce!greg					*/
/************************************************************************/

/*
 * $Source: /usr/sirius/greg/src/makedoc/RCS/parse.c,v $
 *	Print out the method cross reference.
 */

#ifndef lint
static char	*RCSid = "$Header: /usr/sirius/greg/src/makedoc/RCS/parse.c,v 3.4 89/10/24 14:30:29 greg Exp Locker: greg $";
#endif

/*
 * $Log:	parse.c,v $
 * Revision 3.4  89/10/24  14:30:29  greg
 * Print out 'classes used' information.
 * Print out last modified time of .m file.
 * 
 * Revision 3.3  89/10/11  18:02:56  greg
 * Print out functions properly.  Do not treat them as methods.
 * 
 * Revision 3.2  89/10/10  17:30:44  greg
 * Print out standard function names.
 * Print out stuff to make indexes.
 * 
 * Revision 3.1  89/10/10  11:40:03  greg
 * > Output now goes to .tex file.
 * -o and -c ala cc now added.
 * -c becomes -C.
 * 
 * -m becomes -M.
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#ifdef USG
#include <string.h>
#endif
#include "makedoc.h"

void AddMethodName();
boolean LooksLikeTex();
CommentBuf * DigestCCComment(), * DigestObjCComment(), * TerminateComment();
IncludeBuf * AddInclude();
void FreeCommentList();
char *  MakeName();
extern char * malloc();
void FreeIncludeList();
int FindClassName();
int FindMethodName();

/*
 * LooksLikeMethod
 *	Looks for methods in objective C files.  We have to search for
 *	a slightly different set of characters.
 */

boolean terminal_state[] = {
        /*  0 */	false,
	/*  1 */	false,
	/*  2 */	false,
	/*  3 */	true,
	/*  4 */	true,
	/*  5 */	false,
	/*  6 */	false,
	/*  7 */	true,
	/*  8 */	true,
	/*  9 */	true,
        /* 10 */	false,
	/* 11 */	true,
	/* 12 */	false,
	/* 13 */	false,
	/* 14 */	false,
	/* 15 */	false,
	/* 16 */	false,
	/* 17 */	true,
	/* 18 */	false,
	/* 19 */	false,
	/* 20 */	false,
	/* 21 */	false,
	/* 22 */	false,
	/* 23 */	false,
	/* 24 */	false,
	/* 25 */	false,
	/* 26 */	false,
	/* 27 */	false,
	/* 28 */	false,
	/* 29 */	false,
	/* 30 */	false,
	/* 31 */	false,
	/* 32 */	true
};

int
ReadFile( fp, FileName, nesting )
FILE *fp;
char *FileName;
int nesting;
{
	register char *p;
	register int c;			/* current character 		*/
	char *s;
	register int state	= 0;
	char FunctionName[BUFSIZ];
	char ClassName[BUFSIZ];
	char SuperClassName[BUFSIZ];
	int Braces 		= 0;
	int index		= 0;
	CommentBuf * CommentPtr = 0;
	CommentBuf * StartCommentPtr	 = 0;
	IncludeBuf * IncludePtr = 0;
	IncludeBuf * StartIncludePtr	 = 0;
	long space_count = 0;		/* spaces between tokens.	*/
	char * ArgList[ARGLIST_SIZE];
	int ArgCount;
	enum { NONE_FOUND, A_FUNCTION, A_METHOD } TypeFound;
	
	ArgCount = 1;
	TypeFound = NONE_FOUND;
	
	if ( nesting == 0 ) {
		StartIncludePtr = IncludePtr = (IncludeBuf *)malloc( sizeof( struct IncludeBuf ) );
		if ( IncludePtr ) { 
			IncludePtr->next = (IncludeBuf *)0;
			IncludePtr->buf = (char *)0;
		}
	}

	c = getc( fp );

	while ( c != EOF ) {

		/* Look for interesting events.  Strip strings and 	*/
		/* comments.						*/

		switch ( c ) {
		case '{':
			++Braces;
			if ( Braces == 1  && terminal_state[state] && nesting == 0 ) {
				*p++ = 0;
				ArgList[0] = MakeName( FunctionName );
				
				switch( TypeFound ) {
				case A_METHOD:
					AddMethodName( ClassName, ArgList );
					PrintMethod( FunctionName, ArgList, ArgCount, StartCommentPtr );
					break;
				case A_FUNCTION:
					PrintFunction( FunctionName, ArgList, ArgCount, StartCommentPtr );
					break;
				}
				free( ArgList[0] );
				FreeCommentList( StartCommentPtr );
				StartCommentPtr = CommentPtr = (CommentBuf *)0;
				state = 0;
				TypeFound = NONE_FOUND;
				ArgCount = 1;
			}
			break;

		case '}':
			if ( --Braces < 0 ) {
				Braces = 0;
			}
			break;

		case '\'':
			c = getc( fp );
			while ( c != '\'' ) {
				if ( c == '\\' ) {
					c = getc( fp );
				}
				c = getc( fp );
			}
			break;

		case '"':
			if ( state == 35 || state == 38 ) break;	/* Override for include processing */
			while ( c = getc( fp ) ) {
				if ( c == '\\' ) {
					c = getc( fp );
				} else if ( c == '"' ) {
					break;
				}
			}
			break;

		case '/':
			c = getc( fp );
			if ( c == '/' || c == '*' ) {
				if ( !CommentPtr && Braces == 0 && nesting == 0 ) {
					StartCommentPtr = CommentPtr = (CommentBuf *)malloc( sizeof( struct CommentBuf ) );
					if ( StartCommentPtr ) {
						StartCommentPtr->n = 0;
						StartCommentPtr->buf[0] = '\0';
					}
				}
				if ( c == '/' ) {
					space_count = 0;
					CommentPtr = DigestObjCComment( fp, CommentPtr );
				} else {
					space_count = -1;
					CommentPtr = DigestCCComment( fp, CommentPtr );
				}
				c = ' ';	/* Comments become a space	*/

			} else {
				(void) ungetc( c, fp );
				c = '/';
			}
			break;

		default:
			if ( isspace( c ) ) {
				space_count += 1;
			}
			break;
		}

		if ( space_count > 0 && StartCommentPtr && LooksLikeTex( StartCommentPtr ) ) {
			PrintComment( StartCommentPtr, (char **)0, 0 );
			FreeCommentList( StartCommentPtr );
			StartCommentPtr = CommentPtr = (CommentBuf *) 0;
		}
			
		if ( Braces == 0 ) {

			switch ( state ) {

			/************************************************/
			/*						*/
			/* Ground State.				*/
			/*						*/
			/************************************************/

			case 0:		/* Look for <'+'|'-'>		*/
				if ( c == '+' || c == '-' ) {
					p = FunctionName;
					TypeFound = A_METHOD;
					*p++ = c | 0x80;
					state = 1;
				} else if ( c == '#' ) {
					p = FunctionName;
					*p = '\0';
					state = 33;
				} else if ( isidchr( c ) ) {
					p = FunctionName;
					TypeFound = A_FUNCTION;
					*p++ = c | 0x80;
					state = 14;
				} else if ( c == '@' ) {
					p = FunctionName;
					*p = '\0';
					state = 23;
				}
				break;

			/************************************************/
			/*						*/
			/* Objective C method Name.			*/
			/*						*/
			/************************************************/

			case 1:		/* Look for method name		*/
				if ( c == '(' ) {
					*p++ = c;
					state = 2;
				} else if ( isidchr( c ) && !isdigit( c ) ) {
					*p++ = c | 0x80;
					state = 3;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 2:		/* strip leading type info	*/
				if ( c == ')' ) {
					*p++ = c;
					state = 1;
				} else if ( isidchr( c ) || c == '*' ) {
					*p++ = c;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/*----------------------------------------------*/
			/* Method Name					*/
			/*						*/
			/* <name> ':'					*/
			/*----------------------------------------------*/

			case 3:		/* Copy method name		*/
				if ( isidchr( c ) ) {
					*p++ = c | 0x80;
				} else if ( isspace( c ) ) {
					state = 4;
				} else if ( c == ':' ) {
					*p++ = c | 0x80;
					state = 5;
				} else {
					state = -1;
				}
				break;

			case 4:		/* Strip space after <name>	*/
				if ( c == ':' ) {
					*p++ = c | 0x80;
					state = 5;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 5:		/* strip info before <argument>	*/
				if ( c == '(' ) {
					*p++ = c;
					state = 6;
				} else if ( isidchr( c ) && !isdigit( c ) ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 7;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 6:		/* strip type info before <argument>	*/
				if ( c == ')' ) {
					*p++ = c;
					state = 5;
				} else if ( isidchr( c ) || c == '*' ) {
					*p++ = c;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/*----------------------------------------------*/
			/* Generic argument to be ignored.              */
			/*				                */
			/* <name> [':' | ',' | ';' ]	                */
			/*----------------------------------------------*/

			case 7:		/* strip <argument>		*/
				if ( isspace( c ) ) {
					*p++ = ' ';
					state = 8;
				} else if ( c == ':' ) {
					*p++ = c | 0x80;
					state = 5;
				} else if ( c == ',' ) {
					*p++ = c;
					state = 9;
				} else if ( c == ';' ) {
					*p++ = c;
					state = 11;
				} else if ( isidchr( c ) ) {
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 8:
				if ( c == ':' ) {
					*p++ = c | 0x80;
					state = 5;
				} else if ( c == ',' ) {
					*p++ = c;
					state = 9;
				} else if ( c == ';' ) {
					*p++ = c;
					state = 11;
				} else if ( isidchr( c ) && !isdigit( c ) ) {
					*p++ = c | 0x80;
					state = 3;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/*----------------------------------------------*/
			/* New style argument declaration.		*/
			/*						*/
			/* '(' <type name> '*'* ')' <name>		*/
			/*----------------------------------------------*/

			case 9:		/* strip info before ',' <argument>	*/
				if ( c == '(' ) {
					*p++ = c;
					state = 10;
				} else if ( isidchr( c ) && !isdigit( c ) ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 7;
				} else if ( c == '.' ) {
					*p++ = c;
					state = 30;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 10:	/* strip type info before <argument>	*/
				if ( c == ')' ) {
					*p++ = c;
					state = 9;
					break;
				} else if ( isidchr( c ) || c == '*' ) {
					*p++ = c;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/*----------------------------------------------*/
			/* Old style argument declaration.		*/
			/*						*/
			/* ["struct"|"union"] <type name> '*'* <name>	*/
			/*----------------------------------------------*/

			case 11:	/* ["struct"|"union"]		*/
				if ( isidchr( c ) && !isdigit( c ) ) {
					s = p;
					*p++ = c;
					state = 12;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 12:	/* <type name>			*/
				if ( isspace( c ) || c == '*' ) {
					*p = '\0';
					if ( strcmp( s, "struct" ) == 0 || strcmp( s, "union" ) == 0 ) {
						state = 11;
					} else {
						state = 13;
					}
					*p++ = ( c == '*' ) ? '*' : ' ';
				} else if ( isidchr( c ) ) {
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 13:	/* '*'*					*/
				if ( isidchr( c ) && !isdigit( c ) ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 7;
				} else if ( c == '*' ) {
					*p++ = c;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;


			/************************************************/
			/*						*/
			/* Generic C function Name.			*/
			/*						*/
			/************************************************/

			case 14:
				if ( isspace( c ) || c == '*' ) {
					*p++ = (c == '*') ? '*' : ' ';
					state = 15;
				} else if ( isidchr( c ) ) {
					*p++ = c | 0x80;
				} else if ( c == '(' ) {
					*p++ = c | 0x80;
					state = 16;
				} else {
					state = -1;
				}
				break;

			case 15:
				if ( isidchr( c ) ) {
					char *q;
					
					for ( q = FunctionName; q < p; ++q ) {
						*q &= ~0x80;	/* Nuke marker. */
					}

					*p++ = c | 0x80;
					state = 14;
				} else if ( c == '(' ) {
					*p++ = c | 0x80;
					state = 16;
				} else if ( c == '*' ) {
					*p++ = c;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 16:
				if ( c == ')' ) {
					*p++ = c | 0x80;
					state = 17;
				} else if ( isidchr( c ) || c == ',' || c == '*' ) {
					*p++ = c;
				} else if ( isspace( c ) ) {
					*p++ = ' ';
				} else {
					state = -1;
				}
				break;

			case 17:	/* ["struct"|"union"]			*/
				if ( isidchr( c ) && !isdigit( c ) ) {
					s = p;
					*p++ = c;
					state = 18;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 18:	/* <type name>				*/
				if ( isspace( c ) || c == '*' ) {
					*p = '\0';
					if (   strcmp( s, "struct" )   == 0
					    || strcmp( s, "union" )    == 0
					    || strcmp( s, "register" ) == 0 ) {
						state = 17;
					} else {
						state = 19;
					}
					*p++ = ( c == '*' ) ? '*' : ' ';
				} else if ( isidchr( c ) ) {
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 19:	/* '*'*					*/
				if ( c == '*' ) {
					*p++ = c;
					break;
				} else if ( isidchr( c ) && !isdigit( c ) ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 20;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 20:		/* strip <argument>			*/
				if ( isspace( c ) ) {
					state = 21;
				} else if ( c == ',' ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 19;
				} else if ( c == ';' ) {
					*p++ = c;
					state = 17;
				} else if ( isidchr( c ) ) {
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 21:
				if ( c == ',' ) {
					if ( ArgCount < ARGLIST_SIZE ) {
						ArgList[ArgCount++] = p;
					}
					*p++ = c;
					state = 19;
				} else if ( c == ';' ) {
					*p++ = c;
					state = 17;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/************************************************/
			/*						*/
			/* Implementation stuff.			*/
			/*						*/
			/* @implementation GNode : Rectangle 		*/
			/************************************************/

			case 23:
				if ( isalpha( c ) ) {
					*p++ = c;
					state = 24;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 24:
				if ( isalpha( c ) ) {
					*p++ = c;
				} else if ( isspace( c ) ) {
					*p = '\0';
					if (   strcmp( FunctionName, "implementation" ) == 0
					    || strcmp( FunctionName, "interface" ) == 0 ) {
						state = 25;
					} else {
						state = -1;
					}
				} else {
					state = -1;
				}
				break;

			case 25:
				if ( isidchr( c ) ) {
					s = ClassName;
					*s++ = c;
					state = 26;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 26:
				if ( isidchr( c ) ) {
					*s++ = c;
				} else if ( c == ':' ) {
					*s = '\0';
					state = 28;
				} else if ( isspace( c ) ) {
					*s = '\0';
					state = 27;
				} else {
					state = -1;
				}
				break;

			case 27:
				if ( c == ':' ) {
					state = 28;
				} else if ( !isspace( c ) ) {
					index = AddClassName( ClassName, (char *)0, FileName, nesting );
					if ( nesting == 0 ) {
				 		PrintClassHeader( index, FileName, StartCommentPtr, StartIncludePtr );
						FreeIncludeList( StartIncludePtr );
						StartIncludePtr = IncludePtr = (IncludeBuf *)0;
					}
					state = -1;
				}
				break;

			case 28:
				if ( isidchr( c ) ) {
					s = SuperClassName;
					*s++ = c;
					state = 29;
				} else if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			case 29:
				if ( isidchr( c ) ) {
					*s++ = c;
				} else { 
					*s = '\0';
					index = AddClassName( ClassName, SuperClassName, FileName, nesting );
					if ( nesting == 0 ) {
				 		PrintClassHeader( index, FileName, StartCommentPtr, StartIncludePtr );
						FreeIncludeList( StartIncludePtr );
						StartIncludePtr = IncludePtr = (IncludeBuf *)0;
					}
					state = -1;
				}
				break;

			/************************************************/
			/*						*/
			/* special ... arg.				*/
			/*						*/
			/************************************************/

			case 30:
				if ( c == '.' ) {
					state = 31;
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 31:
				if ( c == '.' ) {
					state = 32;
					*p++ = c;
				} else {
					state = -1;
				}
				break;

			case 32:
				if ( !isspace( c ) ) {
					state = -1;
				}
				break;

			/************************************************/
			/*						*/
			/* CPP.						*/
			/*						*/
			/************************************************/

			case 33:
				if ( isalpha( c ) ) {
					state = 34;
					*p++ = c;
				} else if ( c == '\n' ) {
					state = -1;
				}
				break;

			case 34:
				if ( isalpha( c ) ) {
					*p++ = c;
				} else if ( c == '\n' ) {
					state = -1;
				} else if ( isspace( c ) ) {
					state = 35;
				}
				break;

			case 35:
				if (   (c == '"' || c == '<')
				    && (strncmp( FunctionName, "import", 6 ) == 0 || strncmp( FunctionName, "include", 7 ) == 0 ) ) {
					p = FunctionName;
					*p = '\0';
					state = 38;
				} else if ( c == '\n' ) {
					state = -1;
				} else if ( !isspace( c ) ) {
					state = 36;
				}
				break;

			case 36:
				if ( c == '\\' ) {
					state = 37;
				} else if ( c == '\n' ) {
					state = -1;
				}
				break;

			case 37:		/* Flush char after \ */
				state = 36;
				break;

			case 38:
				if ( c == '"' || c == '>' ) {
					*p++ = '\0';
					IncludePtr = AddInclude( FunctionName, FileName, IncludePtr );
					state = -1;
				} else if ( c == '\n' ) {
					state = -1;	/* error */
				} else {
					*p++ = c;
				}
				break;

			default:
				(void) fprintf( stderr, "%s: Main State machine error in %s\n", ProgName, FileName );
				exit( 1 );

			}

			/* End state reached.  Reset state to 0 and	*/
			/* flush accumulated comments.			*/
			
			if ( state == -1 ) {
				state = 0;
				if ( StartCommentPtr ) {
					FreeCommentList( StartCommentPtr );
					StartCommentPtr = CommentPtr = (CommentBuf *)0;
				}
			}
		}
		c = getc( fp );
	}
	
	return index;
}



static CommentBuf *
DigestCCComment( fp, bufp )
FILE * fp;
CommentBuf * bufp;
{
	register int c;
	register int n;
	register char *p;
	register int state	= 0;
	
	if ( bufp ) {
		n = bufp->n;
		p = bufp->buf + n;
	}
	
	c = getc( fp );
	while ( c != EOF ) {

		if ( n >= COMMENT_BUFSIZ ) {
			bufp->n    = COMMENT_BUFSIZ;
			bufp->next = (CommentBuf *)malloc( sizeof( struct CommentBuf ) );
			if ( bufp->next ) {
				bufp = bufp->next;
				bufp->next = 0;
			} 
			p = bufp->buf;
			n = 0;
		}

		switch( state ) {
		case 0:
			if ( c == '*' ) {
				state = 1;
			} else if ( c != '-' && !isspace( c ) ) {
				state = 2;
			}
			break;

		case 1:
			if ( c == '/' ) {
				goto done;
			} else if ( isspace( c ) ) {
				state = 0;
			} else if ( c != '*' ) {
				state = 2;
			}
			break;

		case 2:
			if ( c == '*' ) {
				state = 3;
			} else if ( c == '\n' ) {
				state = 0;
			}
			break;

				   
		case 3:
			if ( c == '/' ) {
				goto done;
			} else if ( c == '\n' ) {
				state = 0;
			} else {
				state = 2;
				continue;
			}
			break;
		}

		if ( bufp && (state == 2 || (state == 0 && c == '\n') ) ) {
			*p++ = c;
			++n;
		}
		
		c = getc( fp );
	}

 done:
	if ( bufp ) {
		bufp->n = n;
		return TerminateComment( bufp );
	} else {
		return 0;
	}
}



static CommentBuf *
DigestObjCComment( fp, bufp )
FILE * fp;
CommentBuf * bufp;
{
	register int c;
	register int n;
	register char *p;
	register int state	= 0;
	
	if ( bufp ) {
		n = bufp->n;
		p = bufp->buf + n;
	}
	
	c = getc( fp );
	while ( c != EOF && c != '\n' ) {

		if ( n >= COMMENT_BUFSIZ ) {
			bufp->n    = COMMENT_BUFSIZ;
			bufp->next = (CommentBuf *)malloc( sizeof( struct CommentBuf ) );
			if ( bufp->next ) {
				bufp = bufp->next;
				bufp->next = 0;
			} 
			p = bufp->buf;
			n = 0;
		}

		switch( state ) {
		case 0:
			if ( c != '/' && c != '-' && !isspace( c ) ) {
				state = 1;
			}
			break;

		case 1:
			break;
		}
		
		if ( bufp && state == 1 ) {
			*p++ = c;
			n++;
		}

		if ( c == '\n' ) break;

		c = getc( fp );
	}

	if ( bufp ) { 
		bufp->n = n;
		bufp->next = 0;
		return TerminateComment( bufp );
	} else {
		return 0;
	}
}



/*
 * 	Strip trailing whitespace (up to boudary), then tack on newline.
 *	(Generalized to tack on anything)
 */

static CommentBuf * 
TerminateComment( bufp )
register CommentBuf * bufp;
{
	register char * p;
	register int n;
	register char *q;

	q = "\n";
	
	if ( bufp ) {
		n = bufp->n;
		p = bufp->buf + n;
		do { 
			--p;
			--n;
		} while ( n >= 0 && isspace( *p ) );

		/* We are now at a non white space character or at the	*/
		/* beginning of the buffer.				*/

		n++;
		p++;

		while( *q ) {
			if ( n >= COMMENT_BUFSIZ ) {
				bufp->next = (CommentBuf *)malloc( sizeof( struct CommentBuf ) );
				if ( bufp->next ) {
					bufp = bufp->next;
					bufp->next = 0;
				}
				p = bufp->buf;
				n = 0;
			}
			*p++ = *q++;
			n++;
		};
		bufp->n = n;
		bufp->next = 0;
	}
	return bufp;
}



/*
 * AddInclude
 *	Check name of include file and add it to the list iff
 *	appropriate.  The test is simple - look for
 *	an upper case letter as the first character in the name
 *	after the last '/'.  Also make sure the include file
 *	name does not match our file name.
 *	Don't forget to strip file types.
 */

static IncludeBuf *
AddInclude( Name, FileName, bufp )
char * Name;
char * FileName;
IncludeBuf *bufp;
{
	register char * p;
	register char * q;
	register char * r;
	register int n;
	
	p = strrchr( Name, '/' );
	p = (!p) ? Name : p + 1;
	q = strrchr( FileName, '/' );
	q = (!q) ? FileName : q + 1;
	r = strchr( q, '.' );
	
	if ( bufp && 'A' <= *p && *p <= 'Z' && strncmp( p, FileName, ((r != 0) ? r - q: strlen( q )) ) != 0 ) {
		r = strchr( p, '.' );
		n = (r != 0) ? r - p : strlen( p );
		if ( bufp && (bufp->buf = (char *)malloc( n + 1 ) ) != (char *) 0 ) {
			(void) strncpy( bufp->buf, p, n );
			bufp->buf[n] = '\0';
		}
		bufp->next = (IncludeBuf *)malloc( sizeof( IncludeBuf ) );
		bufp = bufp->next;
		if ( bufp ) {
			bufp->next = (IncludeBuf *)0;
			bufp->buf = (char *)0;
		}
	}
	return bufp;
}



/*
 * AddClassName
 *	See if we have already added this class to the has table.  If
 *	we have, we're done.  Otherwise, continue chaining backwards
 *	through the interface files recursively.  When we eventually
 *	run out of files, add the superclass name to the table.
 */

static int
AddClassName( ClassName, SuperClassName, FileName, nesting )
char * ClassName;
char * SuperClassName;
char * FileName;
int nesting;
{
	static boolean no_can_do 	= false;
	register int index;
	register int i;
	register BackLink ** p;
	BackLink *q;
 	int j;
	
	if ( no_can_do ) return 0;
	
	index = FindClassName( ClassName );
	if ( index == 0 ) {
		(void) fprintf( stderr, "%s: Class table full, ignoring from %s\n", ProgName, ClassName );
		no_can_do = true;
	} else if ( !ClassNameTable[index].class_name ) {
		ClassNameTable[index].class_name = (char *) malloc( strlen( ClassName ) );
		if ( !ClassNameTable[index].class_name ) {
			(void) fprintf( stderr, "%s: No more core, ignoring from %s.\n", ProgName, ClassName );
			no_can_do = true;
		} else {
			(void) strcpy( ClassNameTable[index].class_name, ClassName );
		}
	}

	if ( ClassNameTable[index].class_name && ClassNameTable[index].parent_class_index == -1 ) {
		if ( SuperClassName != (char *)0 ) {
			ClassNameTable[index].parent_class_index = FindSuperClass( SuperClassName, nesting+1 );
			if ( ClassNameTable[index].parent_class_index == 0 ) {
				ClassNameTable[index].parent_class_index = AddClassName( SuperClassName, (char *)0, (char *)0, nesting+1 );
			}
		} else if ( nesting == 0 ) {

			/* Perhaps our include file has definition.  	*/
			/* Guess that the name is Filename.h		*/

			int len = strlen( FileName );
			if ( FileName[len-2] == '.' && FileName[len-1] == 'm' ) {
				FileName[len-2] = '\0';
				(void)FindSuperClass( FileName, nesting+1 );
				FileName[len-2] = '.';
			} else {
				ClassNameTable[index].parent_class_index = 0;
			}
		} else {
			ClassNameTable[index].parent_class_index = 0;
		}

		/* Forward link from the superclass to the class for	*/
		/* latter processing.  Forward links are made up of	*/
		/* a linked list.  Do not duplicate labels.  Sort now	*/
		/* while the sorting is easy.				*/
		
		i = ClassNameTable[index].parent_class_index;
		p = &ClassNameTable[i].next;
		while ( *p ) {
			j = strcmp( ClassNameTable[(**p).index].class_name, ClassName );
			if ( j < 0 ) {
				p = &(**p).next;
			} else if ( j == 0 ) {
				goto done;
			} else {
				break;
			}
		}
		q = (BackLink *)malloc( sizeof( struct BackLink ) );
		if ( q ) {
			q->index = index;
			q->next = *p;
			*p = q;
		} else {
			(void) fprintf( stderr, "%s: no more core\n", ProgName );
			no_can_do = true;
		}
		
	} 
 done:
	return index;

}



static int
FindClassName( ClassName )
char * ClassName;
{
	int index;
	register int i;
	
	index = HashClassName( ClassName );
	for ( i = (index+1)%MAX_CLASS_ENTRIES; i != index; i = (i + 1)%MAX_CLASS_ENTRIES ) {
		if ( !ClassNameTable[i].class_name || strcmp( ClassName, ClassNameTable[i].class_name ) == 0 ) return i;
	}
	return 0;
}



static int
HashClassName( p )
register char *p;
{
	register int i;

	i = 0;
	while( *p ) {
		i = (i + *p++) % MAX_CLASS_ENTRIES;
	}
	return i ;
}



static int
FindSuperClass( ClassName, nesting )
char *ClassName;
{
	FILE * fptr;
	char FileName[BUFSIZ];
	int index = 0;
	
	if ( fptr = OpenIncludeFile( ClassName, FileName ) ) {
		index = ReadFile( fptr, FileName, nesting );
		(void) fclose( fptr );
	}
	return index;
}


static char *
MakeName( s )
register char *s;
{
	register char * p;
	char * MethodName;
	static boolean no_can_do = false;
	
	if ( no_can_do ) return (char *)0;
	 
	MethodName = (char *)malloc( strlen( s ) );
	if ( !MethodName ) {

		(void) fprintf( stderr, "%s: No more memory!\n", ProgName );
		no_can_do = true;

	} else { 
		
		/* Extract only the selector name.  Strip the magic bit */
		/* that denotes the selector portion.			*/
		
		p = MethodName;		
		for( ; *s; ++s ) {
			if ( *s & 0x80 ) *p++ = *s & 0x7f;
		}
		*p++ = '\0';
	}
	return( MethodName );
}



/*
 * AddMethodName
 *	Add a method to the method name table then link it to its class name.
 */

static void
AddMethodName( ClassName, ArgList )
char * ClassName;
char * ArgList[];
{
	register int index;
	char * MethodName 	= ArgList[0];
	BackLink *q;
	static boolean no_can_do = false;

	if ( no_can_do ) return;
	
	if ( *MethodName ) {
		
		/* Locate or install the method name in the table.		*/
		
		index = FindMethodName( MethodName );
		if ( index == 0 ) {
			(void) fprintf( stderr, "%s: Method table full, ignoring from %s\n", ProgName, MethodName );
			no_can_do = true;
		} else if ( !MethodNameTable[index].method_name ) {
			MethodNameTable[index].method_name = (char *) malloc( strlen( MethodName ) );
			if ( !MethodNameTable[index].method_name ) {
				(void) fprintf( stderr, "%s: No more core, ignoring from %s.\n", ProgName, MethodName );
				no_can_do = true;
			} else {
				(void) strcpy( MethodNameTable[index].method_name, MethodName );
			}
		}
		
		if ( index ) {
			
			/* Link to the class name.			*/
			
			q = (BackLink *)malloc( sizeof( struct BackLink ) );
			if ( q ) {
				q->index = FindClassName( ClassName );
				q->next =  MethodNameTable[index].next;
				MethodNameTable[index].next = q;
			} else {
				(void) fprintf( stderr, "%s: no more core\n", ProgName );
				no_can_do = true;
			}
			
		}
	}
}



static int
FindMethodName( MethodName )
char * MethodName;
{
	int index;
	register int i;
	
	index = HashMethodName( MethodName );
	for ( i = (index+1)%MAX_METHOD_ENTRIES; i != index; i = (i + 1)%MAX_METHOD_ENTRIES ) {
		if ( !MethodNameTable[i].method_name || strcmp( MethodName, MethodNameTable[i].method_name ) == 0 ) return i;
	}
	return 0;
}


static int
HashMethodName( p )
register char *p;
{
	register unsigned int i;

	i = 0;
	while( *p ) {
		i = (i + *p++) % MAX_CLASS_ENTRIES;
	}
	return (int)i;
}

/*----------------------------------------------------------------------*/


static void
FreeCommentList( CommentPtr )
CommentBuf * CommentPtr;
{
	if ( CommentPtr ) {
		FreeCommentList( CommentPtr->next );
		(void) free( (char *)CommentPtr );
	}
}



static void
FreeIncludeList( IncludePtr )
IncludeBuf * IncludePtr;
{
	if ( IncludePtr ) {
		FreeIncludeList( IncludePtr->next );
		(void) free( IncludePtr->buf );
		(void) free( (char *)IncludePtr );
	}
}



/*
 * LooksLikeTex
 *	This function returns true if the first non-white-space character
 *	in a comment string is a backslash.
 */

static boolean
LooksLikeTex( CommentPtr ) 
CommentBuf * CommentPtr;
{
	char * p;
	int n;
	
	n = 0;
	while ( CommentPtr ) {
		for ( p = CommentPtr->buf, n = CommentPtr->n; n > 0; ++p, --n ) {
			if ( *p == '\\' ) {
				return true;
			} else if ( !isspace( *p ) ) {
				return false;
			}
		}
		CommentPtr = CommentPtr->next;
	}
	return false;
}
