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
 * $Source: /usr/sirius/greg/src/makedoc/RCS/makedoc.c,v $
 *	Search through files looking for magical comments and extract them
 *	thus creating beautiful documentation.
 *
 *	Input is assumed to be objective C.
 */

#ifndef lint
static char	*RCSid = "$Header: /usr/sirius/greg/src/makedoc/RCS/makedoc.c,v 3.6 89/10/24 14:29:55 greg Exp Locker: greg $";
#endif

/*
 * $Log:	makedoc.c,v $
 * Revision 3.6  89/10/24  14:29:55  greg
 * Print out 'classes used' information.
 * Print out last modified time of .m file.
 * 
 * Revision 3.5  89/10/11  18:02:30  greg
 * Print out functions properly.  Do not treat them as methods.
 * 
 * Revision 3.4  89/10/10  17:30:27  greg
 * Print out standard function names.
 * Print out stuff to make indexes.
 * 
 * Revision 3.3  89/10/10  11:38:09  greg
 * Output now goes to .tex file. 
 * -o and -c ala cc now added.
 * -c becomes -C
 * -m becomes -M
 * 
 * Revision 3.2  89/10/04  20:53:12  greg
 * Print comments properly.  Flush tex commands upon arrival.
 * 
 * Revision 3.1  89/10/04  10:14:48  greg
 * Print out method index.  This scheme doesn't work.
 * 
 * Revision 2.4  89/10/03  12:38:43  greg
 * Proper printing of class hierarchy.
 * Now searches for methods and indexes.
 * 
 * Revision 2.3  89/10/02  17:01:19  greg
 * tex drawing routines for hierarchy.
 * 
 * Revision 2.2  89/10/02  13:02:44  greg
 * Now restrict number of levels printed horizontally.
 * 
 * Revision 2.1  89/10/02  11:31:59  greg
 * Now prints out class hierarchies.  Table of contents stuff removed.
 * 
 * Revision 1.4  89/09/29  20:21:38  greg
 * Handles all comment forms properly.  Even searches include files.
 * 
 * Revision 1.3  89/09/29  18:49:04  greg
 * Now handles and displays commets (quote mode for methods).
 * 
 * Revision 1.2  89/09/29  13:53:48  greg
 * Now searches for dependencies.
 * 
 * Revision 1.1  89/09/29  08:42:06  greg
 * Initial revision
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#ifndef AZTEC_C
# include <sys/types.h>
# include <sys/stat.h>
#endif

#ifdef USG
# include <string.h>
#endif

#include "makedoc.h"


FILE	*i_fptr;

char * IncludeDir[MAX_DIRS];
int IncludeCount 	= 0;
char * Defines[MAX_DEFINES];
int DefinesCount	= 0;

#ifdef MCH_AMIGA
char * CPPstring 	= "cpp ";
#else
char * CPPstring 	= "/lib/cpp -E -P ";
#endif
char * ProgName;

FILE * o_fptr		= (FILE *)0;	/* Output file pointer.		*/
FILE * g_fptr		= (FILE *)0;	/* Global File pointer.		*/
FILE * c_fptr		= (FILE *)0;	/* Output file for hierarchy. 	*/
FILE * m_fptr		= (FILE *)0;	/* Output file for hierarchy. 	*/

boolean PutHeader	= true;
boolean MakeGlobalFile	= false;
boolean ClassFilePresent= false;

/*----------------------------------------------------------------------*/
/* Class Hierarch printing stuff.					*/
/*----------------------------------------------------------------------*/

char * ClassFileName	= (char *)0;
ClassNameEntry ClassNameTable[MAX_CLASS_ENTRIES];


/*----------------------------------------------------------------------*/
/* Method Index  printing stuff.					*/
/*----------------------------------------------------------------------*/

char * MethodFileName	= (char *)0;
MethodNameEntry MethodNameTable[MAX_METHOD_ENTRIES];

/*----------------------------------------------------------------------*/

FILE *popen();
char *malloc();
int perror(), exit();
char * sprintf();

