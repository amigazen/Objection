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

/* Some customizable parameters.	*/

/* #define	MAX_CLASS_ENTRIES	1024  */
#define	MAX_CLASS_ENTRIES	96
#define	MAX_METHOD_ENTRIES	8096
#define	MAX_DIRS		10
#define MAX_DEFINES		10
#define COLUMNS_PER_PAGE	3

/* Non-customizable parameters.	*/
    
#define COMMENT_BUFSIZ		128
#define	ARGLIST_SIZE 		10

#define isidchr(c)	(isalnum(c) || (c == '_') || (c == '$'))

typedef	enum { false, true } boolean ;

typedef struct BackLink {
	struct BackLink *next;
	int index;
} BackLink;

typedef struct ClassNameEntry {
	char *class_name;
	struct {
		unsigned  defined:1;
		unsigned  traversed:1;
	} tag;
	int parent_class_index;
	struct BackLink *next;		/* For hierarchy printing	*/
} ClassNameEntry;

typedef struct CommentBuf {
	char buf[COMMENT_BUFSIZ];
	int n;
	struct CommentBuf * next;
} CommentBuf;

typedef struct IncludeBuf {
	char *buf;
	struct IncludeBuf * next;
} IncludeBuf;

typedef struct MethodNameEntry {
	char *method_name;
	struct BackLink *next;		/* For hierarchy printing	*/
} MethodNameEntry;

extern boolean PutHeader;
extern boolean MakeGlobalFile;

extern ClassNameEntry ClassNameTable[];
extern MethodNameEntry MethodNameTable[];
extern char * IncludeDir[];
extern int IncludeCount;
extern char * Defines[];
extern int DefinesCount;

extern char * ProgName;

/*----------------------------------------------------------------------*/

FILE * OpenIncludeFile();
int ReadFile();
void PrintClassHeader();
void PrintDescription();
void PrintMethod();
void PrintFunction();
void PrintComment();

void PrintClassHierarchy();
void DescendTree();
void PrintTree();
void PictureNewLine();
void InitializeTreePrint();
void TerminateTreePrint();

void PrintMethodIndex();
void PutLatexChar();
