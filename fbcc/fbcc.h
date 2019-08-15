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
 * table of symbols
 */

#include <fbvmspec.h>


extern char *tag_str[];

enum {
	   LIST_LIST=1,
		 LIST_INT,
		 LIST_STR,
		 LIST_TAG,
		 LIST_SYM,
};

typedef struct _LIST {
	 int type;
	 struct _LIST *tl;
	 union {
			struct _LIST *list;
			int val;
			int buf_size;
			struct _SYM *sym;
	 } data;
	 char str[1];
} LIST;

typedef LIST *PLIST;

LIST *mk_int(int a,LIST *tl);
LIST *mk_tag(int a,LIST *tl);
LIST *mk_buf(char *buf,int buf_size,LIST *tl);
LIST *mk_list(LIST *list,LIST *tl);
LIST *mk_sym(struct _SYM *s,LIST *tl);
LIST *mk_str(char *str,LIST *tl);

int hd_tag(LIST *l);
int hd_int(LIST *l);
LIST *hd_list(LIST *l);
char *hd_str(LIST *l);
struct _SYM *hd_sym(LIST *l);

void put_int(LIST *l,int a);
void put_tag(LIST *l,int a);
void put_list(LIST *l,LIST *a);

LIST *list_dup(LIST *l);
LIST *append(LIST *a,LIST *b);
void list_free(LIST *l);
LIST *tl(LIST *l);


void List_Print(LIST *l);

#define YYSTYPE PLIST

void DeclareVars(LIST *spec,LIST *decl);



/* fbsym.c */

#define SYM_HASH_SIZE 1031

#define SYM_LENMAX 64

enum {
	   TABLE_VAR=1,    /* table des symboles pour les variables */
		 TABLE_LABEL,    /* labels pour goto */
		 TABLE_STRUCT,   /* structures/unions/enums */
};

typedef struct _SYM {
	 char str[SYM_LENMAX];
	 int sym_table;

	 struct _SYM *block_next;
	 struct _SYM *hash_next;
	 struct _SYM **hash_prev;
	 struct _BLOCK *block;

	 struct _LIST *list;
} SYM;


enum {
	   BLOCK_WHILE=1,
		 BLOCK_SWITCH,
		 BLOCK_IF,
		 BLOCK_DECL,
		 BLOCK_GLOBAL,
};

typedef struct _BLOCK {
	 int type;
   int label_break;
	 int label_continue;
	 int label_restart;     /* pour le début des boucles 'do' et 'for' */
	 
	 LIST *switch_values;

	 int local_var_offset;
	 struct _SYM *sym_first;
	 struct _BLOCK *dad;
} BLOCK;

extern BLOCK *block_global;
extern BLOCK *block_function; 
extern BLOCK *block_current;  /* bloc courant */
extern BLOCK *block_decl;     /* bloc courant pour les déclarations */

void Sym_Init(void);
void Sym_Print(void);

SYM *Sym_Search(char *str,int sym_table);
SYM *Sym_New1(char *str,int sym_table,BLOCK *b,LIST *l);
void Sym_Free(SYM *s);

void Sym_NewName(char *str);
SYM *Sym_New(char *str,int sym_table,LIST *l);
SYM *Sym_Create(char *str,int sym_table,LIST *l);

void Block_Enter(int type);
void Block_Leave(void);


/* fberror.c */

extern int line_current;
void Error_Internal(char *format,...);
void Error(char *format,...);
void ddprintf(char *format,...);
void Warning(char *format,...);

/* fbtype.c */

LIST *Type_ArrayDeclare(LIST *type,LIST *expr);
LIST *Type_FuncDeclare(int func_type,LIST *var_list);

LIST *Type_VarList(LIST *spec,LIST *decl_list);
LIST *Struct_Declare(char *str,int type,LIST *var_list);
LIST *Struct_Use(char *str,int type);
void Var_Declare(LIST *var_list);
void Func_Declare(LIST *spec,LIST *decl,LIST *var_list);
void Func_End(void);

int Type_Size(LIST *type);


LIST *Expr_List(LIST *expr_list);
LIST *Expr_Binary(int op,LIST *e1,LIST *e2);
LIST *Expr_Indir(LIST *e);
LIST *Expr_Index(LIST *tab,LIST *index);
LIST *Expr_Field(LIST *e,char *str);
LIST *Expr_AssignOp(int op,LIST *e1,LIST *e2);
LIST *Expr_Assign(LIST *e1,LIST *e2);
LIST *Expr_Addr(LIST *e);
LIST *Expr_Call(LIST *e1,LIST *expr);
LIST *Expr_Ident(char *str);
LIST *Expr_ConstInteger(int c);
LIST *Expr_Unary(int op,LIST *e1);
LIST *Expr_Sizeof(LIST *e,int is_expr);
LIST *Expr_Cast(LIST *new_type,LIST *e1);
LIST *Expr_Cond(LIST *e,LIST *e1,LIST *e2);

