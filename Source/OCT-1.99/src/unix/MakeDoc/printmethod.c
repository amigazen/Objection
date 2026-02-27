/************************************************************************/
/* Copyright the Real-Time and Distributed Systems Group,		*/
/* Department of Systems and Computer Engineering,			*/
/* Carleton University, Ottawa, Ontario, Canada. K1S 5B6		*/
/*									*/
/* October 1989.							*/
/* 									*/
/*									*/
/* This software is published on an as-is basis. There is ABSOLUTELY NO	*/
/* WARRANTY for any part of this software to work correctly or as	*/
/* described in the manuals. We do not accept any liability for any	*/
/* kind of damage caused by use of this software, such as loss of data,	*/
/* time, money, or effort.						*/
/*									*/
/* Permission is granted to use, copy, modify, or distribute any part	*/
/* of this software as long as this is done without asking for		*/
/* charge, and provided that this copyright notice is retained as	*/
/* part of the source files. You may charge a distribution fee for	*/
/* the physical act of transferring a copy, and you may at your		*/
/* option offer warranty protection in exchange for a fee.		*/
/*									*/
/* Please send comments or suggestions to:				*/
/*	Greg Franks							*/
/* 	Department of Systems and Computer Engineering,			*/
/* 	Carleton University, Ottawa, Ontario, Canada. K1S 5B6		*/
/*	(613) 788-5736							*/
/*									*/
/*	BITNET: greg@sce.carleton.ca					*/
/*	UUCP: uunet!mitel!sce!greg					*/
/************************************************************************/

/*
 * $Source: /usr/sirius/greg/src/makedoc/RCS/printmethod.c,v $
 *	Print out the method cross reference.
 */

#ifndef lint
static char	*RCSid = "$Header: /usr/sirius/greg/src/makedoc/RCS/printmethod.c,v 3.2 89/10/04 20:53:46 greg Exp Locker: greg $";
#endif

/*
 * $Log:	printmethod.c,v $
 * Revision 3.2  89/10/04  20:53:46  greg
 * Print comments properly.  Flush tex commands upon arrival.
 * 
 * Revision 3.1  89/10/04  10:15:46  greg
 * Print out method index.  This scheme doesn't work.
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#ifdef USG
#include <string.h>
#endif
#include "makedoc.h"

int compare();
void TagClasses();
void DescendTreeAndTag();
void PrintClasses();
void MethodDescendTree();

void
PrintMethodIndex( fptr )
FILE * fptr;
{
	int i;
	BackLink *p;

	/* Sort everything except for the "root" entry.	*/
	
	qsort( (char *)&MethodNameTable[1], MAX_METHOD_ENTRIES-1, sizeof( struct MethodNameEntry ), compare );

#ifdef	DEBUG
 	(void) fprintf( fptr, "\\documentstyle[11pt]{report}\n\\begin{document}\n" );
	(void) fprintf( fptr, "\\newlength{\\mywidth} \\newlength{\\mylength}\n" );
#endif	DEBUG
	(void) fprintf( fptr, "\\begin{description}\n" );

	for ( i = 1; i < MAX_METHOD_ENTRIES &&  MethodNameTable[i].method_name; ++i ) {
		(void) fprintf( fptr, "\\item[%s]\n\\mbox{}\n\n", MethodNameTable[i].method_name );
		p = MethodNameTable[i].next;
		if ( p ) { 
			TagClasses( p, true );
			PrintClasses( fptr, p );
			TagClasses( p, false );
		}
	}
	(void) fprintf( fptr, "\\end{description}\n" );
	
#ifdef	DEBUG
 	(void) fprintf( fptr, "\\end{document}\n" );
#endif	DEBUG
}




/*
 * compare
 *	A fancy string compare that ensures that it has valid
 *	pointers.  Null pointers compare higher.
 */

int
compare( e1, e2 )
register MethodNameEntry *e1, *e2;
{
	if ( e1->method_name != 0 && e2->method_name != 0 ) {
		return strcmp( e1->method_name, e2->method_name );
	} else if ( e1->method_name != 0 ) {
		return -1;
	} else if ( e2->method_name != 0 ) {
		return 1;
	} else {
		return 0;
	}
}



/*
 * TagClasses
 *	Set or clear the tag field of all of the sub-classes of all of the
 *	classes indicated by the list pointer 'p'.
 */

void
TagClasses( p, arg )
register BackLink *p;
boolean arg;
{
	for( ; p; p = p->next ) {
		ClassNameTable[p->index].tag.defined = (unsigned)arg;
		DescendTreeAndTag( p->index, arg );
	}
}



void
DescendTreeAndTag( index, arg )
int index;
boolean arg;
{
	register BackLink * p;
	
	for ( p = ClassNameTable[index].next; p; p = p->next ) {
		ClassNameTable[p->index].tag.traversed = (unsigned)arg;
		DescendTreeAndTag( p->index, arg );
	}
}



void
PrintClasses( fptr, p )
FILE *fptr;
register BackLink *p;
{
	(void) fprintf( fptr, "\\begin{description}\n" );
	for( ; p; p = p->next ) {
		if ( !ClassNameTable[p->index].tag.traversed ) {
			MethodDescendTree( fptr, p->index, 0 );
		}
		(void) fprintf( fptr, "\n" );
	}
	(void) fprintf( fptr, "\\end{description}\n" );
}


boolean PrintComma	= false;

void
MethodDescendTree( fptr, index, level )
FILE * fptr;
int index;
int level;
{
	register BackLink * p;
	
	if ( level == 0 ) {
		(void) fprintf( fptr, "\\item[{\\em %s\\/}]\n\\mbox{}\n\n", ClassNameTable[index].class_name );
		PrintComma = false;
	} else if ( ClassNameTable[index].tag.defined ) {
		(void) fprintf( fptr, "%s%s", PrintComma ? ", " : "", ClassNameTable[index].class_name );
		PrintComma = true;
	} 

	level = level + 1;
	p = ClassNameTable[index].next;
	while ( p ) {
		MethodDescendTree( fptr, p->index, level );
		p = p->next;
	}
}
