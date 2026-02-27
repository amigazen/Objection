/*   HW.M   Hello World Example                            Aug 89, bjw */


#include <stdio.h>
#include <apps/NewByte.h>
#include <apps/tree.h>
@includes

#define  STREQ(s,t) 	(!strcmp(s,t))


main( argc, argv )
	int 	argc;
	char	*argv[];
{
	id  	msg;

	[NewByte show];

	msg = [NewByte str: "!dlroW olleH"];
	printf( "%s\n", [msg str] );

	[msg reverse];
	printf( "%s\n", [msg str] );

	/*  CmdLine args creates a Tree: */
	if( argc == 2 )
	{
		extern BOOL 	_msgTrace;

		_msgTrace = 1;
		printf( "\nTree with %s\n", [[[Tree new] addNode: argv[1] ] str] );
	}

	[NewByte show];
	[msg free];
}	/* main */

