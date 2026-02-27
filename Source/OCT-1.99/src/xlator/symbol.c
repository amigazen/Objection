/*   SYMBOL.C    Symbol table search and rescue             Aug 89, bjw */
/*	:ts=8	%W%  %G%  */

/*****************************************************************
 *   This program is copywrite (c) 1989,1990,1991,1993 by Brian J. Witt.
 *                  aleks@rael.mystic.com
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
 *    Maintain some internal tables.  Also helper routines for the
 *    grammar.  Error handler and table init stuff.  The function
 *    lex_text() saves text as it is being read in.  Caller can
 *    query status, append or prepend a string, and stop to
 *    retrieve currently saved string.
 * 
 *    09-aug-89     bjw    Ripped code from strs.c
 *    16-aug-89     bjw    Include filenames more ANSI C-like.
 *    2-sept-89     bjw    Imported grammar support stuff.
 *    7-oct-89      bjw    strdup() instrad of STRSAVE().
 *    12-oct-89     bjw    gerr() prints "erorr" or "fatal".
 *    4-nov-89      bjw    Error handlers code moved to separate file.
 *    27-mar-90     bjw    Added pr_name_method(,). Now str_name_method().
 *    15-sept-90    bjw    Aligned towards types of NeXT.
 *    27-sept-90    bjw    "SHR" is now "Class".
 *    2-march-91    bjw    Removed initialization code --> gram0.c file.
 *    12-april-91   bjw    Changed /strdup/ --> /newstring/.
 *     28-May-91    bjw    <ctype.h> included from "oct.h"
 *     7-june-91    bjw    Fixed NULL ptr to strcmp() in lu_word().
 *     2-july-92    bjw    mk_mynode() always sets 'lineDefined.'
 */


#define  SW_ASSERT  1
#define  SW_WATCH  0
#define  SW_DEBUG  0


#include "oct.h"
#include <tree.h>
#include "gram.h"
#include "error.h"

#include <objc/objc-class.h>


/* --------------------  Our Private Variables  ---------------------- */


/* ---------------------  Symbol Table Support  ------------------------- */


/*   enter_type  --  Record a typedef as being recognized.  These
 *            	     typedef'ed names are used during parsing of structures
 *            	     for legal tokens.  Remembers what the might be.
 *              The node is given to the type equivalence tree.
 */
    void
enter_type( tname, equiv )
    CONST char	*tname;
    CONST char	*equiv;
{
    struct mynode  *typedefNode;

    if( searchTree(symtab, tname) == (struct Node *)NULL )
    {
    	typedefNode = mk_mynode( tname, equiv );
    	addTree( symtab, &typedefNode->node );
    } else
    {
        error_string = tname;
        gwarn( GW_DUP_TYPEDEF );
#if SW_ASSERT
        if( yydebug )
        {
            typedefNode = (struct mynode *) searchTree(symtab,tname);
            printf( "dup %s - %s vs %s\n", tname, equiv, typedefNode->def );
        }
#endif
    }

}	/* enter_type */


/*   lu_type  --  Return a boolean True/False if the passed token is a
//            	  typedef name.
*/
    int
lu_type( tname )
    char	*tname;
{
    return( (searchTree(symtab, tname) != NULL) );
}	/* lu_type */


/*   str_name_method  --  Provide a standard formatting routine to display
 *                        method name.  Returns string in global 'buff'.
 *           The leading char is -/+ for instance/factory methods.
 */
    char *
str_name_method( node )
    CONST register struct mynode	*node;
{

    sprintf( buff, " - [ %s ", node->node.ln_Name );
    if( node->order == 'F' )
    	buff[1] = '+' ;

    if( lastchr( node->node.ln_Name ) == ':' )
    	strcat( buff, ".. " );
    strcat( buff, "] " );

    return( & buff[0] );
}	/* str_name_method */


/*   enter_instance  --  Create a node remembering an instance variable.
 *                       Copies of all strings are made.
 */
void
enter_instance( class, variables, types, elements, publics )
    CONST char    *class;                  /* "ByteArray" */
    CONST char    *variables;              /* "{ char *string; }" */
    CONST char    *types;                  /* "char *"    */
    CONST char    *elements;               /* "string"    */
    CONST char    *publics;                /* ""          */
{
    struct mynode   *np;

    np = mk_mynode( class, variables );
    np->tag         = newstring( types );
    np->names       = newstring( elements );      /* Just element names */
    np->publicNames = newstring( publics );

    addTree( ivar_tree, & np->node );

}   /* enter_instance */



/* -----------------------  Object Maintaince  ------------------------- */


