//   This is a little test  :bk=0   (((  t7.m  )))
//	BongoDude belongs to Disk Fong (used without permission :-)

#import "t7.h"


@implementation  BongoDude


+ new
{
    id    this = [super new];

    return [this initialize];
}


- initialize
{

    first  = [Object new];
    second = [Object new];
    last   = [Object new];
    
    xxx    = 1;
    yyy    = 'Y' ;
    zzz    = 'Z' ;
    aaa    = 'A' ;

    return self;
}

- fill
{

    second = [[self class] new];
    return self;
}


- cleanse
{

    xxx = 0;
    first = [first free];
    second = [second free];
    last = [last free];

    return self;
}

@end