/* fbgen.c */

void Gen_Code(int c);
int Gen_NewLabel(void);
void Gen_Label(int c);

void Gen_Expr(LIST *e);
void Gen_Instr(LIST *instr);

void Gen_Int(unsigned int c);





/* fblist.c */

enum {
		 TYPE_VOID=1,
		 TYPE_CHAR,
		 TYPE_UCHAR,
		 TYPE_SHORT,
		 TYPE_USHORT,
	   TYPE_INT,
		 TYPE_UINT,
		 TYPE_POINTER,
		 
		 
		 TYPE_FUNC,
		 TYPE_ARRAY,
		 TYPE_STRUCT,
		 TYPE_UNION,
		 TYPE_ENUM,
		 TYPE_TYPEDEF,
		 
		 QUALIF_CONST,
		 QUALIF_VOLATILE,

		 TYPE_VAR_IDENT,
		 TYPE_TYPEDEF_IDENT,
		 TYPE_UNSIGNED,
		 TYPE_SIGNED,
		 
		 STORAGE_AUTO,
		 STORAGE_REGISTER,
		 STORAGE_STATIC,
		 STORAGE_EXTERN,
		 STORAGE_TYPEDEF,
		 STORAGE_DEFAULT,
		 
		 SYM_VAR,
		 SYM_TYPEDEF,
		 SYM_STRUCT,
		 SYM_UNION,
		 SYM_ENUM,
		 SYM_ENUM_CONST,

		 VAR_STACK,
		 VAR_DATA,
		 VAR_CODE,
		 
		 FUNC_NEW,
		 FUNC_ELLIPSIS,
		 FUNC_OLD,

		 EXPR_INT,
		 EXPR_STR,

		 EXPR_IDENT,
		 EXPR_CAST,
		 
		 EXPR_CALL,
		 EXPR_INDIR,
		 EXPR_ADDR,
		 EXPR_ASSIGN,

		 EXPR_ADD,
		 EXPR_SUB,
		 EXPR_MUL,
		 EXPR_DIV,
		 EXPR_MOD,
		 EXPR_NEG,
		 EXPR_PLUS,
		 
		 EXPR_AND,
		 EXPR_OR,
		 EXPR_XOR,
		 EXPR_NOT,
		 EXPR_SHR,
		 EXPR_SHL,
		 
		 EXPR_LT,
		 EXPR_LE,
		 EXPR_EQ,
		 EXPR_GE,
		 EXPR_GT,
		 EXPR_NE,
		 
		 EXPR_LAND,
		 EXPR_LOR,
		 EXPR_LNOT,

		 EXPR_LIST,
		 EXPR_COND,

		 INIT_EXPR,
		 INIT_LIST,
};

/* lexer - parser */

#define STRING_SIZE_MAX 256

extern char lex_string[STRING_SIZE_MAX];
extern int lex_string_size;

int yylex(void);
int yyparse(void);
void yyerror(char *format);
void Lex_Integer(int *sym_ptr,int *num_ptr,char *str,int base);
void Lex_AddString(int c);
int Lex_CharEsc(char *str);


/* génération de code */

extern int enum_val;

/* numéro de référence pour les variables globales */
extern int global_var_num;

/* allocation des variables locales */
extern int local_var_size;
extern SYM *local_var_sym;

void Gen_LI(int c,int data,SYM *s);

void Gen_InstrWhile1(LIST *expr);
void Gen_InstrWhile2(void);

void Gen_InstrDo1(void);
void Gen_InstrDo2(LIST *expr);

void Gen_InstrFor1(LIST *expr1,LIST *expr2);
void Gen_InstrFor2(LIST *expr3);

void Gen_InstrSwitch1(LIST *expr);
void Gen_InstrSwitch2(void);

void Gen_InstrBreak(void);
void Gen_InstrContinue(void);
void Gen_InstrGoto(LIST *ident);
void Gen_InstrReturn(LIST *expr);
void Gen_InstrLabel(LIST *ident);
void Gen_InstrExpr(LIST *expr);
void Gen_InstrCase(LIST *expr);

void Gen_InstrIf1(LIST *expr);
void Gen_InstrIf2(void);
void Gen_InstrIfElse2(void);
void Gen_InstrIfElse3(void);

LIST *Expr_ConstEval(LIST *expr);

extern int debug_print_expr;
