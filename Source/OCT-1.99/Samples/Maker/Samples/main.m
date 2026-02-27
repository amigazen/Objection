/*   MAIN.M     Main Driver for Dependency graph Example.     July 89 bjw */

/*
 *    The input language accepted looks kinda like a Makefile without
 *    any build rules.  Ie, the input is target followed by dependencies.
 *    
 *        #  This is a comment
 *        project: output
 *        output: main
 *        main: file1 file2.c
 *        file2: header
 *        file3: Makefile
 *    
 *    July-89      bjw   Begin implementation from Dr. Cox's book.
 *    25-april-91  bjw   More comments. Works with NeXT-like classes!
 *    19-sept-93   bjw   Removed #import directives.
 */

#include  <stdio.h>
#include  "Node.h"
#include  "Graph.h"
#include  <collect/Sequence.h>
@includes()


/* ------------------------  Master Defines  ------------------------- */


/*  TRUE to display some progress status information:  */
#define  SW_WATCH  0

#define  STREQ(s,t) 	((s)[0] == (t)[0] && !strcmp(s,t))


/* ----------------------  Global Varsiables  ------------------------ */

char       *progName;

id			graph, lastNode;

char		root[ 96 ]; 		// String name of root node

char		yytext[ BUFSIZ ];		// Lexical buffer
FILE    	*yyin;


/* -----------------------  Graph Driver  ---------------------------- */


/*   demoGraph  --  Build a demonstration graph with "root" on the top.
 *
 *    	main: functionA functionB
 *   	functionA:  rtl-A
 *   	functionB:  rtl-B
 *   	other-func:
 *   	rtl-A:  mylibrary
 *   	rtl-B:  mylibrary
 */
	void
demoGraph()
{
	id  	node;
	short	count;

	//	Build a graph just for fun!
	strcpy( root, "main" );
	graph = [Graph new];

	//	Fill in the graph:
	node = [graph defineNode:root];
	[node addReference:[graph addNode:"functionA"]];
	[node addReference:[graph addNode:"functionB"]];

	node = [graph defineNode:"functionA"];
	[node addReference:[graph addNode:"rtl-A"]];

	node = [graph defineNode:"other-func" ];

	node = [graph defineNode:"functionB"];
	[node addReference:[graph addNode:"rtl-B"]];

	node = [graph defineNode:"rtl-A"];
	[node addReference:[graph addNode:"mylibrary"]];
	node = [graph defineNode:"rtl-B"];
	[node addReference:[graph addNode:"mylibrary"]];

	//	Mark nodes reachable from 'root' node:
	[graph mark:root];

}	/* demoGraph */


	void
doGraph()
{
	id  	node;

	//	Build the graph by reading the input file
	graph = [Graph new];
	yyparse();

	printf( "** File Now Read **\n" );
#if SW_WATCH
	[[graph findSTR:"closeInputFile"] show];
#endif

	// -------------------------------------
	//	Mark all nodes reachable from root
	[[graph addNode:root] mark];
}	/* doGraph */


printGraph ()
{
	register short	count;
	id  	sequence, node;

	//	Print reachable nodes
	sequence = [graph eachElement];
	printf( "Reachable from %s:", root );
	count = -1;
	while( (node=[sequence next]) != NULL )
	{
		if( [node isMarked] )
		{
			if( (++count % 4) == 0 )
				putchar( '\n' );
			printf( "  %s", [node str] );
		}
	}
	printf( "\n" );

	//	Print unreachable nodes:
	[sequence toFirst];
	printf( "Not reachable from %s:", root );
	count = -1;
	while( (node=[sequence next]) != NULL )
	{
		if( ! [node isMarked] )
		{
			if( (++count % 4) == 0 )
				putchar( '\n' );
			printf( "  %s", [node str] );
		}
	}
	printf( "\n" );

	//	Print undefined nodes:
	[sequence toFirst];
	printf( "Undefined nodes:" );
	count = -1;
	while( (node=[sequence next]) != NULL )
	{
		if( ! [node isDefined] )
		{
			if( (++count % 4) == 0 )
				putchar( '\n' );
			printf( "  %s", [node str] );
		}
	}
	printf( "\n-------\n" );

	[sequence free];
	[graph freeContents];
	[graph free];
}	/* printGraph */


/* ------------------------  Lexer / Parser  ---------------------------- */

#include <ctype.h>

#define  DefineAction(text)	lastNode = [graph defineNode:text]
#define  ReferAction(text)  [lastNode addReference:[graph addNode:text]]

#define  WRD	1
#define  EOL	2


	void
yyerror( str )
	char	*str;
{
	fprintf( stderr, "?%s: yyerror: %s!\n",  progName, str );
	exit( 10 );
}


	int
yylex()
{
	register char	*p;
	register int 	c;

	while( (c=getc(yyin)) != EOF )
	{
	    while( c == ':' )		/*  Ignore colons.. */
		    c = getc(yyin);

		if( c == '#' )			/* Eat comment until EOL */
		{
			while( (c=getc(yyin)) != '\n' && c != EOF )
				;
			if( c == EOF )
				break ;
		}

		if( c == '\n' )
			return EOL;
		else
		if( isspace(c) )
		{
			while( isspace(c=getc(yyin)) )
				;
			ungetc( c, yyin );
		} else
		{
			p = yytext;
			do
			{
				*p++ = c;
			}
			while( !isspace(c=getc(yyin)) ) ;;
			*p = 0;

			ungetc( c, yyin );
			return WRD;
		}
	}

	return 0;
}	/* yylex */



	int
yyparse()
{
	register short	c;

	while( (c=yylex()) != 0 )
	{
		switch( c )
		{
			case WRD :
#if SW_WATCH
					printf( "graph from <%s>\n", yytext );
#endif
					if( root[0] == 0 )
						strcpy( root, yytext );
					DefineAction( yytext );      /* Reference a node */
					while( (c=yylex()) != EOL )
					{
						if( c != WRD )
							yyerror( "bad symbol for reference" );

						ReferAction( yytext );

					}	/* until EOL, define each symbol on the line */
					break ;

			case EOL :
					break ;

			default :
					yyerror( "bad symbol on begin of line" );

		}	/* switch */

	}	/* for each line in file.. */

	return 0;
}	/* yyparse */


/* ------------------------  Main Driver  ---------------------------- */


main( argc, argv )
	int 	argc;
	char	*argv[];
{
    extern int   _msgTrace;

    progName = argv[0];
	fprintf( stderr, "(Dependency graph example)\n" );
	fprintf( stderr, "(From OOP book by Dr. Cox, 1986, Ch. 6)\n" );

	if( argc != 2 )
	{
		fprintf( stderr, "?%s: usage: %s <dependency_file>\n",
		                  progName, argv[0] );
		fprintf( stderr, "  But here's a demo!\n" );
		demoGraph();
		printGraph();
	} else
	{
		if( (yyin = fopen( argv[1], "r" )) == NULL )
		{
			printf( "?%s: can't read <%s>\n", progName, argv[1] );
			return( 10 );
		}

		doGraph();
		printGraph();
	}

    _msgTrace = 1;        // Request messaging stats
}	/* main */

