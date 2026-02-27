/*   STDLIB.H		Standard Library stuff (K&R,2Ed, pg 251-252)   */

/*     This header declares functions for number conversion, storeage
 *     allocation, and similar tasks.
 */

extern double 		atof();
extern int  		atoi();
extern long 		atol();

extern double       strtod();
extern long   		strtol();
extern long   		strtoul();

extern int 			rand();				/* 0..32767 random numbers */
extern void			srand();			/* Seed it */

extern char			*calloc();
extern char			*malloc();
#ifdef __STDC__
extern void			free();				/* ANSI always succeeds */
#endif
#ifdef AZTEC_C
extern int 			free();				/* Aztec returns 0 if OK */
#endif

extern void			abort();			/* Causes a signal :-) */
extern void			exit();
extern int          atexit();

extern int 			system();
extern char			*getenv();
extern void 		*bsearch();
extern void 		qsort();

extern int  		abs();
extern long 		labs();

/*	end of stdlib.h */
