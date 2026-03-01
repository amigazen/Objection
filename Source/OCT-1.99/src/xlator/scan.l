%{
/*    SCAN.L   Lexical definition for ANSI "C", April 30, 1985 draf  */

/*****************************************************************
 *   This scanner was originally from Fred Fish disk #157 as a
 *   "C" grammar by William Loftus.  Both a scanner and a grammar
 *   were contributed; I use only the scanner.
 *  
 *  From the FF disk:

[FF157:grammars/readme.fnf]
This material was submitted to me on disk directly by William Loftus,
for inclusion in the library.

-Fred  ><>
22-Sep-88
--------
[FF157:grammars/C/README]
The files in this directory contain the ANSI C grammar from the April 30, 1985
draft of the proposed standard. This copy also incorporates all bug fixes I
have seen since the last two postings. With a little work this grammar can
be made to parse the C that most of us know and love (sort of).

There is one bug fix to the grammar that is in this posting. On line 295
of gram.y it previously read declaration_specifiers instead of
type_specifier_list as it does now. I believe the folks at the ANSI committee
made a mistake since if you replace the line with what the original read
you will end up with 16 shift/reduce errors and 2 reduce/reduce errors
(the good ones). As it is, it only has 1 shift/reduce error that occurs
on the if/else construct. YACC creates the correct parser and I don't want
to ugly my grammar up.

Anyway, all cumquats unite and generate this sucker. Then just sit and play
with it. Remember, the grammar accepts things like

    "Hello, world"++;
    --1.23456;
    *'a'

but this is not a bug, but simply a shuffling of the checking into the
semantic analysis. If you want to hack it up to do lvalue and rvalue
checking, I'm sure the ANSI committee would be glad to have your changes.
Don't send'em to me though. I don't want'em. Wear this in good health.

Jeff Lee
gatech!jeff	jeff@gatech	jeff%gatech.CSNet@CSNet-Relay.ARPA

 *
 ******************************************************************/


/*
 *    Successfull translated with Xenix 3.3 lex and FSF FLEX 1 "13 May 1987".
 *
 *    Mods:
 *        * Changed "CONST" into "CONST_T" to avoid symbol clashes with prog.
 *        * Hands off #-line processing.
 *
 *        'td_nest' goes to -1 upon EOF.  Done in get_tok().
 *
 *        input()  --  extents yytext string.
 *        unput()  --  prepares text for *next* token.
 *
 *        The following OCT pre-processor functions are defined:
 *        @selector( <selector> )     " @selector( at:putChar: ) "
 *                                    " MS_AT_PUTCHAR_ "
 *            converts selector into internal code.[25-july-89]
 *        @defs( <class_name> )       " @defs( Sequence ) "
 *                                    " Class isa; id contents; int capacity; "
 *            replaces with instance variable definition for class.[25-july-89]
 *        @includes
 *            inserts any includes the translator inserts.[10-aug-89]
 *      @protocol ClassName
 *          Creates template of methods for abstract classes.[29-oct-89]
 *        Allow "^+" and "^-" as token within expressions.[01-nov-89]
 *      @public
 *          Denotes following instance variables are public accessable
 *          [14-dec-90].
 *
 *        ******** Update to Version 4 of Objective-C ********
 *        These are defined:
 *            @interface
 *            @implementation
 *            @end
 *
 *        This file can be translated using lex from AT&T UNIX(tm). 
 *      Change the define 'IAM_FLEX', remove the #define'd for 'output',
 *      and expose the %p directive below.
 *        This was tested under Xenix.                [29-aug-89].
 *
 *        The Elipsis has been added.                 [03-mar-90].
 *        changed /strdup/ --> /newstring/            [12-apr-91].
 *
 *     28-May-91    bjw     <ctype.h> included from "oct.h"
 *    20-april-92  bjw    @abstract --> @protocol (for NeXT v2).
 */


/* vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

#define  SW_WATCH_LINE	0

#define  IAM_FLEX	1
%}

/*  %p size used only by UNIX lex: */
#if IAM_FLEX
#else
%p 3000
#endif

D                       [0-9]
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+
FS                      (f|F|l|L)
IS                      (u|U|l|L)*
WS                        [ \t]

%{

#define  SW_WATCH  0

#define  GRAMMAR  1


#include "oct.h"
#include "gram.h"


/*  Flex doesn't define this macro (remove if using UNIX lex): */
#if IAM_FLEX
# define  output(c)  fputc(c,yyout)
#endif

#ifndef TRUE
# define  TRUE   1
# define  FALSE  0
#endif


/* ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */


void 	count();
void 	ne_count();
void	hash_line();
void 	hash_directive();
void 	comment();
void 	comment2();
int     check_type();
void	inc_td_cnt();
void	dec_td_cnt();

void	unput_last();
int     msg_parm();
int 	open_method();
void	ooc_selector( );
void	ooc_defs( );
void	ooc_includes( );
void	ooc_encode( );
void    ooc_public( );
void    ooc_protected( );
void    ooc_private( );
void    ooc_alias( );

%}


%%
"/*"                    { comment(); }
"//"                    { comment2(); }
^"#"{WS}*"line"{WS}+	{ hash_line(1); }
^"#"{WS}*{D}            { unput_last(); hash_line(1); }
^"#"                	{ hash_directive(); }   /* Catch-all */

"auto"                  { count(); return(T_AUTO); }
"break"                 { count(); return(BREAK); }
"case"                  { count(); return(CASE); }
"char"                  { count(); return(T_CHAR); }
"const"                 { count(); return(T_CONST); }
"continue"              { count(); return(CONTINUE); }
"default"               { count(); return(DEFAULT); }
"do"                    { count(); return(DO); }
"double"                { count(); return(T_DOUBLE); }
"else"                  { count(); return(ELSE); }
"enum"                  { count(); return(T_ENUM); }
"extern"                { count(); return(T_EXTERN); }
"float"                 { count(); return(T_FLOAT); }
"for"                   { count(); return(FOR); }
"goto"                  { count(); return(GOTO); }
"if"                    { count(); return(IF); }
"int"                   { count(); return(T_INT); }
"long"                  { count(); return(T_LONG); }
"register"              { count(); return(T_REGISTER); }
"return"                { count(); return(RETURN); }
"short"                 { count(); return(T_SHORT); }
"signed"                { count(); return(T_SIGNED); }
"sizeof"                { count(); return(SIZEOF); }
"static"                { count(); return(T_STATIC); }
"struct"                { count(); return(T_STRUCT); }
"switch"                { count(); return(SWITCH); }
"typedef"               { count(); return(T_TYPEDEF); }
"union"                 { count(); return(T_UNION); }
"unsigned"              { count(); return(T_UNSIGNED); }
"void"                  { count(); return(T_VOID); }
"volatile"              { count(); return(T_VOLATILE); }
"while"                 { count(); return(WHILE); }
"..."                   { count(); return(ELIPSIS); }


^"@interface"           { ne_count(); return(OPEN_INTERFACE); }
^"@implementation"      { ne_count(); return(OPEN_IMPLEMENTATION); }
^"@protocol"            { ne_count(); return(OPEN_PROTOCOL); }
^"@class"               { ne_count(); return(OPEN_CLASS); }
^"@end"                	{ ne_count(); return(CLOSE_END); }
^"+"                	{ return open_method();  }
^"-"                	{ return open_method();  }

"@selector"                {  ooc_selector( ); }
"@defs"                    {  ooc_defs( );     }
"@includes"                {  ooc_includes( );	}

"@public"                {  ooc_public( );  }
"@protected"             {  ooc_protected( );  }
"@private"               {  ooc_private( );  }
"@encode"                {  ooc_encode( );	}
"@alias"                 {  ooc_alias( );	}


{L}({L}|{D})*":"        {          return(msg_parm()); }
{L}({L}|{D})*           { count(); return(check_type()); }


0[xX]{H}+{IS}?          { count(); return(CONSTANT); }
0{D}+{IS}?              { count(); return(CONSTANT); }
{D}+{IS}?               { count(); return(CONSTANT); }
'(\\.|[^\\'])+'         { count(); return(CONSTANT); }

