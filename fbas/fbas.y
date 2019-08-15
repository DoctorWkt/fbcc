%{

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
 /*
 * Un petit assembleur simplifié. 
 * (c) 1996 Fabrice Bellard
 * 
 * Il a été prévu au départ pour générer un code objet avec table des 
 * symboles. 
 * Ici, pour plus de simplicité, on génère directement un code exécutable
 * et on simule l'édition de lien par la concaténation des sources assembleur.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>

#include <fbvmspec.h>
#include <fbvminstr.h>

/* parsing */

#define PATH_SIZE 256	 
#define IDENT_SIZE_MAX 256
	 
int lex_linenum;
	 
void yyerror(char *format,...);
int yylex(void);	 

/* gestion des segments */

#define FBVM_MAGIC (('f' << 24)|('b' << 16)|('v' << 8)|'m')

#define SEG_NB 2

#define SEG_CONST -1	 
#define SEG_TEXT  0
#define SEG_DATA  1
#define SEG_STACK 2

#define SEG_SIZE_INC 4096
 
int seg_cur;
unsigned char *seg_data[SEG_NB];	 
int seg_maxsize[SEG_NB];
int seg_offset[SEG_NB];

void Seg_Init(void);	 
void Seg_PutByte(int c);
void Seg_PutSym(int data,char *sym);
	 
/* table des symboles */
	 
#define HASH_SIZE 1001	 

#define SYM_IMPORTED 0
#define SYM_EXPORTED 1
#define SYM_PRIVATE  2
	 
typedef struct _SYM_ENT {
	 char name[IDENT_SIZE_MAX];
	 int value;
	 int seg;    /* si le segment vaut SEG_CONST, la constante est absolue */
	 int sym_type;
	 int reloc_nb;
	 struct _SYM_ENT *hash_next;
	 struct _RELOC_ENT *reloc_first;
} SYM_ENT;

typedef struct _RELOC_ENT {	 
	 int seg;
	 int offset;
	 struct _RELOC_ENT *next;
} RELOC_ENT;
	 
SYM_ENT *hash_tab[HASH_SIZE];
	 
void Hash_Init(void);
SYM_ENT *Hash_New(char *name,int sym_type);
SYM_ENT *Hash_Search(char *name); 

void Sym_Declare(char *name,int seg,int val);
void Sym_NewPrivate(void);
		 
typedef union {
	 int lex_num;
	 char *lex_ident;
} YYSTYPEDEF;
	 
#define YYSTYPE YYSTYPEDEF
	 
%}

%token sym_text
%token sym_data
%token sym_byte
%token sym_globl
%token sym_int
%token sym_short
%token sym_zero
%token sym_align
%token sym_equ
%token sym_module

%token sym_opcode
%token sym_opcode1

%token sym_ident
%token sym_num
%%

source_assembleur:
  instruction
| source_assembleur instruction
;


instruction:
  sym_text {
		 seg_cur=SEG_TEXT;
	}

| sym_data {
	 seg_cur=SEG_DATA;
}

| sym_byte sym_num {
	 Seg_PutByte($2.lex_num);
}

| sym_short sym_num {
	 int c;
	 c=$2.lex_num;
#if VM_LITTLE_ENDIAN == 0
	 Seg_PutByte((c >> 8) & 0xFF);
	 Seg_PutByte(c & 0xFF);
#else
	 Seg_PutByte(c & 0xFF);
	 Seg_PutByte((c >> 8) & 0xFF);
#endif
}

| sym_int expr_int { }

| sym_zero sym_num {
	 int i,size;
	 size=$2.lex_num;
	 for(i=0;i<size;i++) Seg_PutByte(0);
}

| sym_align sym_num {
	 int align;
	 align=$2.lex_num;
	 while ((seg_offset[seg_cur] % align) != 0) Seg_PutByte(0);
}

| sym_equ sym_ident ',' sym_num {
	 /* définition d'un symbole absolu */
	 Sym_Declare($2.lex_ident,SEG_CONST,$4.lex_num);
	 free($2.lex_ident);
}

