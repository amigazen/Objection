/*    OBJC.H    Object Oriented Defines and Such          Mar 89, bjw */
/*****************************************************************
 *   This program is copyright (c) 1989, 1990, 1993 by Brian J. Witt.
 *                  netcom.com!gentoo.com!rael!aleks
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responibility for usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage.  A small fee ( < $25 US) may be charged to cover
 *   distribution costs.
 *
 ******************************************************************/

/*
//		This file handles two define'd symbols:  MAIN and SUBSTRATE.
//		MAIN is used by "methods.h" to define the strings relating to
//		each method the system knows about.  SUBSTRATE creates a phony
//		definition of what an object is, and it also supresses the
//		re-definition of main() function.
//
//		The application must include this file.  We convert its main()
//		and exit() calls to use the OCT substrate routines.
//
//		The selector codes are 32 bits.  This way, the same code and
//		data structures may be used universally (ie, throughout a system),
//		without regard to how the programs were compiled.
//
//		15-mar-89	bjw		Initial coding.
//		5-sept-89	bjw 	Version 1.50.
//		14-sept-89	bjw 	Added indirect selector support.
//		29-sept-89	bjw 	Connect _SHARED to class_cb (again).
//		30-sept-89	bjw 	calls app_main() and app_exit().
//		5-oct-89	bjw 	Selectors are 32bits wide.
//		3-nov-89	bjw 	Loosend '_PRIVATE' dependency (removed it).
//		03-mar-90	bjw 	Added class delegates.
//		28-mar-90	bjw 	Added allowances for Xenix '286 systems.
//		12-apr-90	bjw 	Removed MCH_MEM_LINEAR -- too much hassle.
//     	15-sept-90	bjw 	Added some missing NeXT structures.
//      27-sept-90  bjw     "_SHARED" is now "objc_class" and "Class".
*/


/*  Define this for Xenix '286 release 2.3 systems (circa Summer 1987): */
/*  #define  XENIX_OLD   1  */


#ifndef _OBJC_INCL
# define  _OBJC_INCL	1

# ifndef _OOC_PASS2
#  include <octhead.h>
#  include <stdio.h>
# endif


# ifndef NULL
#  define  NULL  ((void *)0)
# endif


# ifdef MPU68000
#  define  m68000	1
# endif
# ifdef MPU8086
#  define  m8086	1
# endif


/*  Our printf() does have %p(pointer) format: */
# ifndef ANSI_PRINTF
#  define  ANSI_PRINTF	1
# endif


/*v ======================================= v*/
/*v                                         v*/
/*v  Machine/Compiler Dependencies Section  v*/

/*  Convert ptr to something integer-like:  */
# define  PTR2INT(ptr)	((unsigned long)(ptr))

/*  Mask the hash value to something nice to integers: */
# define  OC_HASH_MASK   0x7FFF


# ifdef AZTEC_C
 /*  MANX's AZTEC C  */
 /*  Install macros to convert your compiler's memory functions
  *	to ANSI conforming calls.  These are for MANX:
  */
#  define  memcpy(dest,src,cnt)	movmem(src,dest,cnt)
#  define  memset(dest,ch,cnt)	setmem(dest,cnt,ch)

 /*  Our printf() doesn't have %p(pointer) format: */
#  undef  ANSI_PRINTF

 /*  Force no prototyping for simple Aztec compiler: */
#  undef  NO_PROTOTYPE
#  define  NO_PROTOTYPE=1

# endif

/** ======================================= **/

# ifdef XENIX_OLD

 /*  Our printf() doesn't have %p(pointer) format: */
#  undef  ANSI_PRINTF
#  undef  NO_PROTOTYPE
#  define  NO_PROTOTYPE=1

#  define  void 	int

# endif

/*^  Machine/Compiler Dependencies Section  ^*/
/*^                                         ^*/
/*^ ======================================= ^*/



/* ---------------------  PreProcessor Directives  --------------------- */


# ifndef NO_PROTOTYPE
#  define  PARMS(x)   x
#  define  CONST 	const
# else
#  define  PARMS(x)   ()
#  define  CONST
# endif


/*  Control compilation of floating point calls: */
# ifndef  OOC_FLOAT
#  define  OOC_FLOAT  0
# endif


# define  METHOD_IS	static
# ifndef FORWARD
#  define  FORWARD  extern
# endif


/*  Change these as appropriate for your compiler: */
/*  These specify storage class for 'self' variable: */
# ifndef REG_FACT_SELF
#  define  REG_FACT_SELF
# endif
# ifndef REG_INST_SELF
#  define  REG_INST_SELF   register
# endif


/*  Convert applications's main() to another for transparent compiling: */
# ifndef  SUBSTRATE
#  define  main   app_main
#  define  exit   app_exit
# endif


/*  A Boolean TRUE type: */
#define  YES	((BOOL)1)
#define  NO 	((BOOL)0)


/*  Used as the id of the Nil class: */
#define  Nil	((Class)0)

/*  the one and only instance of the Nil class: */
#define  nil	((id)0)



/* ----------------------  Object Class Types  ------------------------- */

# ifndef _OOC_PASS2
#  ifdef SUBSTRATE
typedef struct objc_class		*id;
#   else
typedef struct _PRIVATE     *id;
#  endif

