/*   STDLIB.H   Standard Library (ANSI prototypes for SAS/C).   */

#ifndef _STDLIB_H
#define _STDLIB_H 1

double atof(const char *nptr);
int atoi(const char *nptr);
long atol(const char *nptr);

double strtod(const char *nptr, char **endptr);
long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);

int rand(void);
void srand(unsigned int seed);

void *calloc(unsigned long nmemb, unsigned long size);
void *malloc(unsigned long size);
void free(void *ptr);

void abort(void);
void exit(int status);
int atexit(void (*function)(void));

int system(const char *command);
char *getenv(const char *name);
void *bsearch(const void *key, const void *base, unsigned long nmemb,
    unsigned long size, int (*compar)(const void *, const void *));
void qsort(void *base, unsigned long nmemb, unsigned long size,
    int (*compar)(const void *, const void *));

int abs(int j);
long labs(long j);

#endif
