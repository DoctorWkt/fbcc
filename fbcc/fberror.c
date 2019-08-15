/*
 *  FBCC - A simple C compiler.
 * 
 *  Copyright (c) 1996 Fabrice Bellard
 *
 *  Contact addresses:
 *  mail: Fabrice Bellard, 451 chemin du mas de Matour, 34790 Grabels, France
 *  email: bellard@email.enst.fr
 *  url: http://www.enst.fr/~bellard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "fbcc.h"

int debug_print_expr=0;

int errors_nb;
int line_current;

void Error(char *format,...)
{
	 va_list ap;
	 
	 errors_nb++;
	 fprintf(stderr,"Error #%d line %d: ",errors_nb,line_current);
	 
	 va_start(ap,format);
	 vfprintf(stderr,format,ap);
	 va_end(ap);
	 
	 fprintf(stderr,"\n");
	 exit(1);
}

void Warning(char *format,...)
{
	 va_list ap;
	 
	 errors_nb++;
	 fprintf(stderr,"Warning line %d: ",line_current);
	 
	 va_start(ap,format);
	 vfprintf(stderr,format,ap);
	 va_end(ap);
	 
	 fprintf(stderr,"\n");
}


void yyerror(char *str)
{
	 Error(str);
}

void Error_Internal(char *format,...)
{
	 va_list ap;

	 fprintf(stderr,"Internal error line %d: ",line_current);

	 va_start(ap,format);
	 vfprintf(stderr,format,ap);
	 va_end(ap);
	 
	 fprintf(stderr,"\n");
	 exit(1);
}

void ddprintf(char *format,...)
{
	 va_list ap;
	 
	 va_start(ap,format);
	 vfprintf(stderr,format,ap);
	 va_end(ap);

}