| sym_ident ':' {
	 /* définition d'un symbole relogeable */
	 Sym_Declare($1.lex_ident,seg_cur,seg_offset[seg_cur]);
	 free($1.lex_ident);
}

| sym_globl sym_ident {
	 /* marquage d'un symbole en global */
	 SYM_ENT *s;
	 s=Hash_Search($2.lex_ident);
	 if (s==NULL) yyerror("Symbole '%s' non défini",$2.lex_ident);
	 s->sym_type=SYM_EXPORTED;
	 free($2.lex_ident);
}

| sym_module {
	 /* effaçage des symboles privés */
	 Sym_NewPrivate();
}


| sym_opcode {
	 Seg_PutByte($1.lex_num);
}

| sym_opcode1 { 
	 Seg_PutByte($1.lex_num); 
} expr_int
;


expr_int:
  sym_num {							
		 Seg_PutSym($1.lex_num,NULL);
	}
| sym_ident {
		 Seg_PutSym(0,$1.lex_ident);
	   free($1.lex_ident);
	}
| sym_ident '+' sym_num { 	 
		 Seg_PutSym($3.lex_num,$1.lex_ident);
	   free($1.lex_ident);
	}
;

							
%%

#include "lex.yy.c"


/*
 * Gestion des symboles
 */

void Hash_Init(void)
{
	 int i;
	 
	 for(i=0;i<HASH_SIZE;i++) hash_tab[i]=NULL;
}

int Hash_Func(char *name)
{
	 unsigned int h;
	 unsigned char *p;
	 
	 h=0;
	 p=(unsigned char *)name;
	 while (*p != 0) {
			h=((h<<8)+(*p)) % HASH_SIZE;
			p++;
	 }
	 return h;
}

	 
SYM_ENT *Hash_New(char *name,int sym_type)
{
	 int h;
	 SYM_ENT *a;

	 a=malloc(sizeof(SYM_ENT));
	 strcpy(a->name,name);
	 a->reloc_first=NULL;
	 a->sym_type=sym_type;
	 a->reloc_nb=0;
	 
	 h=Hash_Func(a->name);
	 a->hash_next=hash_tab[h];
	 hash_tab[h]=a;
	 return a;
}


SYM_ENT *Hash_Search(char *name)
{
	 int h;
	 SYM_ENT *p;

	 h=Hash_Func(name);
	 p=hash_tab[h];
	 while (p!=NULL) {
			if (strcmp(p->name,name)==0) return p;
			p=p->hash_next;
	 }
	 return NULL;
}

void Reloc_New(SYM_ENT *s,int seg,int offset)
{
	 RELOC_ENT *e;
	 
	 s->reloc_nb++;
	 e=malloc(sizeof(RELOC_ENT));
	 e->next=s->reloc_first;
	 s->reloc_first=e;
	 e->seg=seg;
	 e->offset=offset;
}
	 
void Sym_Declare(char *name,int seg,int val)
{
	 SYM_ENT *s;
	 s=Hash_Search(name);
	 if (s!=NULL) {
			if (s->sym_type!=SYM_IMPORTED) yyerror("Redéfinition de '%s'",name);
			s->sym_type=SYM_PRIVATE;
	 } else {
			s=Hash_New(name,SYM_PRIVATE);
	 }
	 s->value=val;
	 s->seg=seg;
}

void Sym_NewPrivate(void)
{
	 int i;
	 SYM_ENT *s;
	 
	 for(i=0;i<HASH_SIZE;i++) {
			s=hash_tab[i];
			while (s!=NULL) {
				 if (s->sym_type==SYM_PRIVATE) {
						strcpy(s->name,".");
				 }
				 s=s->hash_next;
			}
	 }
}

			
/* segments */