void Process();
void PrintHeader();
void PrintTrailer();
void PrintEntry();
void PrintUsesList();


int
main(argc, argv)
char	**argv;
{
	int c;
	int i;
	static char OutputFileName[BUFSIZ]; 	/* Amiga */
	char *p;
	
	extern int	errno;
	extern char	*optarg;
	extern int	optind;

	ProgName = argv[0];

	while (( c = getopt( argc, argv, "co:I:D:C:M:" )) > 0)	/* Amiga */
	{
		switch ( c ) {

		case 'c':
			PutHeader      = false;
			break;

		case 'o':
			PutHeader      = false;
			MakeGlobalFile = true;
			g_fptr = fopen( optarg, "w" );
			if ( !g_fptr ) {
				(void) fprintf( stderr, "%s: cannot open %s", ProgName, optarg );
				(void) perror( "" );
				(void) exit( 1 );
			}
			break;
			
			
		case 'C':
			ClassFileName = optarg;
			p = strrchr( optarg, '/' );
			if ( !p ) p = optarg;
			p = strchr( optarg, '.' );
			(void) strcpy( OutputFileName, optarg );
			if ( !p ) {
				(void) strcat( OutputFileName, ".tex" );
			} else if ( strcmp( p, ".tex" ) == 0 ) {
				*p = '\0';	/* Chop off .tex */
			} else {
				(void) fprintf( stderr, "%s: Illegal extension for \"-C %s\"  -- %s\n", ProgName, optarg, p );
				exit( 1 );
			}

			/* Now that we have a valid file name, see if	*/
			/* a class file exists.  If it does, simply	*/
			/* read its contents.  Otherwise generate it.	*/
			
			if ( ( c_fptr = fopen( OutputFileName, "r" ) ) ) {
				(void) fclose( c_fptr );
				c_fptr = (FILE *)0;
			} else if ( !(c_fptr = fopen( OutputFileName, "w" )) ) {
				(void) fprintf( stderr, "%s: cannot open %s", ProgName, OutputFileName );
				(void) perror( "" );
				(void) exit( 1 );
			}
			ClassFilePresent = true;
			break;
			
		case 'D':
			if ( DefinesCount >= MAX_DIRS ) {
				(void) fprintf( stderr, "%s: Too many include directories, % s ignored\n", ProgName, optarg );
			} else { 
				Defines[DefinesCount++] = optarg;
			}
			break;
			
		case 'I':
			if ( IncludeCount >= MAX_DIRS ) {
				(void) fprintf( stderr, "%s: Too many include directories, % s ignored\n", ProgName, optarg );
			} else { 
				IncludeDir[IncludeCount++] = optarg;
			}
			break;
			
		case 'M':
			MethodFileName = optarg;
			m_fptr = fopen( optarg, "w" );
			if ( !m_fptr ) {
				(void) fprintf( stderr, "%s: cannot open %s", ProgName, optarg );
				(void) perror( "" );
				(void) exit( 1 );
			}
			break;
			
		default:
			(void) fprintf( stderr, "Usage: %s file ...\n", ProgName);
			(void) exit(1);
		}
	}

	/* Some initialization */
	
	for ( i = 0; i < MAX_CLASS_ENTRIES; ++i ) {
		ClassNameTable[i].parent_class_index = -1;
		ClassNameTable[i].tag.defined   = 0;
		ClassNameTable[i].tag.traversed = 0;
		ClassNameTable[i].class_name = (char *)0;
		ClassNameTable[i].next = (BackLink *)0;
	}
	ClassNameTable[0].class_name = "** ROOT **";
	
	for ( i = 0; i < MAX_METHOD_ENTRIES; ++i ) {
		MethodNameTable[i].method_name = (char *)0;
		MethodNameTable[i].next = (BackLink *)0;
	}
	MethodNameTable[0].method_name = "** ROOT **";
	
	/* and away we go */

	if ( optind == argc ) {				/* no file names */
		if ( DefinesCount == 0 ) { 
			i_fptr = stdin;
			o_fptr = stdout;
			Process( i_fptr, o_fptr, "** stdin **" );
		} else {
/* 			i_fptr = popen( cmd, "r" ); */
			/* Run CPP */
		}
		exit ( 0 );
	}

	if ( MakeGlobalFile ) {
		PrintHeader( g_fptr );
		if ( ClassFilePresent ) {
			(void) fprintf( g_fptr, "\\include{%s}\n", ClassFileName );
		}
		(void) fprintf( g_fptr, "\\chapter{Classes}\n" );
	}
	
	for ( ; optind < argc; ++optind ) {
		if ( strcmp(argv[optind], "-") == 0 ) {

			i_fptr = stdin;
			o_fptr = MakeGlobalFile ? g_fptr : stdout;

		} else {

			/* Search for extension on file name	*/
			
			(void) strcpy( OutputFileName, argv[optind] );

			p = strrchr( OutputFileName, '/' );
			if ( !p ) p = OutputFileName;
			p = strchr( OutputFileName, '.' );
			if ( !p ) {
				(void) fprintf( stderr, "%s: Cannot find file type -- %s\n", ProgName, argv[optind] );
				continue;
			} else if ( strcmp( p, ".m" ) == 0 || strcmp( p, ".c" ) == 0 || strcmp( p, ".h" ) == 0 ) {
				if ( MakeGlobalFile ) {
					(void) fprintf( g_fptr, "\\include{%.*s}\n", p-OutputFileName, OutputFileName );
				}
				(void) strcpy( p, ".tex" );
			} else if ( strcmp( p, ".tex" ) == 0 ) {
				if ( MakeGlobalFile ) {
					(void) fprintf( g_fptr, "\\include{%.*s}\n", p-OutputFileName, OutputFileName );
				}
				continue;
			} else {
				(void) fprintf( stderr, "%s: Illegal file type -- %s\n", ProgName, argv[optind] );
				continue;
			}
			
			if ( (i_fptr = fopen( argv[optind], "r" )) == NULL ) {
				(void) fprintf(stderr, "%s: Cannot open input file -- %s ", ProgName, argv[optind] );
				(void) perror( "" );
				exit( 1 );
			}
			
			if ( (o_fptr = fopen( OutputFileName, "w" )) == NULL ) {
				(void) fprintf(stderr, "%s: Cannot open output file -- %s ", ProgName, OutputFileName );
				(void) perror( "" );
				exit( 1 );
			}

		}

		if ( DefinesCount == 0 ) {
			Process( i_fptr, o_fptr, argv[optind] );
		} else {
			/* Run CPP */
		}

		if ( i_fptr != stdin ) {
			(void) fclose( i_fptr );
			(void) fclose( o_fptr );
		}
	}

	if ( c_fptr ) {
		PrintClassHierarchy( c_fptr );
	}
/*	if ( MethodFileName ) {
		PrintMethodIndex( m_fptr );
	} */
	if ( g_fptr ) {
		PrintTrailer( g_fptr );
	}
	
	
	(void) exit( 0 );

	/*NOTREACHED*/
}



