/*
 * stdio.h -- Minimal stub for OCT preprocessor and xlator build.
 * When oct runs cpp on .m files, cpp only sees -I paths passed to oct.
 * This file lets #include <stdio.h> succeed during translation.
 * Full ANSI prototypes for SAS/C and other strict compilers (no Warning 100).
 */

#ifndef _STDIO_H
#define _STDIO_H  1

typedef struct _FILE FILE;

#define stdin   ((FILE *)0)
#define stdout  ((FILE *)1)
#define stderr  ((FILE *)2)

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *buf, const char *format, ...);
int putchar(int c);
int putc(int c, FILE *stream);
int puts(const char *s);
int fgetc(FILE *stream);
int getchar(void);
char *fgets(char *buf, int n, FILE *stream);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fflush(FILE *stream);
long ftell(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int feof(FILE *stream);
int ferror(FILE *stream);
int ungetc(int c, FILE *stream);
void rewind(FILE *stream);
unsigned long fread(void *ptr, unsigned long size, unsigned long nmemb, FILE *stream);

#endif
