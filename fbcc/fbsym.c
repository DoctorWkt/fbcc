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

BLOCK *block_global;
BLOCK *block_current;
BLOCK *block_function;

SYM *sym_hash[SYM_HASH_SIZE];

int hash_func(char *str,int sym_table)
{
	 char *p;
	 int h;
  
	 p=str;
	 h=sym_table % SYM_HASH_SIZE;
	 while (*p!=0) {
			h=((h<<8)+(*p)) % SYM_HASH_SIZE;
			p++;
	 }
	 return h;
}

SYM *Sym_Search(char *str,int sym_table)
{
	 int h;
	 SYM *p;
	 
	 h=hash_func(str,sym_table);
	 p=sym_hash[h];
	 while (p!=NULL) {
			if ( strcmp(p->str,str) == 0 && p->sym_table == sym_table ) return p;
			p=p->hash_next;
	 }
	 return NULL;
}

/*
 * Création d'un nouveau symbole dans un bloc
 */

SYM *Sym_New1(char *str,int sym_table,BLOCK *b,LIST *l)
{
	 SYM *s;
	 int h;
	 
	 s=malloc(sizeof(SYM));
	 
	 strcpy(s->str,str);
	 
	 h=hash_func(s->str,sym_table);
	 s->hash_next=sym_hash[h];
	 if (s->hash_next!=NULL) s->hash_next->hash_prev=&s->hash_next;
	 sym_hash[h]=s;
	 s->hash_prev=&sym_hash[h];

	 s->block_next=b->sym_first;
	 b->sym_first=s;
	 s->block=b;
	 
	 s->list=l;
	 s->sym_table=sym_table;
	 return s;
}


/*
 * Création d'un nouveau symbole dans le bloc de déclaration courant
 */

SYM *Sym_New(char *str,int sym_table,LIST *l)
{
	 return Sym_New1(str,sym_table,block_decl,l);
}


/* 
 * Retour d'un nouveau nom de symbole assurante son unicité
 */

int sym_noname_num;

void Sym_NewName(char *str)
{
	 sprintf(str,"@S%d",sym_noname_num);
	 sym_noname_num++;
}

/*
 * Effacement d'une entrée dans la table des symboles
 * On ne se préoccupe pas ici des problèmes de blocs
 */
void Sym_Free(SYM *s)
{
	 *s->hash_prev=s->hash_next;
	 if (s->hash_next != NULL) s->hash_next->hash_prev=s->hash_prev;
	 list_free(s->list);
	 free(s);
}



/*
 * Entree dans un bloc
 * On alloue une structure BLOCK
 */
void Block_Enter(int type)
{
	 BLOCK *b;
	 
	 b=malloc(sizeof(BLOCK));
	 b->dad=block_current;
	 b->type=type;
	 b->sym_first=NULL;
	 block_current=b;

}

/*
 * Sortie d'un bloc. On désalloue la structure ainsi que les symboles
 * associés
 */
void Block_Leave(void)
{
	 SYM *s,*s1;
	 BLOCK *b;

	 s=block_current->sym_first;
	 while (s!=NULL) {
			s1=s->block_next;
			Sym_Free(s);
			s=s1;
	 }
	 b=block_current->dad;
	 free(block_current);
	 block_current=b;
}


void Sym_Init(void)
{
	 int i;
	 for(i=0;i<SYM_HASH_SIZE;i++) sym_hash[i]=NULL;
	 sym_noname_num=0;
}

void Sym_Print(void)
{
	 SYM *s;
	 int i;
	 
	 printf("Table des symboles:\n");
	 for(i=0;i<SYM_HASH_SIZE;i++) {
			s=sym_hash[i];
			while (s!=NULL) {
				 printf("h=%d t=%d b=%p str=%s : ",i,s->sym_table,s->block_next,s->str);
				 List_Print(s->list);
				 printf("\n");
				 s=s->hash_next;
			}
	 }
}


/*
 * Création d'un nouveau symbole.
 * S'il existe déjà dans le bloc courant, on emet une erreur
 */

SYM *Sym_Create(char *str,int sym_table,LIST *l)
{
	 SYM *s;
	 s=Sym_Search(str,sym_table);
	 if (s!=NULL) {
			if (s->block==block_current) 
				Error("L'identificateur '%s' est déjà défini dans ce bloc",str);
	 }
	 return Sym_New(str,sym_table,l);
}

