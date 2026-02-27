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
 * $Source: /usr/sirius/greg/src/makedoc/RCS/printclass.c,v $
 *	Print out the class cross reference.
 */

#ifndef lint
static char	*RCSid = "$Header: /usr/sirius/greg/src/makedoc/RCS/printclass.c,v 3.2 89/10/10 17:30:51 greg Exp Locker: greg $";
#endif

/*
 * $Log:	printclass.c,v $
 * Revision 3.2  89/10/10  17:30:51  greg
 * Print out standard function names.
 * Print out stuff to make indexes.
 * 
 * Revision 3.1  89/10/04  10:15:33  greg
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

/*----------------------------------------------------------------------*/

#define	PAGE_HEIGHT		540		/* Points */
#define	PAGE_WIDTH		390		/* Points */
#define	POINTS_PER_COLUMN	(PAGE_WIDTH/3)
#define	POINTS_PER_ROW		15
#define	INTERCOLUMN_SPACE	30

boolean PrintNewLine;		/* Control Variable	*/
int LineNumber;
struct  {
	int StartLineNumber;
	int EndLineNumber;
	boolean more;
} PageControl[COLUMNS_PER_PAGE];
int PageHeight		= PAGE_HEIGHT;

void
PrintClassHierarchy( fptr )
FILE * fptr;
{
#ifdef	DEBUG
 	(void) fprintf( fptr, "\\documentstyle[11pt]{report}\n\\begin{document}\n" );
#endif	DEBUG

	(void) fprintf( fptr, "\\newlength{\\mywidth} \\newlength{\\mylength}\n" );
	(void) fprintf( fptr, "\\chapter{Class Inheritance Hierarchy}\n" );

	InitializeTreePrint( fptr );

	DescendTree( fptr, 0, 0 );
	
	TerminateTreePrint( fptr );

#ifdef	DEBUG
 	(void) fprintf( fptr, "\\end{document}\n" );
#endif	DEBUG
}



void
InitializeTreePrint( fptr )
FILE * fptr;
{
	register int i;

	for ( i = 0; i < COLUMNS_PER_PAGE; ++i ) {
		PageControl[i].StartLineNumber = 0;
		PageControl[i].EndLineNumber   = 0;
		PageControl[i].more            = false;
	}
	LineNumber = 0;
	(void) fprintf( fptr, "\\begin{figure}[htbp]\\begin{picture}(%d,%d)(0,-%d)\n", PAGE_WIDTH, PAGE_HEIGHT, PAGE_HEIGHT );
}


void
TerminateTreePrint( fptr )
FILE * fptr;
{
	(void) fprintf( fptr, "\\end{picture}\\end{figure}\n" );
}



void
DescendTree( fptr, index, level )
FILE * fptr;
int index;
int level;
{
	register BackLink * p;
	
	if ( level % COLUMNS_PER_PAGE == 0 ) {
		p = ClassNameTable[index].next;
		if ( p ) {
			if ( LineNumber != 0 ) {
				PictureNewLine( fptr );
			}
			(void) fprintf( fptr, "\\settowidth{\\mywidth}{ %s }\n",ClassNameTable[index].class_name );
			(void) fprintf( fptr, "\\setlength{\\mylength}{0.5\\textwidth}\n" );
			(void) fprintf( fptr, "\\addtolength{\\mylength}{-0.5\\mywidth}\n" );
			(void) fprintf( fptr, "\\put(0,%d){", -LineNumber*POINTS_PER_ROW );
			(void) fprintf( fptr, "\\rule[3pt]{\\mylength}{2pt} %s \\rule[3pt]{\\mylength}{2pt}}\n", ClassNameTable[index].class_name );

			PictureNewLine( fptr );

			while( p ) {
				PrintTree( fptr, p->index, 0 );
				p = p->next;
			}
		}
		
	}
	
	level = level + 1;
	p = ClassNameTable[index].next;
	while ( p ) {
		DescendTree( fptr, p->index, level );
		p = p->next;
	}
}