{D}+{E}{FS}?            { count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?  { count(); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?  { count(); return(CONSTANT); }

\"(\\.|[^\\"])*\"       { count(); return(STRING_LITERAL); }

">>="                   { count(); return(RIGHT_ASSIGN); }
"<<="                   { count(); return(LEFT_ASSIGN); }
"+="                    { count(); return(ADD_ASSIGN); }
"-="                    { count(); return(SUB_ASSIGN); }
"*="                    { count(); return(MUL_ASSIGN); }
"/="                    { count(); return(DIV_ASSIGN); }
"%="                    { count(); return(MOD_ASSIGN); }
"&="                    { count(); return(AND_ASSIGN); }
"^="                    { count(); return(XOR_ASSIGN); }
"|="                    { count(); return(OR_ASSIGN); }
">>"                    { count(); return(RIGHT_OP); }
"<<"                    { count(); return(LEFT_OP); }
"++"                    { count(); return(INC_OP); }
"--"                    { count(); return(DEC_OP); }
"->"                    { count(); return(PTR_OP); }
"&&"                    { count(); return(AND_OP); }
"||"                    { count(); return(OR_OP); }
"<="                    { count(); return(LE_OP); }
">="                    { count(); return(GE_OP); }
"=="                    { count(); return(EQ_OP); }
"!="                    { count(); return(NE_OP); }
";"                     { count(); return(';'); }
"{"                     { count(); inc_td_cnt(); return(CH_LCURLY); }
"}"                     { count(); dec_td_cnt(); return(CH_RCURLY); }
","                     { count(); return(','); }
":"                     { count(); return(':'); }
"="                     { count(); return('='); }