void Seg_PutByte(int c)
{
	 if (seg_offset[seg_cur] == seg_maxsize[seg_cur]) {
			seg_maxsize[seg_cur]+=SEG_SIZE_INC;
			if (seg_data[seg_cur] == NULL) {
				 seg_data[seg_cur]=malloc(seg_maxsize[seg_cur]);
			} else {
				 seg_data[seg_cur]=realloc(seg_data[seg_cur],seg_maxsize[seg_cur]);
			}
	 }
	 seg_data[seg_cur][seg_offset[seg_cur]]=c;
	 seg_offset[seg_cur]++;
}

void Seg_PutSym(int data,char *sym_str)
{
	 SYM_ENT *s;
	 
	 if (sym_str!=NULL) {
			s=Hash_Search(sym_str);
			if (s==NULL) {
				 s=Hash_New(sym_str,SYM_IMPORTED);
			}
			Reloc_New(s,seg_cur,seg_offset[seg_cur]);
	 }

#if VM_LITTLE_ENDIAN == 0
	 Seg_PutByte((data >> 24) & 0xFF);
	 Seg_PutByte((data >> 16) & 0xFF);
	 Seg_PutByte((data >> 8 ) & 0xFF);
	 Seg_PutByte((data      ) & 0xFF);
#else
	 Seg_PutByte((data      ) & 0xFF);
	 Seg_PutByte((data >> 8 ) & 0xFF);
	 Seg_PutByte((data >> 16) & 0xFF);
	 Seg_PutByte((data >> 24) & 0xFF);
#endif	 
}
	 
void Seg_Init(void)
{
	 int i;
	 
	 for(i=0;i<SEG_NB;i++) {
			seg_offset[i]=0;
			seg_maxsize[i]=0;
			seg_data[i]=NULL;
	 }
}

void fput_i(FILE *f,unsigned int a)
{
	 fputc((a >>24) & 0xFF,f);
	 fputc((a >>16) & 0xFF,f);
	 fputc((a >> 8) & 0xFF,f);
	 fputc((a     ) & 0xFF,f);
}

#if VM_LITTLE_ENDIAN == 0

static void mput_i(unsigned char *p,unsigned int a)
{
	 p[0]=a >> 24;
	 p[1]=a >> 16;
	 p[2]=a >> 8;
	 p[3]=a;
}

static int mget_i(unsigned char *p)
{
	 return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + (p[3]);
}

#else 

static void mput_i(unsigned char *p,unsigned int a)
{
	 p[3]=a >> 24;
	 p[2]=a >> 16;
	 p[1]=a >> 8;
	 p[0]=a;
}

static int mget_i(unsigned char *p)
{
	 return (p[3]<<24) + (p[2]<<16) + (p[1]<<8) + (p[0]);
}

#endif
	 
/*
 * Ecriture sur disque d'un éxécutable
 */

void Seg_ExecWrite(char *filename,int stack_size)
{
	 FILE *f;
	 int i,adr;
	 SYM_ENT *s;
	 RELOC_ENT *ent;
	 unsigned char *buf;
	 int code_size,data_size;
	 int reloc_nb;
	 
	 /* alignement des segments */
	 for(i=0;i<2;i++) {
			seg_cur=i;
			while ((seg_offset[seg_cur] % VM_SEG_ALIGN) != 0) Seg_PutByte(0);
	 }
	 
	 code_size=seg_offset[0];
	 data_size=seg_offset[1];
	 reloc_nb=0;
	 
	 /* résolution des références */
	 for(i=0;i<HASH_SIZE;i++) {
			s=hash_tab[i];
			while (s!=NULL) {
				 if (s->sym_type == SYM_IMPORTED) 
					 yyerror("Symbole '%s' non résolu",s->name);
				 ent=s->reloc_first;
				 while (ent!=NULL) {
						if (s->seg!=SEG_CONST) reloc_nb++;
						buf=&seg_data[ent->seg][ent->offset];
						adr=mget_i(buf);
/*						printf("adr=%d v=%d\n",adr,s->value); */
						if (s->seg==SEG_DATA) adr+=code_size+s->value;
						else adr+=s->value;
						mput_i(buf,adr);
						
						ent=ent->next;
				 }
				 s=s->hash_next;
			}
	 }
	 
	 f=fopen(filename,"w");
	 fput_i(f,FBVM_MAGIC);
	 fput_i(f,reloc_nb);

	 fput_i(f,code_size+data_size);
	 fput_i(f,stack_size);
	 
	 for(i=0;i<SEG_NB;i++) {
			fwrite(seg_data[i],1,seg_offset[i],f);
	 }

	 /* table de relocation : on exporte seulement les symboles des
		* segments SEG_DATA et SEG_CODE 
		*/
	 for(i=0;i<HASH_SIZE;i++) {
			s=hash_tab[i];
			while (s!=NULL) {
				 if (s->seg != SEG_CONST) {
						ent=s->reloc_first;
						while (ent!=NULL) {
							 if (ent->seg==SEG_DATA) 
								 fput_i(f,code_size+ent->offset);
							 else
								 fput_i(f,ent->offset);
							 ent=ent->next;
						}
				 }
				 s=s->hash_next;
			}
	 }
	 
	 fclose(f);
}


