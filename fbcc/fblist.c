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
#include <string.h>
#include "fbcc.h"


LIST *mk_int(int a,LIST *tl)
{
	 LIST *l;
	 l=malloc(sizeof(LIST));
	 l->type=LIST_INT;
	 l->data.val=a;
	 l->tl=tl;
	 return l;
}

LIST *mk_tag(int a,LIST *tl)
{
	 LIST *l;
	 l=malloc(sizeof(LIST));
	 l->type=LIST_TAG;
	 l->data.val=a;
	 l->tl=tl;
	 return l;
}

LIST *mk_list(LIST *list,LIST *tl)
{
	 LIST *l;
	 l=malloc(sizeof(LIST));
	 l->type=LIST_LIST;
	 l->data.list=list;
	 l->tl=tl;
	 return l;
}

LIST *mk_buf(char *buf,int buf_size,LIST *tl)
{
	 LIST *l;

	 l=malloc(sizeof(LIST)+buf_size-1);
	 l->type=LIST_STR;
	 l->data.buf_size=buf_size;
	 memcpy(l->str,buf,buf_size);
	 l->tl=tl;
	 return l;
}

LIST *mk_str(char *str,LIST *tl)
{
	 return mk_buf(str,strlen(str)+1,tl);
}

LIST *mk_sym(SYM *s,LIST *tl)
{
	 LIST *l;

	 l=malloc(sizeof(LIST));
	 l->type=LIST_SYM;
	 l->data.sym=s;
	 l->tl=tl;
	 return l;
}

int hd_tag(LIST *l)
{
	 if (l==NULL || l->type!=LIST_TAG) Error_Internal("'hd_tag': type incorrect");
	 return l->data.val;
}

int hd_int(LIST *l)
{
	 if (l==NULL || l->type!=LIST_INT) Error_Internal("'hd_int': type incorrect");
	 return l->data.val;
}

LIST *hd_list(LIST *l)
{
	 if (l==NULL || l->type!=LIST_LIST) Error_Internal("'hd_list': type incorrect");
	 return l->data.list;
}

char *hd_str(LIST *l)
{
	 if (l==NULL || l->type!=LIST_STR) Error_Internal("'hd_str': type incorrect");
	 return l->str;
}

SYM *hd_sym(LIST *l)
{
	 if (l==NULL || l->type!=LIST_SYM) Error_Internal("'hd_sym': type incorrect");
	 return l->data.sym;
}

void put_int(LIST *l,int a)
{
	 if (l->type!=LIST_INT) Error_Internal("'put_int': type incorrect");
	 l->data.val=a;
}

void put_tag(LIST *l,int a)
{
	 if (l->type!=LIST_TAG) Error_Internal("'put_tag': type incorrect");
	 l->data.val=a;
}

void put_list(LIST *l,LIST *a)
{
	 if (l->type!=LIST_LIST) Error_Internal("'put_tag': type incorrect");
	 l->data.list=a;
}



LIST *tl(LIST *l) 
{
	 if (l==NULL) Error_Internal("'tl' appelé sur une liste nulle");
	 return l->tl;
}

LIST *append(LIST *a,LIST *b)
{
	 LIST *t;
	 
	 t=a;
	 if (t==NULL) return b;
	 while (t->tl!=NULL) t=t->tl;
	 t->tl=b;
	 return a;
}

void list_free(LIST *l)
{
	 if (l==NULL) return;
	 if (l->type==LIST_LIST) list_free(l->data.list);
	 list_free(l->tl);
	 free(l);
}


LIST *list_dup(LIST *l)
{
	 if (l==NULL) return NULL;
	 switch (l->type) {
		case LIST_LIST:
			return mk_list(list_dup(l->data.list),list_dup(l->tl));
		case LIST_INT:
			return mk_int(l->data.val,list_dup(l->tl));
		case LIST_STR:
			return mk_buf(l->str,l->data.buf_size,list_dup(l->tl));
		case LIST_TAG:
			return mk_tag(l->data.val,list_dup(l->tl));
		case LIST_SYM:
			return mk_sym(l->data.sym,list_dup(l->tl));
		default:
			Error_Internal("'list_dup': type incorrect");
			return NULL;
	 }
}

char *tag_str[]=
{
	   "UNKNOWN",
		 "TYPE_VOID",
		 "TYPE_CHAR",
		 "TYPE_UCHAR",
		 "TYPE_SHORT",
		 "TYPE_USHORT",
	   "TYPE_INT",
		 "TYPE_UINT",
		 "TYPE_POINTER",
		 
		 "TYPE_FUNC",
		 "TYPE_ARRAY",
		 "TYPE_STRUCT",
		 "TYPE_UNION",
		 "TYPE_ENUM",
		 "TYPE_TYPEDEF",
		 
		 "QUALIF_CONST",
		 "QUALIF_VOLATILE",

		 "TYPE_VAR_IDENT",
		 "TYPE_TYPEDEF_IDENT",
		 "TYPE_UNSIGNED",
		 "TYPE_SIGNED",

		 "STORAGE_AUTO",
		 "STORAGE_REGISTER",
		 "STORAGE_STATIC",
		 "STORAGE_EXTERN",
		 "STORAGE_TYPEDEF",
		 "STORAGE_DEFAULT",

		 "SYM_VAR",
		 "SYM_TYPEDEF",
		 "SYM_STRUCT",
		 "SYM_UNION",
		 "SYM_ENUM",
		 "SYM_ENUM_CONST",

		 "VAR_STACK",
		 "VAR_DATA",
		 "VAR_CODE",

		 "FUNC_NEW",
		 "FUNC_ELLIPSIS",
		 "FUNC_OLD",

		 "EXPR_INT",
		 "EXPR_STR",

		 "EXPR_IDENT",
		 "EXPR_CAST",

		 "EXPR_CALL",
		 "EXPR_INDIR",
		 "EXPR_ADDR",
		 "EXPR_ASSIGN",

		 "EXPR_ADD",
		 "EXPR_SUB",
		 "EXPR_MUL",
		 "EXPR_DIV",
		 "EXPR_MOD",
		 "EXPR_NEG",
		 "EXPR_PLUS",

		 "EXPR_AND",
		 "EXPR_OR",
		 "EXPR_XOR",
		 "EXPR_NOT",
		 "EXPR_SHR",
		 "EXPR_SHL",
		 
		 "EXPR_LT",
		 "EXPR_LE",
		 "EXPR_EQ",
		 "EXPR_GE",
		 "EXPR_GT",
		 "EXPR_NE",
		 
		 "EXPR_LAND",
		 "EXPR_LOR",
		 "EXPR_LNOT",

		 "EXPR_LIST",
		 "EXPR_COND",

		 "INIT_EXPR",
		 "INIT_LIST",
};


void List_Print(LIST *l) 
{
	 printf("( ");
	 while (l!=NULL) {
			switch(l->type) {
			 case LIST_LIST:
				 List_Print(l->data.list);
				 break;
			 case LIST_INT:
				 printf("%d",l->data.val);
				 break;
			 case LIST_STR:
				 printf("'%s'",l->str);
				 break;
			 case LIST_TAG:
				 printf("%s",tag_str[l->data.val]);
				 break;
			 case LIST_SYM:
				 printf("[%s]",l->data.sym->str); 
				 break;
			}
			printf(" ");
			l=l->tl;
	 }
	 printf(")");
}

								
								
								