"("                     { count(); return('('); }
")"                     { count(); return(')'); }
"["                     { count(); inc_td_cnt(); return('['); }
"]"                     { count(); dec_td_cnt(); return(']'); }

"."                     { count(); return('.'); }
"&"                     { count(); return('&'); }
"!"                     { count(); return('!'); }
"~"                     { count(); return('~'); }
"-"                     { count(); return('-'); }
"+"                     { count(); return('+'); }
"*"                     { count(); return('*'); }
"/"                     { count(); return('/'); }
"%"                     { count(); return('%'); }
"<"                     { count(); return('<'); }
">"                     { count(); return('>'); }
"^"                     { count(); return('^'); }
"|"                     { count(); return('|'); }
"?"                     { count(); return('?'); }

[ \t\v\n\f]             { count(); }
.                       { /* ignore bad characters */ }

%%

/* ----------------------  Handling Code  ------------------------------- */


#define  SIZ_BUFFER   120

unsigned    lineat = 1, column = 0;
extern short save_text;   /* defined in main.c (oct.h when MAIN) */
extern char *texts;

int     	trip_hline;

/*  Result of scan_next_word() stored here:  */
static char  scan_keep[ SIZ_BUFFER+2 ];


/*   yywhitespace  --  Eat whitespace upto next token, but preserve
 *                     the next token.  Track our column/lineat counters.
 *    Outline:
 *         while( isspace(nextByte) )
 *             eat char,
 *             update lineat/column counters.
 */
    void
yywhitespace()
{
	int    c;
	char   newline = FALSE;

	while( (c=input()) != 0 && isspace(c) )
    {
	    /*  Calc position of this char: */
		switch( c )
		{
			case '\f' :
			case '\n' :
			        ++lineat ;
					column  = 0;
			        newline = TRUE;
					break ;

			case '\t' :
			        column += 8 - (column%8) ;
					break ;

			default :
			        ++column;
					break ;
		}
	}
	/*  Now restore and summarize: */
    unput(c);
	if( newline )
    {
		unput( '\n' );
		--lineat;
    }
}   /* yywhitespace */