void Sym_Print(int type)
{
	 int i;
	 SYM_ENT *s;
	 RELOC_ENT *ent;
	 
	 for(i=0;i<HASH_SIZE;i++) {
			s=hash_tab[i];
			while (s!=NULL) {
				 if (s->sym_type==type) {
						if (type!=SYM_IMPORTED)
							printf("%s: %d:0x%X @ ",s->name,s->seg,s->value);
						else
							printf("%s @ ",s->name);
						
						ent=s->reloc_first;
						while (ent!=NULL) {
							 printf("%d:0x%X ",ent->seg,ent->offset);
							 ent=ent->next;
						}
						printf("\n");
				 }
				 s=s->hash_next;
			}
	 }
}

void Seg_Verbose(void)
{
	 int i;
	 
	 for(i=0;i<SEG_NB;i++) {
			printf("Segment %d: size=%d\n",i,seg_offset[i]);
	 }
	 printf("\nPrivate Symbols:\n");
	 Sym_Print(SYM_PRIVATE);
	 printf("\nExported Symbols:\n");
	 Sym_Print(SYM_EXPORTED);
	 printf("\nImported Symbols:\n");
	 Sym_Print(SYM_IMPORTED);
}


/* présentation :) */	 
	 
void yyerror(char *format,...)
{
	 va_list ap;
	 
	 fprintf(stderr,"fbas: Error Line %d: ",lex_linenum);
	 
	 va_start(ap,format);
	 vfprintf(stderr,format,ap);
	 va_end(ap);
	 
	 fprintf(stderr,"\n");
	 exit(1);
}

void print_help(void)
{
	 printf("usage: fbas [-h] [-?] [-v] [-s stack_size] [-o exec_name]\n"
					"Assembler (version 1.00) (c) 1996 Fabrice Bellard\n"
					"(compiled for %s)\n"
					"\n",VM_ARCH_NAME
					);
}

int main(int argc,char *argv[])
{
	 int c;
	 char obj_filename[PATH_SIZE];
	 int verbose,stack_size;
	 
	 strcpy(obj_filename,"fba.out");
	 verbose=0;
	 stack_size=64 * 1024;
	 
	 while (1) {
			c=getopt(argc,argv,"h?vo:s:");
			if (c==-1) break;
			switch (c) {
			 case 'h':
			 case '?':
				 print_help();
				 exit(0);
				 break;
			 case 'o':
				 strcpy(obj_filename,optarg);
				 break;
			 case 'v':
				 verbose=1;
				 break;
			 case 's':
				 stack_size=atoi(optarg);
				 break;
			}
	 }

	 Seg_Init();
	 Hash_Init();
	 
	 lex_linenum=1;
	 yyparse();
	 
	 if (verbose) Seg_Verbose();
	 
	 Seg_ExecWrite(obj_filename,stack_size);

	 return 0;
}