void
Process( i_fptr, o_fptr, FileName )
FILE * i_fptr;
FILE * o_fptr;
char * FileName;
{
	if ( PutHeader ) PrintHeader( o_fptr );
	(void) ReadFile( i_fptr, FileName, 0 );
	if ( PutHeader ) PrintTrailer( o_fptr );
}



FILE *
OpenIncludeFile( ClassName, FileName )
char * ClassName;
char * FileName;
{
	register char *p;
	register char *q;
	FILE * fptr;
	register int i;
	
	/* Try local directory first, then try search path	*/

	p = FileName;
	q = ClassName;
	while( *q  ) {
		*p++ = *q++;
	}
	*p++ = '.';
	*p++ = 'h';
	*p++ = '\0';

	fptr = fopen( FileName, "r" );
	for ( i = 0; !fptr && i < IncludeCount; ++i ) {
		p = FileName;
		q = IncludeDir[i];
		while( *q ) {
			*p++ = *q++;
		}
		if ( *(p-1) != '/' ) {
			*p++ = '/';	/* Ignore trailin '/' */
		}
		q = ClassName;
		while( *q ) {
			*p++ = *q++;
		}
		*p++ = '.';
		*p++ = 'h';
		*p++ = '\0';
		fptr = fopen( FileName, "r" );
	}
	return fptr;
}