/*   scan_next_word  --  Manually read ahead for next token.  Will not
 *                       move off current line.  Ie, EOLN terminates
 *                       search unsuccessfully.  Returns 0 if errors.
 *            Returns last char is scan_keep[] contains valid string.
 */
    static int
scan_next_word( )
{
    register char	*p;
    register int  	c;
    int        last = ' ';

    /*  Skip to next word (skip horizontal space):  */
    while( (c=input()) == ' ' || c == '\t' )
        ;;
    switch( c )
    {
	case 0 :
            return 0;

	case '"' :
            last = c;
            c = input();            /* Skip quotes */
            break ;

	case CH_OPEN_PAREN :
            last = CH_CLOSE_PAREN;
            c = input();             /* Find first close paren */
            break ;
    }

    for( p = (&scan_keep[0]) ;
         p < &scan_keep[SIZ_BUFFER] && c != last && c != '\n' && c != '\0' ;
         c = input() )
    {
        *p++ = c;
    }
    *p = '\0' ;

    if( c != last )
        unput( c );

    return( c );
}   /* scan_next_word */


/*   unput_last  --  Unput the last char of current token.
*/
    void
unput_last()
{
    yyless( yyleng-1 );
}


/*  comment  --  Do a slash-star comment.  Whole comment is eaten.
*/
    void
comment()
{
        char c, c1;

 loop:
        while ((c = input()) != '*' && c != 0)
            if( c == '\n' )
                ++lineat;

        if ((c1 = input()) != '/' && c != 0)
        {
                unput(c1);
                goto loop;
        }
}   /* comment */


/*   comment2  --  Do a slash-slash comment until end of line.  Eat me.
*/
    void
comment2( )
{
    int   c;

    while( (c=input()) != '\n' && c != 0 )
        ;
    unput( '\n' );
}   /* comment2 */


    void
hash_line( grabit_flag )
    int 	grabit_flag;
{
    extern int   atoi();
    extern char  *src_filename;

    char    *last;
    int 	c = 0, flag = 0;

    last = "\n" ;
    if( grabit_flag > 0 )
    {
        /* update name and position: */
        /*  #line  <line_number>  ["<filename>"]  */
        /*        ^-- inbuff ptr                  */

        flag = 1;
        last = "";
        if( (c=scan_next_word()) > 0 && strlen(scan_keep) > 0 )
        {
            lineat = atoi( scan_keep );
            if( lineat > 4 )      /* Prevent '#line 0' from appearing */
            {
                --lineat;
                last = "\n" ;
            }

            flag = 2;
            c = scan_next_word();
            if( strlen(scan_keep) > 0 )
            {
                MFREE( src_filename );
                src_filename = newstring( scan_keep );
            }
        }

#if SW_WATCH_LINE
        printf( "(hashline) flag=%d, lineat=%u.\n", flag, lineat );
#endif
        if( c != '\n' && c != 0 )
        {
     	    for ( ; c != '\n' && c != 0 ; c=input() )
                ;;
      	    unput( c );         /* Give back newline */
        }
    } 

    fprintf( yyout, "#line %u \"%s\"%s", lineat, src_filename, last );

}   /* hash_line */


/*   hash_directive  --  Catch hash-directives that aren't "#line" stuff.
 *                       These directives are copied verbatum (kinda).
 */
    void
hash_directive()
{
    register int   c;

 	output( '#' );              /* Scanner ate this, so output it again */

    c = input();
 LOOP:
    while( c != '\\' && c != '\n' && c != '\0' )
    {
        output(c);
        c = input();
    }

    /* If this line continues, advance to next line and continue copying! */
    if( c == '\\' )
    {
        output( c );
        if( (c=input()) == '\n' )
        {
            output('\n');
            ++lineat;
            c = input();
        }
        goto  LOOP;
    }

    /*  'c' is either end-of-file, or newline: */
    if( c == '\n' )
        unput( c );

}   /* hash_directive */


