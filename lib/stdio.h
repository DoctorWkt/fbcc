
#include <stdlib.h>
#include <stdarg.h>

#ifndef _STDIO_H

#define _STDIO_H

#define EOF (-1)

typedef struct _FILE FILE;

extern FILE *stdin,*stdout,*stderr;

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char * buf, const char *fmt, ...);

void vfprintf(FILE *f,const char *fmt,va_list args);
void fprintf(FILE *,const char *fmt,...);

void printf(const char *fmt, ...);

FILE *fopen(char *,char *);
int fclose(FILE *);
int fwrite(void *,size_t,size_t,FILE *);
int fread(void *,size_t,size_t,FILE *);
int fgetc(FILE *);
int fputc(int,FILE *);
int ferror(FILE *);

#define getc(f) fgetc(f)
#define putchar(c) fputc(c,stdout)
#define getchar() fgetc(stdin)
#endif