/*----------------------------------------------------------------------*/

void
PrintHeader( fp )
FILE * fp;
{
	(void) fprintf( fp, "\\documentstyle[11pt]{report}\n" );
	(void) fprintf( fp, "\\newcommand{\\heading}[1]{\n" );
	(void) fprintf( fp, "\t\\addcontentsline{toc}{subsection}{\n" );
	(void) fprintf( fp, "\t\t\\protect\\numberline{}{#1}}{\\begin{center}{\\LARGE\\bf #1}\\end{center}}}\n" );
	(void) fprintf( fp, "\\newcommand{\\subheading}[1]{\n" );
	(void) fprintf( fp, "\t\\addcontentsline{toc}{subsubsection}{\n" );
	(void) fprintf( fp, "\t\t\\protect\\numberline{}{#1}}{\\begin{center}{\\Large\\bf #1}\\end{center}}}\n" );
	(void) fprintf( fp, "\\makeindex\n" );
	(void) fprintf( fp, "\\newlength{\\funkywidth}\n" );
	(void) fprintf( fp, "\\newlength{\\classitemtagwidth}\n" );
	(void) fprintf( fp, "\\begin{document}\n" );
	(void) fprintf( fp, "\\settowidth{\\classitemtagwidth}{\\it Interface File:\\/}\n" );
	(void) fprintf( fp, "\\setlength{\\funkywidth}{\\textwidth}\n" );
	(void) fprintf( fp, "\\addtolength{\\funkywidth}{-1\\leftmargini}\n" );
}


void
PrintTrailer( fp )
FILE * fp;
{
	(void) fprintf( fp, "\\end{document}\n" );
}



void
PrintClassHeader( ClassIndex, SourceFile, CommentPtr, IncludePtr )
int ClassIndex;
char *SourceFile;
CommentBuf * CommentPtr;
IncludeBuf * IncludePtr;
{
	void PrintInheritsList();
	char *ArgList[1];
	int ArgCount		= 1;
	long FileTime;
	char InterfaceFile[BUFSIZ];
	FILE * temp_fptr;
	extern long time();
	extern char * ctime();
	
#ifndef AZTEC_C
	{
		struct stat statbuf;

		if ( stat( SourceFile, &statbuf ) == 0 ) {
			FileTime = (long)statbuf.st_mtime;
		} else {
			FileTime = time((long *)0);
		}
	}
#else
		FileTime = time((long *)0);
#endif

	if ( ClassIndex != 0 ) {
		ArgList[0] = ClassNameTable[ClassIndex].class_name;
	} else {
		ArgList[0] = "";
	}
	
	(void) fprintf( o_fptr, "\\newpage" );
	(void) fprintf( o_fptr, "\\begin{center}\\rule{\\textwidth}{0.5mm}\\end{center}" );
	(void) fprintf( o_fptr, "\\pagestyle{myheadings}\n" );
	(void) fprintf( o_fptr, "\\markright{ %s }\n", ArgList[0] );
	(void) fprintf( o_fptr, "\\addcontentsline{toc}{section}{\\protect\\numberline{}{%s}}\n", ArgList[0] );
	(void) fprintf( o_fptr, "\\begin{center} {\\Huge\\bf %s } \\end{center}\n", ArgList[0] );
	(void) fprintf( o_fptr, "\\index{%s}\n", ArgList[0] );
	(void) fprintf( o_fptr, "\\begin{description}\n" );
	(void) fprintf( o_fptr, "\\item[\\parbox{\\classitemtagwidth}{\\it Inherits from:\\/}]\n" );
	if ( ClassNameTable[ClassIndex].parent_class_index != 0 ) { 
		PrintInheritsList( ClassNameTable[ClassIndex].parent_class_index );
	} else {
		(void) fprintf( o_fptr, "nobody\n" );
	}
	(void) fprintf( o_fptr, "\n" );
	(void) fprintf( o_fptr, "\\item[\\parbox{\\classitemtagwidth}{\\it Classes used:\\/}] \n" );
	if ( IncludePtr && IncludePtr->buf ) {
		PrintUsesList( IncludePtr );
	} else {
		(void) fprintf( o_fptr, "none\n" );
	}
	(void) fprintf( o_fptr, "\\item[\\parbox{\\classitemtagwidth}{\\it Interface File:\\/}] \n" );
	if ( strlen( ArgList[0] ) != 0 && (temp_fptr = OpenIncludeFile( ArgList[0], InterfaceFile )) ) {
		(void) fprintf( o_fptr, "%s\n", InterfaceFile );
		(void) fclose( temp_fptr );
	} else {
		(void) fprintf( o_fptr, "none\n" );
	}
	(void) fprintf( o_fptr, "\\item[\\parbox{\\classitemtagwidth}{\\it Source File:\\/}] %s\n", SourceFile );
	(void) fprintf( o_fptr, "\\item[\\parbox{\\classitemtagwidth}{\\it Date:\\/}] %s\n", ctime( &FileTime ) );
	(void) fprintf( o_fptr, "\\end{description}\n" );
	(void) fprintf( o_fptr, "\\begin{center}\\rule{\\textwidth}{0.5mm}\\end{center}\n" );
	if ( CommentPtr ) {
		(void) fprintf( o_fptr, "\\begin{center}{\\LARGE\\bf Introduction}\\end{center}\n" );
		
		PrintComment( CommentPtr, ArgList, ArgCount );
	}
}