#if 0
/*   unget_newline  --  Hack to spit out another newline and back up the
 *                      line counter.  We hope that soon a #line directive
 *                      will be outputted to resynch the world.  KLUDGE!!
 */
    void
unget_newline()
{

    --lineat;
    unput( '\n' );
}   /* unget_newline */
#endif


/*   all_count  --  Maintain line and column counter for the token just
//                  accepted.  If we're in a collection mode for lex_text(),
//                  then append a parameter separator symbol.  Special
//                  care is taken to not add too many separators.  Also,
//           any byte sized value may be the separator, so lots of
//           conditionals exist to test against.  If you change anything
//           here, better run a complete test suite against the new code!
*/
    void
all_count( echoFlag )
    int   echoFlag;
{
    /* static char  sep[2] = { CH_PARM_SEP, '\0' } ; */
    register char	*t;
    register int  ch;

    if( trip_hline && !save_text && (yytext[0] == '\n') )
    {
        trip_hline = 0;
        {  fprintf( yyout, " /%c vvv trip_hline vvv %c/\n", '*', '*' );  }
        hash_line(-1);            /* Re-synch 'cc' with input file */
    }

    /* if( echoFlag && !in_class_def && !in_method_def ) */
    if( echoFlag && 
       ((in_state   & (IS_SEEN_IF|IS_CLASS_DEF|IS_METHOD_DEF)) == 0 &&
        (in_context & IC_MESSAGE) == 0 ) )
    {
        ECHO;
    }

    for( t = &yytext[0] ; (ch=(unsigned char)*t) != '\0'; ++t )
    {
        if (ch == '\n' || ch == '\f')
        {
            column = 0;
            ++lineat;
        } else if (ch == '\t')
            column += 8 - (column % 8);
        else
            column++;
    }

    if( save_text )
    {
            ch = (unsigned char) yytext[0];
            if( echoFlag && ch > ' ' && isascii(ch) )
                	texts = newstrcat( texts, yytext );
#if SW_WATCH
            if( !echoFlag && ch > ' ' && isascii(ch) )
                	printf( "-- symbol save prevented <%s> --\n", yytext );
#endif
    }

}   /* all_count */



void count()
{
    all_count( 1 );
}


/*  ne_count  --  Don't echo this symbol!
*/
void ne_count()
{
    all_count( 0 );
}


int check_type()
{
    extern int   lu_type PARMS((  CONST char * ));
/*
* pseudo code --- this is what it should check
*
*       if (yytext == type_name)
*               return(TYPE_NAME);
*
*       return(IDENTIFIER);
*/

    /*  If it's a known typedef type, say OK */
    if( lu_type(yytext) )
        return( TYPE_NAME );

    return(IDENTIFIER);
}   /* check_type */


    void
inc_td_cnt( )
{
    ++td_nest;
}


    void
dec_td_cnt( )
{
    if( td_nest > 0 )
    {
        --td_nest;
        if( trip_hline < 0 && yytext[0] == CH_RCURLY )
            trip_hline = 1;
    }
}   /* dec_td_cnt */



/*	    See if correct context to have message parameter.  Eg "a=b?c:d"
//      is bad.  A "PARM_LABEL" states the identifier just spotted is
//      actually part of the method selector label.   The context to
//      catch this is inside a message.  We try to separate this from
//      the "?:" operator.
*/
    int
msg_parm()
{

    if( in_context & IC_MESSAGE )
    {
        ne_count();                        /* Text picked up by grammar */
        return( PARM_LABEL );            /* Good context */
    }

    unput_last();              /* Give back the colon */
    count();
    return( IDENTIFIER );
}   /* msg_parm */



