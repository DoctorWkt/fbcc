
#ifndef _STRING_H

#define _STRING_H

char * strcpy(char * dest,const char *src);
char * strncpy(char * dest,const char *src,size_t count);
char * strcat(char * dest, const char * src);
char * strncat(char *dest, const char *src, size_t count);
int strcmp(const char * cs,const char * ct);
int strncmp(const char * cs,const char * ct,size_t count);
char * strchr(const char * s,char c);
size_t strlen(const char * s);
size_t strnlen(const char * s, size_t count);
size_t strspn(const char *s, const char *accept);
char * strpbrk(const char * cs,const char * ct);
char * strtok(char * s,const char * ct);
void * memset(void * s,char c,size_t count);
char * bcopy(const char * src, char * dest, int count);
void * memcpy(void * dest,const void *src,size_t count);
void * memmove(void * dest,const void *src,size_t count);
int memcmp(const void * cs,const void * ct,size_t count);
void * memscan(void * addr, unsigned char c, size_t size);


#endif
