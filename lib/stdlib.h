/* stdlib.h */

#ifndef _STDLIB_H

#define _STDLIB_H

#define NULL ((void *)0)

/* stdarg.h */						 
						 
/* il n'y a pas de type long */

#define long int
typedef int size_t;

void *malloc(int);
void free(void *);
void exit(int);
void *realloc(void *,size_t);
char *getenv(const char *name);
int atoi(char *s);

/* void *alloca(int); */
#define alloca malloc


#endif