/* ------------------  OCT Preprocess Helpers  ------------------------ */

/*   take_parm  --  Gobble text up until closing paren.  Used by OCT
//                  preprocessor functions.  Returns text pointer, or
//                  NULL if something wrong, like EOF...  Front-end to
//          scan_next_word() to ensure parameter is inside of parens.
*/
    char *
take_parm()
{
    int 	c;

    if( (c=scan_next_word()) == 0 || c != CH_CLOSE_PAREN )
        return NULL;

#if SW_WATCH
    printf( "(oct_preproc) <%s>", yytext );
#endif

    return( & scan_keep[0] );
}   /* take_parm */


/*   open_method  --  When an open method symbol is detect, it is only
//                	  relative outside any {} nesting.  If we're are
//                	  nested, then simply return the symbol.
*/
    int
open_method()
{
    int 	rc ;

    if( td_nest > 0 )
    {
        count();
        rc = yytext[0];        /* Return first char of line */
    } else
    {
        ne_count();
        if( yytext[0] == '+' )
        {
            m_order = 'F' ;
            rc = OPEN_FACTORY_METHOD;
        } else
        {
            m_order = 'I' ;
            rc = OPEN_INSTANCE_METHOD; 
        }
        in_state |= IS_METHOD_DEF;
    }   /* else if not nested.. */

    return( rc );
}   /* open_method */


/* -----------------------  Translator Directives  ---------------------- */


/*   ooc_public  --  Note that we saw public directive.  Call back to
 *                   grammar to change any state info.
 */
    void
ooc_public( )
{
    extern void 	see_public();

    see_public();
    ne_count();

}	/* ooc_public */


/*   ooc_protected  --  Note that we saw @protected.  Same as @public.
 */
    void
ooc_protected( )
{
    extern void 	see_protected();

    see_protected();
    ne_count();

}	/* ooc_protected */


/*   ooc_private  --  Note that we saw @private.  Same as @public.
 */
    void
ooc_private( )
{
    extern void 	see_private();

    see_private();
    ne_count();

}	/* ooc_private */


/*   ooc_selector  --  Handle "@selector( .. )".  Converts the following
//                	   text into an internal selector code.  The text
//                	   inside parens is converted and setup as the next
//                	   token to be read.  The "@selector(..)" text is
//            ignored by not returning (in scanner definitions area above).
*/
    void
ooc_selector( )
{
    register char	*p;

    if( take_parm() == NULL )
        return ;


    /*  Pack selector, convert to internal name, then `unput' it into
    //	lex's input buffer (treated as a stack).
    */
    strremove( scan_keep, ' ' );
    asCRef_selector( scan_keep );            /* Selector stored in 'buff' */


    /*	The preproc operation has been eaten up by us.  Now stuff the
    //	selector internal code the asCRef_selector() has stored in 'buff'
    //	for us.  By unput()'ing, this token will be read next time thru
    //	the loop of yylex().  This means the text will be examined by
    //	the scanner, and the array reference will appear as another
    //	sub-expression.  Can be used inside of msg exprs!
    */
    for( p=buff+strlen(buff) ; --p >= buff ; )
    {
        unput( *p );
        --column;
    }

#if SW_WATCH
    printf( " @sel(%s) --> {%s}\n", scan_keep, buff );
#endif

}   /* ooc_selector */


/*   ooc_defs  --  Handle the @defs() directive.  Search known classes
//                   for the instance variables is has.  The string is
//                   deposited directly into the output stream.
*/
    void
ooc_defs( )
{
    extern void 	do_td_def PARMS(( char *classname ));

    if( take_parm() != NULL )
    {
        /*  The class name is now in scan_keep[].  Inspect dataset for
        //	all instance variables.  Text is sent to 'yyout'.
        */
        strremove( scan_keep, ' ' );
        do_td_defs( scan_keep );
    }

    trip_hline = -1;	/* we'll need a resynch after closing brace */

}   /* ooc_defs */



    void
