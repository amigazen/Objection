/*    ERROR.H   Error messages and codes                     Sept 89, bjw */

/*  This file contains no user tuneable parameters.  */


enum  GERR_CODES
{
		GERR_NO_INTERFACE = 1,       /* No interface for class */
		GERR_VARIABLE,
		GERR_DEF_METHOD,
		GERR_DUP_CLASS,
		GERR_EXPRESSION,
		GERR_SYNTAX,
		GERR_SHORT_MESSAGE,
		GERR_LONG_MESSAGE,
		GERR_VARTYPE,
		GERR_MISSING_VAR,
		GERR_CLOSE_PAREN,
		GERR_MISMATCH,
		GERR_INTERNAL,				/* Internal error! [13] */
		GERR_METHOD_DECLARE,		/* @if method declare bad */
		GERR_CONTEXT_FAIL,			/* underflowed in popping */
		GERR_NO_CORE,
		GERR_BAD_DEFS,				/* preproc: @defs class unknown %s */
		GERR_SUDDEN_EOF,			/* unexpected EOF */
		GERR_CLASS_LATE,			/* @interface after @includes */
		GERR_CATEGORY_SYNTAX,		/* Class category used wrongly */
		GERR_UNKNOWN_SYMBOL,		/* Undefined symbol encountered: %s */
		GERR_DIRECTIVE_CONTEXT,  	/* Directive out of context: %s */
        GERR_NOT_TYPENAME,          /* Wanted a type-name: %s */
		GERR_NOT_IDENTIFIER,        /* Wanted an identifer, not: %s */
        GERR_NO_IMP,                /* Defining method before @imp */
        GERR_MISSING_INCLUDES,      /* Function before @includes  */
        GERR__LAST
} ;

enum GW_CODES
{
		GW_DIFFERENT_SUPER = 1,		/* diff superclass @imp-@if */
		GW_BAD_SELECTOR,
		GW_TYPECAST,
		GW_NO_CLASS,
		GW_NO_INCLUDES, 	 		/* no @includes directive */
		GW_MISSING_SEMI,
		GW_GENERIC_METHOD,			/* Generic method called %s */
		GW_METHOD_RET_TYPE,			/* Return type mismatch */
		GW_METHOD_ARG_TYPE,			/* Method arg type mismatch */
		GW_INTERNAL,  				/* Continuing; my insides feel funny */
        GW_DUP_TYPEDEF,             /* Duplicate typedef name added: %s */
        GW_BITFIELD_LENGTH,         /* Bad token in bitfield len: %s */
		GW__LAST
} ;



/* -----------------------  Error Codes  ------------------------------ */

/*  Return codes from parser units: */
#define  RC_OK     0
#define  RC_ERROR  1
#define  RC_EOF   -1


/*  Pass to gerr() is wanting fatal-error recovery: */
#define  ERROR_ERROR     (IS_ERROR)
#define  ERROR_RESYNC    (IS_ERROR|IS_NEED_HLINE)
#define  ERROR_ABORT     (IS_ABORT)


/* ----------------------  Externally, Yours  ------------------------ */

extern void 	gerr PARMS(( /* int , enum GERR_CODES, PFI, ULONG */ ));
extern void 	gwarn PARMS(( enum GW_CODES ));

extern int  	trip_hline;			/* Scanner should output #line */
                     /*  =1 , right now,  =-1, after {} block ends */

extern int		er_semi  PARMS(( void ));	/* Error recovery thinkers */
extern int		er_msg   PARMS(( void ));	/* to msg expr end */
extern int		er_curly PARMS(( void ));	/* to next "}" */
extern int  	er_end   PARMS(( void ));	/* up to @end */
extern int  	er_nl    PARMS(( void ));	/* up to NewLine */

extern int  	get_tok  PARMS(( void ));
extern void 	unget_tok PARMS(( void ));


/*  end of error.h */

