//    t7.h   Actually tested class!                       28-feb-91, bjw
//	BongoDude belongs to Disk Fong (used without permission :-)

#import <objc/Object.h>


@interface BongoDude : Object
{
     short   xxx;
     id      first;
     char    yyy;
     id      second;
     char    zzz;
     char    aaa;
     id      last;
}

+ new ;
- initialize ;
- fill ;
- cleanse ;

@end

