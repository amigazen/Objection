/*    STRING.H    String stuff (ANSI prototypes for SAS/C).    */

#ifndef _STRING_H
#define _STRING_H 1

#ifndef EOS
#define EOS '\0'
#endif

char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strncat(char *dest, const char *src, unsigned long n);
char *strncpy(char *dest, const char *src, unsigned long n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned long n);
unsigned long strlen(const char *s);

char *strstr(const char *haystack, const char *needle);

void *memset(void *s, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);

void movmem(const void *src, void *dest, unsigned long cnt);
void setmem(void *s, int c, unsigned long cnt);
void swapmem(void *s1, void *s2, unsigned long cnt);

#endif
