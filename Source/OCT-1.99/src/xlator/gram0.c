/*   gram0.c   Initilization stuff                         March 91, bjw */

/*****************************************************************
 *   This program is (c) COPYRIGHT 1991,1992 by Brian J. Witt.
 *                  seer.UUCP!rael!aleks
 *
 *   The author retains all rights to this work, and may alter
 *   the restrictions/permissions at any time.  The author assumes
 *   no responsibility of usefulness or damage incurred arising
 *   from any use of this intellectual property.
 *   Permission is hereby granted for non-commerical distribution
 *   and usage provided the copyright notices are left intact.  A
 *   small fee ( < $15 US) may be charged to cover distribution costs.
 *
 ******************************************************************/

/*
 *    This file is most of initialization routines we need.  They are
 *    here just to stay out of the way, most of the time.
 *
 *    2-march-91   bjw    Removed file symbol.c file.
 *    28-May-91    bjw    <ctype.h> included from "oct.h"
 *    20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
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

/*  Store typedef'ed names here: */

struct List 	alias_list;			/* handles @alias() */
struct List 	category_list;		/* handles class categories */

static LONG 	_just_in_case;		/* if token DONE == -1 */
unsigned char 	tokenkey[ _LAST_TOKEN_SYMBOL ];



/* ----------------------  Startup and Go Code  ---------------------- */

    void
