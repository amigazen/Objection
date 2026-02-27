/*   OBJC-CLASS.H    OCT Class definition header            bjw, Sept 90 */

/*
 *    This header is for NeXT compatability.  Including this header
 *    will provide the caller with access to the class definitions.
 *    Requires <objc/objc.h> for definition of Method typedef.
 *   
 *    15-sept-90	bjw 	Initial coding.
 *    5-june-91   bjw    Added Cache structures.
 */

#include  <objc/objc.h>

#ifndef _OCT_CLASS_H
# define  _OCT_CLASS_H   1


extern void    class_addClassMethods( );
extern void    class_addInstanceMethods( );


/* Definitions of filer types */

#define  _C_ID  		'@'
#define  _C_CLASS  	    '#'
#define  _C_CHR  		'c'
#define  _C_UCHR		'C'
#define  _C_SHT  		's'
#define  _C_USHT		'S'
#define  _C_INT  		'i'
#define  _C_UINT		'I'
#define  _C_LNG  		'l'
#define  _C_ULNG		'L'
#define  _C_FLT  		'f'
#define  _C_DBL  		'd'
#define  _C_BFLD		'b'
#define  _C_VOID		'v'
#define  _C_UNDEF  	    '?'
#define  _C_PTR  		'^'
#define  _C_CHARPTR  	'*'
#define  _C_ARY_B  	    '['
#define  _C_ARY_E    	']'
#define  _C_UNION_B     '('
#define  _C_UNION_E  	')'
#define  _C_STRUCT_B	'{'
#define  _C_STRUCT_E	'}'

#define  _C_BOOL        'o'  	/* an On-Off bit */
#define  _C_HERESTR     'a'     /* an embedded string */
#define  _C_VECT        'v'     /* <short><short> */
#define  _C_RECT        'r'     /* <short><short> <short><short> */
#define  _C_IOD         'x'     /*  FILE *  */

#define  _C_eof         '|'     /* Don't decode any more.. :-( */


/* ------------------------------------------------------------------ */
/*  Filled at runtime caches.  Possibly resized at runtime.           */
/*  Cache's mask is always 2^n - 1 in value sothat it can be used     */
/*    as a bit-mask as it is.  'occupied' tells how many slots are    */
/*    actually in use.  When this approaches 0.5 * mask, it's time    */
/*    to re-allocate a larger cache structure.                        */


struct obj_cache
{
	unsigned int    mask;     /* total buckets = mask + 1 */
	unsigned short  occupied;
	Method          buckets[ 1 ];     /*  extend as needed.. */
} ;
typedef struct obj_cache   *Cache;


#endif

/*  end of objc-class.h */

