//   This is a little test  :bk=0   (((  T3.M  )))

#import <apps/t3.h>
#import <objc/vectors.h>


typedef struct { short x, y; } VECT;


typedef struct MessageContext {
	char	* ExecMsg;
	VECT TSpot;
	struct View *TView;
	APTR TOverhead;
} MSGCTX ;

@implementation  Stupid : Collection;

#if 1
int  	testMe;
#endif   // test_test xxx xx xx

#iiii


extern int (*funct_ptr)();
extern char 	_ctype[];

struct NewWindow	{ short  uu; long  ll; } ;

- next
{
	struct Window	*w = (struct Window *) _ctype ;
	return self;
}

- doit: (MSGCTX *) mc 
{
	return self ;
}


- class
{
	id	*there, sequence;

	arena_size = NewWindow( collect );
	[[sequence findSTR:"closeInputFile"] isMarked];
	return [ there[5] show ];
}



/*  full comment */  
+ at:(int)garbage put:anObject
{
	char	*pic, c = ' ' ;

	if( 0 )
		[self error:"Type mismatch" ];

	[super new]; 
	switch( (_ctype+1) [(long)anObject] )		/* Contributed by jk */
	{
	}

	if( ((_ctype+1)[c]&010) )
	{
		if( strncmp( "a", pic = [anObject describe], 1) == 0 )
			if( strcmp( pic = [anObject describe], "a" ) == 0 )
				printf( "%s -> %d\n", [anObject describe], [anObject hash] );
	}

	[self charAt:garbage put:[anObject class] ];
	[self at:[self hash] + [self hash] + 5 ];
	[self at:[self hash] / - (*funct_ptr)(garbage) ];

}

- at: (int) anOff  put: anObject_like
{
	if( arena_size > 0 )
		[self isKindOf: (*oc_cvtToId) ("Object") ];
	return self;
}


- (short) at:(int) anOffset
{
	int  a,b,size;

	a>0?b:a;

	[[Sequence new] perform:(SEL)((69+anOffset)) with:self];
	return( (short) (IV(self)[ anOffset ]) );
}

@end