static void
PrintInheritsList( i )
register int i;
{
	while ( i != 0 ) {
		(void) fprintf( o_fptr, ClassNameTable[i].class_name );
		i = ClassNameTable[i].parent_class_index;
		if ( i != 0 ) {
			(void) fprintf( o_fptr, ", " );
		}
	}
	fputc( '\n', o_fptr );
}


static void
PrintUsesList( bufp )
IncludeBuf * bufp;
{
	while ( bufp != 0 && bufp->buf ) {
		(void) fprintf( o_fptr, "%s", bufp->buf );
		bufp = bufp->next;
		if ( bufp != 0 && bufp->buf ) {
			(void) fprintf( o_fptr, ", " );
		}
	}
	fputc( '\n', o_fptr );
}


void
PrintMethod( MethodName, ArgList, ArgCount, CommentPtr )
char * MethodName;
char * ArgList[];
int ArgCount;
CommentBuf * CommentPtr;
{
	register char *p;
	register enum { roman, bold, italic } font;
	
	font = roman;
	
	(void) fprintf( o_fptr, "\\begin{description}\n\\item[" );

	for ( p = MethodName; *p; ++p ) {
		if ( (*p & 0x80) && font != bold ) {
			if ( font == italic ) (void) fprintf( o_fptr, "\\/}" );
			(void) fprintf( o_fptr, "{\\bf " );
			font = bold;
		} else if ( !(*p & 0x80) && font != italic ) {
			if ( font == bold ) (void) fprintf( o_fptr, "}" );
			(void) fprintf( o_fptr, "{\\it " );
			font = italic;
		}
		PutLatexChar( *p, o_fptr );
	}

	if ( font != roman ) (void) fprintf( o_fptr, "}" );
	(void) fprintf( o_fptr, "\\index{%s}", ArgList[0] );

	/* Now print the comment.	*/
	
	(void) fprintf( o_fptr, "]\n\\mbox{}\n\n" );
	PrintComment( CommentPtr, ArgList, ArgCount );
       	(void) fprintf( o_fptr, "\\end{description}\n" );
}