/*   lu_word  --  During parsing of class definition, we kinda cheat on
 *                the parsing.  This function supports the scanning.  It
 *                should be replaced!
 *   Side Effects:  Stores string in global 'buff'.
 */
    LOCAL struct TYPECONNS *
lu_word( table, ptrMatchable )
    struct TYPECONNS	*table;    	/* Search this table */
    char    	**ptrMatchable;         /* InOut:  current pointer */
{
    register char	*p;
    struct TYPECONNS	*tableEntry;
    char	*buffPtr;

    if( *ptrMatchable == (char *)NULL )
        return( NULL );

    for( p=(*ptrMatchable) ; *p != EOS && !isalpha(*p) && *p != '_' ; ++p )
    	;;
    if( *p == EOS )	/* Ran out of string to inspect. */
    {
    	*ptrMatchable = NULL;
    	return( NULL );
    }

    /*  Copy word into 'buff' (global) */
    buffPtr = buff;
    do {
    	*buffPtr++ = *p++ ;
    } while( *p == '_' || isalnum(*p) ) ;
    *buffPtr = EOS;

#if SW_DEBUG
    printf( "lu_word <%s>.\n", buff );
#endif

    *ptrMatchable = p;      	/* Update user's text ptr  */
    for( tableEntry=table ; tableEntry->c_name != NULL ; ++tableEntry )
    {
        if( STREQ( tableEntry->c_name, buff ) )
        {
#if SW_DEBUG
printf( "(lu_word) match \"%s\" --> '%c'. \n",
			tableEntry->c_name, tableEntry->id_chr );
#endif
            return( tableEntry );
        }
    }

    tableEntry->id_chr = -4;	  /* No match for name.. :-(  */
    return( tableEntry );         /* Can't find word match in our list */
}	/* lu_word */



/*   asTypeString  --  Decode the types used in the instance variables of
 *                     this class.  Returns a string to describe the vars.
 *                     The string is static declared.  Trys to decode the
 *          length of an single dimensioned array as long as size is a
 *          simple number.  Exprs output eof symbol.  Caller owns string.
 *
 *   EXAMPLES:
 *          "Class isa ; short xx[5]; unsigned foo"  --> "#sssssI"
 *          "unsigned short foo; struct { int bar, yoda;} zz;" --> "S{ii}"
 *
 *   BUGS:
 *          Comments are not handled at all.
 *          Structures should be names (ala symbolic debuffer).
 *          Complex array sizes terminate type string.
 *          Multi-dimensional arrays are not handled.
 */
    char *