init_parser( )
{

    /*  General init stuff: */
    m_order = 0;


    /*  if 'DONE' is < 0 then don't store!!!! */
#if (DONE>=-3)
    tokenkey[ DONE ] = TKEY_TRAILING | TKEY_SEPERATE;
#endif

    tokenkey[ SIZEOF ] = TKEY_MONADIC;
    tokenkey[ '!' ] = TKEY_MONADIC;
    tokenkey[ '~' ] = TKEY_MONADIC;

    tokenkey[ '+' ] = TKEY_MONADIC | TKEY_DYADIC;
    tokenkey[ '-' ] = TKEY_MONADIC | TKEY_DYADIC;
    tokenkey[ '*' ] = TKEY_MONADIC | TKEY_DYADIC;
    tokenkey[ '/' ] = TKEY_DYADIC;
    tokenkey[ '%' ] = TKEY_DYADIC;
    tokenkey[ '|' ] = TKEY_DYADIC;
    tokenkey[ '&' ] = TKEY_MONADIC | TKEY_DYADIC;
    tokenkey[ '^' ] = TKEY_DYADIC ;

    tokenkey[ '.' ]    = TKEY_DYADIC;
    tokenkey[ PTR_OP ] = TKEY_DYADIC;

    tokenkey[ INC_OP ] = TKEY_MONADIC | TKEY_TRAILING ;
    tokenkey[ DEC_OP ] = TKEY_MONADIC | TKEY_TRAILING ;
    tokenkey[ LEFT_OP ] = TKEY_DYADIC ;
    tokenkey[ RIGHT_OP ] = TKEY_DYADIC ;

    tokenkey[ EQ_OP ] = TKEY_DYADIC ;		/* "==" */
    tokenkey[ '<' ] = TKEY_DYADIC ;
    tokenkey[ '>' ] = TKEY_DYADIC ;
    tokenkey[ LE_OP ] = TKEY_DYADIC ;
    tokenkey[ GE_OP ] = TKEY_DYADIC ;
    tokenkey[ EQ_OP ] = TKEY_DYADIC ;
    tokenkey[ NE_OP ] = TKEY_DYADIC ;
    tokenkey[ AND_OP ] = TKEY_DYADIC ;
    tokenkey[ OR_OP ] = TKEY_DYADIC ;

    tokenkey[ '=' ] = TKEY_DYADIC;
    tokenkey[ MUL_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ DIV_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ MOD_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ ADD_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ SUB_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ LEFT_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ RIGHT_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ AND_ASSIGN ] = TKEY_DYADIC ;
    tokenkey[ OR_ASSIGN ] = TKEY_DYADIC ;

    tokenkey[ T_CHAR ] = TKEY_TYPENAME ;
    tokenkey[ T_SHORT ] = TKEY_TYPENAME ;
    tokenkey[ T_INT ] = TKEY_TYPENAME ;
    tokenkey[ T_LONG ] = TKEY_TYPENAME ;
    tokenkey[ T_SIGNED ] = TKEY_TYPENAME ;
    tokenkey[ T_UNSIGNED ] = TKEY_TYPENAME ;
    tokenkey[ T_FLOAT ] = TKEY_TYPENAME ;
    tokenkey[ T_DOUBLE ] = TKEY_TYPENAME ;

    tokenkey[ T_STRUCT ] = TKEY_TYPENAME ;
    tokenkey[ T_VOID ] = TKEY_TYPENAME ;
    tokenkey[ T_UNION ] = TKEY_TYPENAME ;
    tokenkey[ T_ENUM ] = TKEY_TYPENAME ;

    tokenkey[ T_CONST ] = TKEY_TYPENAME | TKEY_STORECLASS;
    tokenkey[ T_VOLATILE ] = TKEY_TYPENAME | TKEY_STORECLASS;

    tokenkey[ T_EXTERN ] = TKEY_STORECLASS;
    tokenkey[ T_STATIC ] = TKEY_STORECLASS;
    tokenkey[ T_AUTO ] = TKEY_STORECLASS;
    tokenkey[ T_REGISTER ] = TKEY_STORECLASS;

    tokenkey[ '?' ] = TKEY_DYADIC ;
    tokenkey[ ':' ] = TKEY_DYADIC ;
    tokenkey[ ',' ] = TKEY_TRAILING | TKEY_SEPERATE ;
    tokenkey[ ';' ] = TKEY_TRAILING | TKEY_SEPERATE ;
    tokenkey[ CH_CLOSE_PAREN ] = TKEY_TRAILING ;
    tokenkey[ ']' ] = TKEY_TRAILING ;
    tokenkey[ CH_RCURLY ] = TKEY_TRAILING | TKEY_SEPERATE ;
    tokenkey[ CH_LCURLY ] = TKEY_SEPERATE ;


    tokenkey[ OPEN_INTERFACE ] = TKEY_OC_SPECIAL;
    tokenkey[ OPEN_IMPLEMENTATION ] = TKEY_OC_SPECIAL;
    tokenkey[ OPEN_PROTOCOL ] = TKEY_OC_SPECIAL;
    tokenkey[ CLOSE_END  ] = TKEY_OC_SPECIAL;

    tokenkey[ OPEN_FACTORY_METHOD  ] = TKEY_OC_SPECIAL;
    tokenkey[ OPEN_INSTANCE_METHOD ] = TKEY_OC_SPECIAL;
    tokenkey[ PARM_LABEL ] = TKEY_OC_SPECIAL;
    tokenkey[ OOC_DEFS  ] = TKEY_OC_SPECIAL;
    tokenkey[ OOC_SELECTOR ] = TKEY_OC_SPECIAL;
    tokenkey[ OOC_PUBLIC ] = TKEY_OC_SPECIAL;
    tokenkey[ OOC_ENCODE ] = TKEY_OC_SPECIAL;

    tokenkey[ T_DUMMY ] = TKEY_OC_SPECIAL;

}	/* init_parser */



    void
init_types( )
{
                               /*  [nodeName] -->  [def],      [tag]    */
   symtab  = (void *)newTree();	 /* "USHORT" --> "unsigned short"         */
   ivar_tree = (void *)newTree();  /* "Point" --> "id xval, yval;","id id" */
   class_tree = (void *)newTree();  /* "Point" --> "Object"                */
   method_tree = (void *)newTree();  /* "at:"   --> "(id)(int)", "offset"  */

   NewList( & category_list );      /* "Node"  --> "doobie"; --> "test"   */
   NewList( & alias_list );         /* "atChar:" --> "at:"               */
}	/* init_types */

