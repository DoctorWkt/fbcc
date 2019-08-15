#ifndef _STDARG_H

#define _STDARG_H

/* gestion des fonctions � nombre de param�tres variable */
							
typedef char *va_list;

#define va_start(ap,last) ap=(char *)&(last); 
#define va_arg(ap,type) (ap-=sizeof (type), *(type *)(ap))
#define va_end(ap)

#endif