asTypeString( m_vars )
    char	*m_vars;	    /* DON'T store in global buff[] !!! */
{
    static struct TYPECONNS   tc[ ] = {
    {	"int",    	_C_INT	},    	/* "C" built-in types */
    {	"long",    	_C_LNG	},
    {	"double",	_C_DBL	},
    {	"float",	_C_FLT	},
    {	"short",	_C_SHT	},
    {	"char",    	_C_CHR	},   	/*  signed  */

    {	"APTR",    	'^'    },
    {	"BPTR",    	'^'    },
    {	"CPTR",    	'^'    },

    {	"BYTE", 	_C_CHR },
    {	"UBYTE",	_C_UCHR },
    {	"WORD",    	_C_SHT },
    {	"UWORD",	_C_USHT },
    {	"LONG",    	_C_LNG },
    {	"ULONG",	_C_ULNG },

    {	"unsigned",	-1	},    	/* Ignore this one */
    {	"struct",	-2	},    	/* Our defined stuff */
    {	"union",	-2	},    	/* Our defined stuff */
    {	"STR",    	_C_CHARPTR	},    	/* Text */
    {	"id",    	_C_ID	},    	/* an ID (a generic thing) */
    {	"BOOL",    	_C_BOOL	},
    {	"IOD",    	_C_IOD	},    	/* File Stream pointer */

    {	"_SHARED",	_C_CLASS    },	/* These are class structures */
    {	"SHR",  	_C_CLASS    },
    {	"Class",	_C_CLASS    },
    {	"objc_class", _C_CLASS     },

#if 0
    /*  Application short-hands: */
    {	"RECT",    	_C_RECT    },
    {	"VECT",    	_C_VECT    },
#endif

    {	NULL,    	0	}    	/* End of list */
  } ;

    char	    ids[ 90 ];   	/* Returned to our caller */

    register struct TYPECONNS	*found;
    register short	typeCode;
    register char	*buffPtr = ids;
    struct mynode       *instVarTypeNode;
    char         *next = m_vars;
    char         tempstr[16];
    int          struct_nest = 0;
    char	 sawUnsignedLastTime;	  /* Flag to catch "unsigned foo" */

#if SW_DEBUG
    printf( "(asTypeString) using \"%s\"\n", m_vars );
#endif

    sawUnsignedLastTime = FALSE;
    while( (found=lu_word( tc, & next )) != NULL )
    {
    	typeCode = found->id_chr;
    	if( typeCode > 0 )
            *buffPtr++ = found->id_chr;
    	else
    	{
            if( typeCode == -1 )
	    {
	    	sawUnsignedLastTime = TRUE;
            	continue ;            	/* ignore "unsigned" */
            } else
            if( typeCode == -2 )
            {
                *buffPtr++ = CH_LCURLY ;
                ++struct_nest;
                typeCode = _C_PTR;        	/* A generic pointer */
            	if( (found = lu_word( tc, & next )) != NULL )
                    typeCode = found->id_chr;

                if( typeCode > 0 )
                    *buffPtr++ = typeCode;
            } else
            if( lu_type( found->c_name ) )  /* Check type database */
            {
                instVarTypeNode =
		      (struct mynode *) searchTree( symtab, found->c_name );
#if SW_WATCH
		printf( "(asTypeString) type <%s> --> <%s>\n",
				instVarTypeNode->node.ln_Name,
				instVarTypeNode->def );
#endif
            } else
	    {
	    	if( ! sawUnsignedLastTime )
		    goto  COME_UP_SHORT;     /* Can't recoginize word :-( */

		/*  Hey, it was "unsigned foo;"  */
	        typeCode = _C_UINT;
	        *buffPtr++ = typeCode;
	    }
    	}

    NXT_WORD :          /* {  */
    	while( strchr( ";,:[}", *next ) == NULL )
        	++next;
        if( *next == ':' )     /* Bail out on a bitfield :-( */
            goto  COME_UP_SHORT;

        if( *next == CH_RCURLY )     /* End of included structure */
            *buffPtr++ = CH_RCURLY ;
        else
        if( *next == '[' )     /* ] ] (keep vi happy) */
        {
            char   *p;
            int    count = 0;

            while( ! isdigit(*++next) )  ;;
            for( p=tempstr ; isdigit(*next) ; ++next )
            {
                *p++ = *next;
            }
            *p = EOS;
            while( isspace(*next) )
                ++next;
            if( *next == ']' )
            {
                /*  Found array, count is a simple number. */
                count = atoi( tempstr );
#if SW_DEBUG
	printf( "array count=%s of \"%c\"\n", tempstr, typeCode );
#endif
                while( --count > 0 )    /* One 'type' already in string. */
            	    *buffPtr++ = typeCode;
            }
            else
            {
                /*  If not a simple number, then skip expr:  */
                while( *++next != ']' )  ;;
                goto  COME_UP_SHORT ;
            }

    	    while( *next != ';' && *next != ',' && *next != '[' )
            	++next;
    	    if( *next == '[' )     /* Multi-dimensionl array :-( */
    	        goto  COME_UP_SHORT;

        }   /* if array count */

    	if( *next++ == ',' )
    	{
            *buffPtr++ = typeCode;     /* Subsequent variable declaration */
            goto  NXT_WORD;
    	}

        sawUnsignedLastTime = FALSE;
    }	/* while more variables to parse */

  COME_UP_SHORT :
    while( struct_nest-- > 0 )
	   *buffPtr++ = CH_RCURLY;

    if( next != NULL )
    {
    	*buffPtr++ = _C_eof;
    }
    *buffPtr = EOS;

#if SW_WATCH
    if( yydebug )
    	printf( "(asTypeStr) <%s> --> <%s>\n", m_vars, ids );
#endif

    return( newstring(ids) );
}	/* asTypeString */



/*   lu_instvar  --  If target is simply the name of a class instance
 *              variable, then return TRUE, else return FLASE.
 * 
 *   Note:  Since the ivar node is cached, ivars can't be added once
 *          this routine is called.
 */
    int
lu_instvar( target )
    CONST char	*target;
{
    extern char   *publicClassVarSTR();

    static struct mynode 	*my_ivar = NULL;
    static char    *varstring = NULL;

    register char	c, *p;
    int 	found = FALSE;

    if( varstring == NULL )
    {
        varstring = publicClassVarSTR( className );
        if( yydebug )
        {
    	    if( my_ivar != NULL )
                printf( "(lu_instvar) gunna search <%s>\n", varstring );
        }

    }	/* if first time thru.. */

    if( varstring != NULL && (p=strstr( varstring, target )) != NULL )
    {
        /*  If this is a word (spaces at begin and end), then accept: */
    	if( isspace(p[-1]) &&
            (isspace(c=p[strlen(target)]) || c == EOS) )
        {
            found = TRUE;
        }
    }   /* if found word inside varstring.. */

    return( found );
}	/* lu_instvar */



/* -----------------  Lexer Value Added Functions  ---------------------- */