static char *RuleStr	= "\\rule[3pt]{\\mylength}{0.5pt}";

void
PrintTree( fptr, index, column )
FILE * fptr;
int index;
int column;
{
	register BackLink * p;
	register ClassNameEntry *q;
	
	PrintNewLine = true;

	q = &ClassNameTable[index];
	p = q->next;

	if ( q->next ) {
		(void) fprintf( fptr, "\\settowidth{\\mywidth}{%s}\n", q->class_name );
		(void) fprintf( fptr, "\\setlength{\\mylength}{%dpt}\n", POINTS_PER_COLUMN-INTERCOLUMN_SPACE );
		(void) fprintf( fptr, "\\addtolength{\\mylength}{-\\mywidth}\n" );
	}

	(void) fprintf( fptr, "\\put(%d,%d){%s%s}\n",
		       column*POINTS_PER_COLUMN,
		       -LineNumber*POINTS_PER_ROW,
		       q->class_name,
		       (q->next) ? RuleStr: "" );
	
	PageControl[column].EndLineNumber = LineNumber;
	
	column = (column + 1) % COLUMNS_PER_PAGE;

	if ( column != 0 && p ) {
		PageControl[column].StartLineNumber = LineNumber;
		while ( p ) {
			(void) fprintf( fptr, "\\put(%d,%d){\\rule[3pt]{%dpt}{0.5pt}}\n",
				       column*POINTS_PER_COLUMN-INTERCOLUMN_SPACE,
				       -LineNumber*POINTS_PER_ROW, INTERCOLUMN_SPACE-2 );
			PageControl[column].more = (boolean)(p->next != 0);
			PrintTree( fptr, p->index, column );
			p = p->next;
		}
		if ( PageControl[column].EndLineNumber-PageControl[column].StartLineNumber > 0 ) {
			(void) fprintf( fptr, "\\put(%d,%d){\\line(0,-1){%d}}\n",
				       column*POINTS_PER_COLUMN-INTERCOLUMN_SPACE,
				       -PageControl[column].StartLineNumber*POINTS_PER_ROW+(POINTS_PER_ROW/4),
				       (PageControl[column].EndLineNumber-PageControl[column].StartLineNumber)*POINTS_PER_ROW );
		}
		PageControl[column].StartLineNumber = 0;
		PageControl[column].EndLineNumber   = 0;
	}
	if ( PrintNewLine ) {
		PictureNewLine( fptr );
	}
	PrintNewLine = false;
}



void
PictureNewLine( fptr )
FILE *fptr;
{
	register int column;
	int y;
	
	LineNumber = LineNumber + 1;

	if ( LineNumber * POINTS_PER_ROW >= PageHeight ) {
		PageHeight = PAGE_HEIGHT;
		for ( column = 1; column < COLUMNS_PER_PAGE; ++column ) {
			y = PageControl[column].more ? LineNumber : PageControl[column].EndLineNumber;
			if ( PageControl[column].EndLineNumber-PageControl[column].StartLineNumber > 0 ) {
				(void) fprintf( fptr, "\\put(%d,%d){\\line(0,-1){%d}}\n",
					       column*POINTS_PER_COLUMN-INTERCOLUMN_SPACE,
					       -PageControl[column].StartLineNumber*POINTS_PER_ROW+(POINTS_PER_ROW/4),
					       (y-PageControl[column].StartLineNumber)*POINTS_PER_ROW );
			}
			PageControl[column].StartLineNumber = PageControl[column].more ? -1 : 0;
			PageControl[column].EndLineNumber   = 0;
		}
		(void) fprintf( fptr, "\\end{picture}\\end{figure}\n" );
		(void) fprintf( fptr, "\\newpage\n" );
		(void) fprintf( fptr, "\\begin{figure}[htbp]\\begin{picture}(%d,%d)(0,-%d)\n", PAGE_WIDTH, PAGE_HEIGHT, PAGE_HEIGHT );

		LineNumber = 0;
	}
}