ooc_includes( )
{
    extern void   out_includes PARMS(( void ));
    extern void   out_externs PARMS(( int end_flag ));

    take_parm();
    out_includes();
    out_externs( 0 );
    hash_line( 0 );               /* Resynch */
}   /* ooc_includes */



    void
ooc_encode( )
{
    extern const char *encode_type PARMS(( const char * ));
    const char *enc;
    const char *p;

    if ( take_parm() == NULL )
        return;
    strremove( scan_keep, ' ' );
    enc = encode_type( scan_keep );
    /* Emit C string literal to yyout, escaping \ and " */
    fputc( '"', yyout );
    for ( p = enc ; *p != '\0' ; p++ )
    {
        if ( *p == '\\' || *p == '"' )
            fputc( '\\', yyout );
        fputc( (int)(unsigned char)*p, yyout );
    }
    fputc( '"', yyout );
    ne_count();
}   /* ooc_encode */


/*   ooc_alias  --  Parse @alias(real_selector, alias_selector) and record
 *                  the binding via mk_meth_alias() for method name resolution.
 */
    void
ooc_alias( )
{
    extern void mk_meth_alias PARMS(( const char *, const char * ));
    static char real_buf[ SIZ_BUFFER+2 ], alias_buf[ SIZ_BUFFER+2 ];
    char *p, *s;
    int len;

    if ( take_parm() == NULL )
        return;
    p = strchr( scan_keep, ',' );
    if ( p == NULL )
        return;
    s = scan_keep;
    while ( *s == ' ' || *s == '\t' )
        s++;
    len = (int)(p - s);
    while ( len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' ) )
        len--;
    if ( len >= (int)(sizeof(real_buf)) )
        len = (int)(sizeof(real_buf)) - 1;
    real_buf[0] = '\0';
    if ( len > 0 )
    {
        strncpy( real_buf, s, (unsigned)len );
        real_buf[len] = '\0';
    }
    s = p + 1;
    while ( *s == ' ' || *s == '\t' )
        s++;
    strncpy( alias_buf, s, sizeof(alias_buf) - 1 );
    alias_buf[ sizeof(alias_buf) - 1 ] = '\0';
    p = alias_buf + strlen( alias_buf );
    while ( p > alias_buf && (p[-1] == ' ' || p[-1] == '\t') )
        *--p = '\0';
    mk_meth_alias( real_buf, alias_buf );
    ne_count();
}   /* ooc_alias */


/* -----------------------  The Public Part  -------------------------- */


static char    set_unget_token = FALSE;   /* TRUE returns last token again */
static int     this_tok;                  /* Save just in case */


/*   get_tok  --  The main translator calls here to read another token.
//                As 'yytext' can be defined as an array or ptr, we point
//                to it using our own string ptr.  Maintains current
//                and previous token values.
*/
    int
get_tok()
{
    extern char    *curr_name;     /* String ptr to token */
    extern int     curr_tok;       /* Token read by last function */
    extern int     prev_tok;       /* Looking Back.. */
    extern int     pp_tok;         /* Previous previous token */

    if( curr_tok != DONE && !set_unget_token )
    {
        pp_tok   = prev_tok;
        prev_tok = curr_tok;
        this_tok = yylex();
    }
    curr_tok = this_tok;
    set_unget_token = FALSE;
    curr_name = & yytext[0];        /* Access in type-independent manner */

    return( curr_tok );
}   /* get_tok */


    void
unget_tok()
{
    set_unget_token = TRUE;    	/* Return current token once more */
}


/*   peek_tok  --  Non-destructive read of next token.  Never returns
 *                 curr_name; doesn't change curr_tok.
 */
    int
peek_tok()
{

    if( !set_unget_token )
    {
        set_unget_token = TRUE;
        this_tok = yylex();
    }
	return( this_tok );
}   /* peek_tok */


