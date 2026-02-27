/*    USER.M     The User's portion of OCT                 April 89, bjw */

/*
This file tests the Collection phylum of classes.  These
include Set, OrderedCollection and Sequence.

*/


#include <objc/ByteArray.h>

#include <collect/Collection.h>
#include <collect/Sequence.h>
#include <collect/OrdCltn.h>
#include <collect/Set.h>
@includes()



#define  SW_OLD   1


    void
sequenceTest( collect )
    id  	collect;
{
    id  	here, seq, other;
    short 	j;

    printf( "\n- - -  Sequence  - - -\n" );
    seq = [ Sequence array: collect ];
    for( j=0 ; (here = [seq next]) != nil ; ++j )
    {
        printf( " Seq[%d] = ", j );
        [here show];
    }
    printf( "\n" );

    [seq toFirst];
    other = [seq asSet];
    printf( "%s size=%d\n", [[other class] str], [other count] );
    [other free];

    [seq free];
}	/* sequenceTest */


    void
list_test( collect )
    id    collect;
{
    register id  	list = [OrderedCollection new];
    id  	here, target;
	int     cnt;

    /***********************************/
    /**  Test the Ordered Collection  **/
    printf( "\n- - -  Ordered Collection  - - -\n" );

    [list add: [ByteArray str:"00000" ] ];
    [list add: [ByteArray str:"1"     ] ];
    [list add: [ByteArray str:"two"   ] ];
    [list add: [ByteArray str:"drei"  ] ];
    [list add: (target=[ByteArray str:"_zap__"]) ]; 
    [list add: [ByteArray str:"lastone"] ];

    for( cnt=0 ; (here=[list at:cnt]) != nil ; ++cnt )
    {
        [here show];
    }

    [[list remove:[list at:0]] show];     //  Remove first!
    [[list remove:target]      show];     //  Remove target.

    [list freeContents];
    [list free];

}	/* list_test */


/* -----------------------  Main Line Drivers  ------------------------- */


/*   Alert user we did the cleanup stuff.  */
    void
finish()
{
    printf( "..that's all folks..!\n" );
}   /* finish */


    void
main( argc, argv )
    int 	argc;
    char	*argv[];
{
    extern BOOL 	_msgTrace;
    register id	it;
    id  	other;
    short	j;
    int 	size;

    /*  Register a cleanup routine: */
    _oc_atExit( finish );

    it = [OrderedCollection new ];
#if ANSI_PRINTF
    printf( "got it: $%p, of %s class.\n", it, [it name] );
#else
    printf( "got it: $%lx, of %s class\n", (ULONG)it, [it name] );
#endif

    [IdArray show];

    [it add: [Object new]];
    [it add: [IdArray with:1, [it at:0]] ];
    [it add: [ByteArray str:"Hello world"] ];
    [it add: [Set  new:2] ];

#if SW_OLD
    size = [it count];
    for( j=0 ; j < size ; ++j )
    {
        [ [it at:j] show ];
    }

    printf( "array size=%d, cap=%d\n", [it count], [it capacity] );

    /***********************/
    /**  Test relations:  **/
    printf( "found nil at %d\n", [it offsetOf: nil] );
#endif

    other = [it copy];
    printf( "Q-CopyOf = %d (should be 1)\n", [other isCopyOf: it] );
    [other free];
    other = nil;

    if( ! [it isKindOf: IdArray] )
    {
        printf( "'it' ain't an IdArray decendent.\n" );
    }
    sequenceTest( it );
    list_test( it );

    [it freeContents];
    [it free];
    it = nil;

    /**  _msgTrace = -1;  **/
}	/* user_main */