void
PrintFunction( FunctionName, ArgList, ArgCount, CommentPtr )
char * FunctionName;
char * ArgList[];
int ArgCount;
CommentBuf * CommentPtr;
{
	register char *p;
	register enum { roman, bold, italic } font;
	
	font = roman;
	
	(void) fprintf( o_fptr, "\\begin{description}\n\\item[" );

	for ( p = FunctionName; *p; ++p ) {
		if ( (*p & 0x80) && font != bold ) {
			if ( font == italic ) (void) fprintf( o_fptr, "\\/}" );
			(void) fprintf( o_fptr, "{\\bf " );
			font = bold;
		} else if ( !(*p & 0x80) && font != italic ) {
			if ( font == bold ) (void) fprintf( o_fptr, "}" );
			(void) fprintf( o_fptr, "{\\it " );
			font = italic;
		}
		PutLatexChar( *p, o_fptr );
		if ( (*p & 0x7f) == ')' ) {
			p++;
			break;
		}
	}

	if ( font != roman ) (void) fprintf( o_fptr, "}" );
	(void) fprintf( o_fptr, "\\index{%s}", ArgList[0] );
	(void) fprintf( o_fptr, "]\n\\mbox{}\n\n\\parbox{\\funkywidth}{\\it " );
	for( ; *p; ++p ) {
		PutLatexChar( *p, o_fptr );
		if ( (*p & 0x7f) == ';' ) (void) fprintf( o_fptr, "\\\\\n" );
	}
	(void) fprintf( o_fptr, "}\n\n" );

	/* Now print the comment.	*/
	
	PrintComment( CommentPtr, ArgList, ArgCount );
       	(void) fprintf( o_fptr, "\\end{description}\n" );
}



/*
 * Print Comment
 * 	print out a comment.  Be picky about page breaks.
 */

void
PrintComment( CommentPtr, ArgList, ArgCount )
CommentBuf * CommentPtr;
char *ArgList[];
int ArgCount;
{
	register char *p;
	register int state	 = 0;
	register int n;
	int i;
	register char * q;
	
	(void) fprintf( o_fptr, "{\\samepage " );
	
	while( CommentPtr ) {
		for ( p = CommentPtr->buf, n = CommentPtr->n; n > 0; ++p, --n ) {

			switch( state ) {
			case 0:
				if ( *p == '$' ) {
					state = 2;
				} else if ( !isspace( *p ) ) {
					putc( *p, o_fptr );
					state = 1;
				}
				break;

			case 1:
				if ( *p == '$' ) {
					state = 2;
				} else {
					putc( *p, o_fptr );
				}
				break;

			case 2:
				if ( '0' <= *p && *p <= '9' ) {
					i = *p - '0';
					if ( i < ArgCount ) {
						(void) fprintf( o_fptr, "{%s ", (i == 0) ? "\\bf" : "\\it" );
						for ( q = ArgList[i]; q && *q && *q != ',' && *q != ';' && !isspace( *q ); ++q ) {
							PutLatexChar( *q, o_fptr );
						}
						(void) fprintf( o_fptr, "%s}", ( i == 0 ) ? "" : "\\/" );
					}
				} else {
					putc( *p, o_fptr );
				}
				state = 1;
				break;
			}
		}
		CommentPtr = CommentPtr->next;
	}
	(void) fprintf( o_fptr, "}\n" );
		
}


/*
 * PutLatexChar
 *	Print out a character after attending to the fact that
 *	latex does horrible things witch certain characters.
 */


void
PutLatexChar( c, fp )
register char c;
FILE * fp;
{
	c &= 0x7f;
	
	switch ( c ) {
	case '#':
	case '$':
	case '%':
	case '&':
	case '~':
	case '_':
	case '^':
	case '\\':
	case '{':
	case '}':
		putc( '\\', fp );
		break;
	}
	putc( c, fp );

#ifdef	FOOBAR
	/* Help latex with line breaking. */

	switch( c ) {
	case '_':
	case ':':
		putc( '\\', fp );
		putc( '-', fp );
		break;
	}
#endif	FOOBAR

}