/*   lex_text  --  Controls gathering of text after lexer has examined it.
 *                 'code' states the method to perform.
 *   => LT_START to stop processing, pass 'trim off garbage chr'
 *                The string returned call be free()'d.
 *   => LT_STOP to start catching souls as they passs.
 *   => LT_APPEND1 to append on char to capture string.
 *   => LT_TRIM trim capture string by 'eatum' bytes.
 *   => LT_ADD concat an additional string, eatum=0(FRONT), 1(REAR).
 *   => LT_QCAPTURE returns current capture string, NULL if no save.
 */
    char * 
lex_text( code, eatum, plus )
    enum key_lex_text   code;    	/* See codes for actions */
    UCHAR	eatum; 	    	/* When terminating, eat this char! */
    char	*plus;        	/* Additional string. */
{
    extern short  save_text;
    extern char   *texts;

    register char	*s;
    register char	*ret = NULL;
    char   adds[ 2 ];
    int 	pos;

    switch( code )
    {
    	case LT_QCAPTURE :
            	ret = texts;
            	break ;

    	case LT_STOP :
#if SW_ASSERT
            	if( save_text == FALSE && yydebug )
            	{
                    /*  This happens during normal operation: */
                    printf( "lex_text: already stopped\n" );
            	}
#endif
            	if( (ret=texts) == NULL || *ret == EOS )
            	{
                    ret = newstring( NULL );   	/* Forge a string */
            	} else
            	{
                    /* Trim the trailing char */
                    s = ret + strlen(ret);
                    while( --s >= ret )
                    {
                	if( *s == eatum )
                	{
            	            *s = EOS;  	/* Keep going to trim spaces, too */
            	            break ;	 /* If we don't stop here, others die */
                	}
                	else
                	if( *s == ' ' )
            	            *s = EOS ;
                	else
                	if( ((unsigned char)*s) > ' ' )
            	            break ;
                    }
            	}

            	texts = NULL;
            	save_text = FALSE;    	/* End capture */

            	break ;

    	case LT_START :
            	texts = NULL;
            	save_text = TRUE;    	/* Start capturing */
#if SW_DEBUG
            	putchar( '#' );
#endif
            	break ;

    	case LT_APPEND1 :        	/*  Append one char to string */
            	if( save_text == FALSE )
            	{
#if SW_ASSERT
                    if( yydebug )
                    {
                        printf( "(lex_text:APPEND1) %s:%u: restart for <%c>..\n",
                                src_filename, lineat, eatum );
                    }
#endif
                	save_text = TRUE;
            	}
            	adds[ 0 ] = eatum;
            	adds[ 1 ] = EOS;
            	ret = texts = newstrcat( texts, adds );

            	break ;

    	case LT_TRIM :            	/*  Remove some chars from string */
            	if( save_text == FALSE )
            	{
#if SW_ASSERT
                    printf( "(lex_text) can't trim..\n" );
#endif
            	} else
            	{
                    pos = strlen(texts) - eatum;
                    if( pos >= 0 )
                    	texts[ pos ] = EOS;
            	}

            	break ;

    	case LT_ADD :
            	if( save_text == FALSE )
            	{
#if SW_ASSERT
                    printf( "(lex_text:ADD) restart..\n" );
#endif
                    save_text = TRUE;
            	}

            	if( eatum )
                    texts = newstrcat( texts, plus );	/* 1 : REAR */
            	else
                    texts = newstrins( plus, texts );	/* 0 : FRONT */

            	break ;

    }   /* switch (lex code) */

    return( ret );
}	/* lex_text */



/* ------------------------ 'mynode' Support  -------------------------- */


/*   mk_mynode  --  Store a key-value pair onto a list node.  The string
//            	    values are saved here, so both strings can be from
//            	    static buffers.  If no memory, this routine aborts.
*/
    struct mynode *
mk_mynode( name, value )
    char	*name, *value;
{
    register struct mynode  *newNodePtr;

    if( (newNodePtr=(struct mynode *)MALLOC(sizeof(struct mynode))) == NULL )
    {
    	write( 2, "Core - mynode!\n", 15 );
    	exit( 10 );
    }
    memset( (char *)newNodePtr, 0, sizeof(struct mynode) );

    newNodePtr->node.ln_Name = newstring( name );
    newNodePtr->lineDefined  = lineat;
    newNodePtr->def          = newstring( value );

    return( newNodePtr );
}	/* mk_mynode */


    void
done_mynode( node )
    register struct mynode *node;
{

                     MFREE( node->node.ln_Name );
    if( node->def)   MFREE( node->def );
    if( node->tag)   MFREE( node->tag );
    MFREE( node );
}   /* done_mynode */