/****  Defined in <octhead.h>  ****/
#  ifndef _OCTHEAD_H
typedef short        BOOL; 
#  else
#   undef  main
#   undef  exit
#  endif

typedef char    	*STR;   		/* Generic ASCII string pointer */
typedef	ULONG   	SEL;    		/* selector code value  */
typedef id      	(*IMP) ( /* id, ... */ );
typedef FILE    	*IOD;   		/* Access a file stream */


typedef struct objc_class  *Class;

typedef struct objc_object  {
        Class  isa;
} ;

# endif



/* ---------------------- */
/*  COMMON PASS GOODIES   */
/*                        */


/*  Flags used by objc_class.info field: */
#define  CLS_CLASS       	0x0001  /* Factory or Instance class struct */
#define  CLS_META       	0x0002  /*  ???  */
#define  CLS_INITIALIZED	0x0004  /* Already been init'ed */
#define  CLS_POSING      	0x0008  /* Part of category */
#define  CLS_MAPPED     	0x0010	/* Dictionary been bound? */

#define  CLS_STATIC     	0x0100
#define  CLS_PUBLIC     	0x8000 	/* Can object be exported? */

/*  To access these bits:  */
#define  CLS_GETINFO(cls,infomask)   ((cls)->info & (infomask))
#define  CLS_SETINFO(cls,infomask)   ((cls)->info |= (infomask))

/*  Setting used for objc_class of factory and instance vars: */
#define  CLS_SETF_SHARED 	 ( CLS_STATIC | CLS_CLASS )


# ifndef _OOC_PASS2

/* --------------------- */
/*  FIRST PASS GOODIES   */
/*                       */


struct objc_method
{
	SEL   	    	 md_Name;			/* Unique Id value */
	CONST IMP    	 md_Proc;			/* function (code) to do it */
	CONST STR     	 md_String; 		/* standard ASCII-1968 name */
	CONST UWORD      md_Ident;			/* Index in .da_methods[] */
} ;
typedef struct objc_method  *Method;


struct objc_method_list
{
	struct objc_method_list  *da_next;
	CONST WORD    	      da_count;  	/* How many here */
	CONST SEL 		     *da_codes;  	/* selectors used by class */
	struct objc_method 	 *da_methods;  	/*  ptr to array.. (never NULL) */
} ;



/*
//     'memory_version' inc'ed each time poseAs: or sub-cateogoried.
//     'clsCount' is sum of +(new) methods minus -(free) methods for class.
*/
struct  objc_class
{
	CONST Class       isa;    		/* Who we belong to */
	CONST Class		super_class;	/* Who we inheritent from */
	char			*name;  		/* Text name of class */

	UBYTE    		version;    	/* Release version of class */
	UBYTE    		memory_version;	/* Changes while in memory */
	UWORD			info;       	/* System maintaince */
	WORD     		clsCount;		/* + new / - free */
	CONST UWORD		clsSizInstance;	/* sizeof(encode of 'clsTypes') */

	CONST char		*clsTypes;		/* For activate/passivate */

	struct objc_method_list  *clsSelects;	/* Ptr to "shared's" methods */
	struct obj_cache  *clsCache; 	/* Local hash table */
	struct class_cb  *clsControl;	/* Link to class's control block */

	struct objc_class	*clsCatList;	/* Thread for next category */
	IMP     		clsDelegate;	/*  Can intercept all messaging */
} ;



/* -----------  Indirect Selector Support  ------------------ */


/*  This structure used as backing for runtime substrate bind routine. */
struct class_cb
{
	struct objc_method_list   *fact_sels;   /* Original factory methods */
	struct objc_method_list   *inst_sels;   /* Original instance methods */
	struct objc_class  	   *instance;       /* the instance for Class */
	struct objc_class   **depends;          /* classes we depend on */
	CONST char    		*category_class;    /* Category we're part of */

	CONST struct objc_method_list *ref_methods; /* (not class) methods used */
	CONST UWORD  		total_selectors;	/* elements count (SEL array) */
} ;


/*  user level calls:  (ANSI prototypes for substrate compatibility) */

extern  SEL      sel_getUid(const char *colon_name);
extern  STR      sel_getName(SEL sel_id);
extern  BOOL     sel_isMapped(SEL sel_id);

extern  STR      object_getClassName(id someObj);

# else

/* --------------------- */
/*  SECOND PASS GOODIES  */
/*                       */

/*  Return pointer to first byte of indexed instance variable space: */
#  define  IV(obj)  (((STR)(obj))+(obj)->isa->clsSizInstance)

extern struct objc_class	factoryObject;		/* Anchor pad */

# endif


/* ----------------------  General Messaging  ------------------------- */


# ifdef SUBSTRATE
extern struct objc_class	factoryObject;		/* Anchor pad */
# endif

extern void 	*_msg(void *self, SEL sel, ...);
extern void 	*_msgSuper(void *superClass, void *self, SEL sel, ...);

# define  ISSELECTOR(u)     sel_isMapped(u)
# define  SELNAME(u)        sel_getName(u)
# define  SELUID(str)       sel_getUid(str)
# define  NAMEOF(u)         object_getClassName(u)


#endif

/* end of objc.h */

