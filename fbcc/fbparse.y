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
 #include "fbcc.h"
 #include <stdlib.h>
%}

/* structures des programmes */
%token 	   sym_if
%token		 sym_else
%token		 sym_while
%token		 sym_do
%token		 sym_for
%token		 sym_goto
%token		 sym_continue
%token		 sym_break
%token		 sym_return
%token		 sym_case
%token		 sym_default
%token		 sym_switch
%token		 sym_sizeof

/* types */
%token		 sym_void
%token		 sym_char
%token		 sym_short
%token		 sym_int
%token		 sym_long
%token		 sym_float
%token		 sym_double
%token		 sym_signed
%token		 sym_unsigned
%token		 sym_struct
%token		 sym_union
%token     sym_enum
		 
/* classes de stockage */
%token		 sym_auto
%token		 sym_register
%token		 sym_static
%token		 sym_extern
%token		 sym_typedef

/* qualificatifs de type */
%token		 sym_const
%token		 sym_volatile
		 
/* symboles divers */
%token		 sym_le
%token		 sym_ge
%token		 sym_eq
%token		 sym_ne
%token		 sym_inc
%token		 sym_dec
%token		 sym_land
%token		 sym_lor
%token		 sym_shl
%token		 sym_shr
%token		 sym_assign_add
%token		 sym_assign_sub
%token		 sym_assign_mul
%token		 sym_assign_div
%token		 sym_assign_mod
%token		 sym_assign_and
%token		 sym_assign_or
%token		 sym_assign_xor
%token		 sym_assign_shr
%token		 sym_assign_shl

%token     sym_three_points
%token     sym_arrow

/* constantes */
%token     sym_const_int
%token     sym_const_str
%token     sym_const_char

/* identificateurs */
%token		 sym_ident
%token     sym_typedef_ident

%%

unite_de_traduction:
  declaration_externe
| unite_de_traduction declaration_externe
;

declaration_externe:
  definition_de_fonction
| declaration
;

/* declarations de fonction */

definition_de_fonction:
	specificateur_de_declaration declarateur {
		 Func_Declare($1,$2,NULL);
  } instruction_composee {
     Func_End();
	}

| declarateur {
	   Func_Declare(NULL,$1,NULL);
  } instruction_composee {
		 Func_End();
	}
|	specificateur_de_declaration declarateur liste_de_declarations_fonction {
		 Func_Declare($1,$2,$3);
  } instruction_composee {
     Func_End();
	}

| declarateur liste_de_declarations_fonction {
	   Func_Declare(NULL,$1,$2);
  } instruction_composee {
		 Func_End();
	}
;

liste_de_declarations_fonction:
  declaration_fonction { $$=$1; }
| liste_de_declarations_fonction declaration_fonction { $$=append($1,$2); }
;
	 
declaration_fonction:
  specificateur_de_declaration liste_de_declarateurs_init ';' {
	 $$=Type_VarList($1,$2);
	}
;

/* declarations de variables */
	 
liste_de_declarations:
  declaration
| liste_de_declarations declaration
;

declaration:
  specificateur_de_declaration ';' {
		 list_free($1);
	}
| specificateur_de_declaration liste_de_declarateurs_init ';' {
	 LIST *vars;
	 vars=Type_VarList($1,$2);
	 Var_Declare(vars);
	 list_free(vars);
	}
;

specificateur_de_declaration:
  specificateur_de_classe_de_stockage { $$=$1; }
| specificateur_de_classe_de_stockage specificateur_de_declaration { $$=append($1,$2); }

| specificateur_de_type { $$=$1; }
| specificateur_de_type specificateur_de_declaration { $$=append($1,$2); }

| qualificatif_de_type { $$=$1; }
| qualificatif_de_type specificateur_de_declaration { $$=append($1,$2); }
;

specificateur_de_classe_de_stockage :
  sym_auto { $$=mk_tag(STORAGE_AUTO,NULL); }
| sym_register { $$=mk_tag(STORAGE_REGISTER,NULL); }
| sym_static { $$=mk_tag(STORAGE_STATIC,NULL); }
| sym_extern { $$=mk_tag(STORAGE_EXTERN,NULL); }
| sym_typedef { $$=mk_tag(STORAGE_TYPEDEF,NULL); }
;

specificateur_de_type:
  sym_void   { $$=mk_tag(TYPE_VOID,NULL); }
| sym_char   { $$=mk_tag(TYPE_CHAR,NULL); }
| sym_short  { $$=mk_tag(TYPE_SHORT,NULL); }
| sym_int    { $$=mk_tag(TYPE_INT,NULL); }
| sym_signed { $$=mk_tag(TYPE_SIGNED,NULL); }
| sym_unsigned { $$=mk_tag(TYPE_UNSIGNED,NULL); }
| specificateur_de_struct { $$=$1; }
| specificateur_enumeration { $$=$1; }
| sym_typedef_ident { $$=mk_tag(TYPE_TYPEDEF_IDENT,$1); }
;

qualificatif_de_type:
  sym_const { $$=mk_tag(QUALIF_CONST,NULL); }
| sym_volatile { $$=mk_tag(QUALIF_VOLATILE,NULL); }
;

liste_de_declarateurs_init:
  declarateur_init { $$=$1; }
| liste_de_declarateurs_init ',' declarateur_init { $$=append($1,$3); }
;

declarateur_init:
  declarateur { $$=mk_list($1,mk_list(NULL,NULL)); }
| declarateur '=' initialisateur { $$=mk_list($1,mk_list($3,NULL)); }
;

/* structs et unions */

struct_ou_union:
  sym_struct { $$=mk_tag(TYPE_STRUCT,NULL); }
| sym_union { $$=mk_tag(TYPE_UNION,NULL); }
;

specificateur_de_struct:
  struct_ou_union '{' liste_de_declarations_de_struct '}' {
		 $$=Struct_Declare(NULL,hd_tag($1),$3);
		 list_free($1);
		 list_free($3);
	}
| struct_ou_union sym_ident '{' liste_de_declarations_de_struct '}' {
	 $$=Struct_Declare(hd_str($2),hd_tag($1),$4);
	 list_free($1);
	 list_free($2);
	 list_free($4);
}
| struct_ou_union sym_ident {
		 $$=Struct_Use(hd_str($2),hd_tag($1));
		 list_free($1);
		 list_free($2);
}
;

liste_de_declarations_de_struct:
  declaration_de_struct { $$=$1; }
| liste_de_declarations_de_struct declaration_de_struct {
	 $$=append($1,$2);
}
;


declaration_de_struct:
  liste_de_qualificatifs_de_specificateurs 
       liste_de_declarateurs_de_struct ';' {
			 $$=Type_VarList($1,$2);
		}
;

liste_de_qualificatifs_de_specificateurs:
  specificateur_de_type { $$=$1; }
| specificateur_de_type liste_de_qualificatifs_de_specificateurs { $$=append($1,$2); }
| qualificatif_de_type { $$=$1; }
| qualificatif_de_type liste_de_qualificatifs_de_specificateurs { $$=append($1,$2); }
;	 

/* nous avons supprimé les champs de bits */
liste_de_declarateurs_de_struct:
  declarateur { $$=mk_list($1,mk_list(NULL,NULL)); }
| liste_de_declarateurs_de_struct ',' declarateur { $$=append($1,mk_list($3,mk_list(NULL,NULL))); }
;

/* enums */

specificateur_enumeration:
  sym_enum '{' { 
		 enum_val=0;
	} liste_enumerateurs_virgule '}' {
		 $$=Struct_Declare(NULL,TYPE_ENUM,NULL);
	}

| sym_enum sym_ident '{' { 
	 enum_val=0;
} liste_enumerateurs_virgule '}' {
	 $$=Struct_Declare(hd_str($2),TYPE_ENUM,NULL);
	 list_free($2);
}

| sym_enum sym_ident {
	 $$=Struct_Declare(hd_str($2),TYPE_ENUM,NULL);
	 list_free($2);
}
;

liste_enumerateurs_virgule:
  liste_enumerateurs
| liste_enumerateurs ','
;

liste_enumerateurs:
  enumerateur 
| liste_enumerateurs ',' enumerateur
;

enumerateur:
  sym_ident {
		 Sym_Create(hd_str($1),TABLE_VAR,mk_tag(SYM_ENUM_CONST,mk_int(enum_val,NULL)));
		 enum_val++;
		 list_free($1);
	}
| sym_ident '=' expression_constante {
	 enum_val=hd_int(tl(tl($3)));
	 Sym_Create(hd_str($1),TABLE_VAR,mk_tag(SYM_ENUM_CONST,mk_int(enum_val,NULL)));
		 enum_val++;
	 list_free($1);
	 list_free($3);
}
;


/* declarateur avec variable */

declarateur:
  declarateur_absolu
| pointeur declarateur_absolu { $$=append($2,$1); }
;

declarateur_absolu:
  sym_ident { $$=mk_list($1,NULL); }
| '(' declarateur ')' { $$=$2; }
| declarateur_absolu '[' expression_constante ']' {
  $$=append($1,Type_ArrayDeclare(NULL,$3)); 
}
| declarateur_absolu '[' ']' {
  $$=append($1,Type_ArrayDeclare(NULL,NULL));
}
| declarateur_absolu '(' liste_de_parametres ')' {
	 $$=append($1,Type_FuncDeclare(FUNC_NEW,$3));
}
| declarateur_absolu '(' liste_de_parametres ',' sym_three_points ')' {
	 $$=append($1,Type_FuncDeclare(FUNC_ELLIPSIS,$3));
}
| declarateur_absolu '(' ')' {
	 $$=append($1,Type_FuncDeclare(FUNC_OLD,NULL));
}
| declarateur_absolu '(' liste_identificateurs ')' {
	 $$=append($1,Type_FuncDeclare(FUNC_OLD,$3));
}
;

pointeur:
  '*' { $$=mk_tag(TYPE_POINTER,NULL); }
| '*' liste_de_qualificatif_de_type { list_free($2); 
	 $$=mk_tag(TYPE_POINTER,NULL); }
| '*' pointeur { $$=mk_tag(TYPE_POINTER,$2); }
| '*' liste_de_qualificatif_de_type pointeur { 
	 list_free($2); $$=mk_tag(TYPE_POINTER,$3); }
;
     
liste_de_qualificatif_de_type:
  qualificatif_de_type {
		 $$=$1;
	}
| liste_de_qualificatif_de_type qualificatif_de_type { $$=append($1,$2); }
;

liste_de_parametres:
  declaration_de_parametres {
		 $$=$1;
	}
| liste_de_parametres ',' declaration_de_parametres {
	 $$=append($1,$3); }
;

declaration_de_parametres:
  specificateur_de_declaration declarateur {
		 $$=Type_VarList($1,mk_list($2,mk_list(NULL,NULL)));
	}
| specificateur_de_declaration {
		 $$=Type_VarList($1,mk_list(mk_list(NULL,NULL),mk_list(NULL,NULL)));
}
| specificateur_de_declaration declarateur_abstrait {
		 $$=Type_VarList($1,mk_list($2,mk_list(NULL,NULL))); }
;

liste_identificateurs:
  liste_identificateurs1 { $$=$1; }
| liste_identificateurs ',' liste_identificateurs1 { $$=append($1,$3); }
;

liste_identificateurs1:
  sym_ident {
		 $$=mk_list($1,mk_tag(STORAGE_DEFAULT,
													mk_list(NULL,mk_list(NULL,NULL))));
	}
;


/* declarateur abstrait (sans variable) */

nom_de_type:
  liste_de_qualificatifs_de_specificateurs {
		 LIST *var;
		 var=Type_VarList($1,mk_list(mk_list(NULL,NULL),mk_list(NULL,NULL)));
		 $$=list_dup(hd_list(tl(tl(var))));
		 list_free(var);
	}
| liste_de_qualificatifs_de_specificateurs declarateur_abstrait {
	 LIST *var;
	 var=Type_VarList($1,mk_list($2,mk_list(NULL,NULL)));
	 $$=list_dup(hd_list(tl(tl(var))));
	 list_free(var);
}
;

declarateur_abstrait:
  pointeur { $$=mk_list(NULL,$1); }
| pointeur declarateur_abstrait_absolu { $$=append($2,$1); }
| declarateur_abstrait_absolu { $$=$1; }
;

declarateur_abstrait_absolu:
  '(' declarateur_abstrait ')' { $$=$2; }

| declarateur_abstrait_absolu '[' expression_constante ']' {
	 $$=append($1,Type_ArrayDeclare(NULL,$3));
}
| '[' expression_constante ']' {
	 $$=mk_list(NULL,Type_ArrayDeclare(NULL,$2));
}
| declarateur_abstrait_absolu '[' ']' {
	 $$=append($1,Type_ArrayDeclare(NULL,NULL));
}
| '[' ']' { $$=mk_list(NULL,Type_ArrayDeclare(NULL,NULL)); }

| declarateur_abstrait_absolu '(' ')' 
    { $$=append($1,Type_FuncDeclare(FUNC_OLD,NULL)); }

| declarateur_abstrait_absolu '(' liste_de_parametres ')' 
    { $$=append($1,Type_FuncDeclare(FUNC_NEW,$3)); }

| declarateur_abstrait_absolu '(' liste_de_parametres ',' sym_three_points ')' 
    { $$=append($1,Type_FuncDeclare(FUNC_ELLIPSIS,$3)); }

| '(' ')' 
    { $$=mk_list(NULL,Type_FuncDeclare(FUNC_OLD,NULL)); }

| '(' liste_de_parametres ')'
	  { $$=mk_list(NULL,Type_FuncDeclare(FUNC_NEW,$2)); }

| '(' liste_de_parametres ',' sym_three_points ')'
	  { $$=mk_list(NULL,Type_FuncDeclare(FUNC_ELLIPSIS,$2)); }
;

/* initialiseur de variables */

initialisateur:
  expression_affectation { $$=mk_tag(INIT_EXPR,Expr_ConstEval($1)); }
| '{' liste_initialisateurs '}' { $$=mk_tag(INIT_LIST,$2); }
| '{' liste_initialisateurs ',' '}' { $$=mk_tag(INIT_LIST,$2); }
;


liste_initialisateurs:
  initialisateur { $$=mk_list($1,NULL); }
| liste_initialisateurs ',' initialisateur { $$=append($1,mk_list($3,NULL)); }
;


/* blocs & instructions */

instruction:
  instruction_composee
| instruction_expression
| instruction_iteration 
| instruction_de_selection
| instruction_de_saut
| instruction_etiquetee
;


instruction_expression:
  ';' 
| expression ';' { Gen_InstrExpr(Expr_ConstEval($1)); }
;

instruction_composee:
  entree_bloc liste_de_declarations liste_instructions sortie_bloc 
| entree_bloc liste_de_declarations sortie_bloc 
| entree_bloc liste_instructions sortie_bloc 
| entree_bloc sortie_bloc 
;

entree_bloc:
 '{' {
		BLOCK *b;
		Block_Enter(BLOCK_DECL);
		block_decl=block_current;
		b=block_current;
		do {
			 b=b->dad;
			 if (b==NULL) Error_Internal("Bloc de déclaration supérieur non trouvé");
		} while (b->type != BLOCK_DECL);
		block_decl->local_var_offset=b->local_var_offset;
  }
;

sortie_bloc:
 '}' {
    BLOCK *b;
    Block_Leave();
		b=block_current;
    while (b->type != BLOCK_DECL) {
			 b=b->dad;
			 if (b==NULL) Error_Internal("Bloc de déclaration supérieur non trouvé");
		} 
		block_decl=b;
 }
;


liste_instructions:
  instruction { $$=mk_list($1,NULL); }
| liste_instructions instruction { $$=append($1,mk_list($2,NULL)); }
;

instruction_de_selection:
  instruction_de_test instruction {
		 Gen_InstrIf2();
	}
| instruction_de_test instruction sym_else {
		 Gen_InstrIfElse2();
	} instruction {
		 Gen_InstrIfElse3();
	}
| sym_switch '(' expression ')' {
	   Gen_InstrSwitch1($3);
  } instruction {
		 Gen_InstrSwitch2();
	}		 
;


instruction_de_test:
  sym_if '(' expression ')' {
		 Gen_InstrIf1($3);
	}
;

expression_optionnelle:
  { $$=NULL; }
| expression { $$=$1; }
;
	 
instruction_iteration:
  sym_while '(' expression ')' {
		 Gen_InstrWhile1($3);
	} instruction {
		 Gen_InstrWhile2();
	}

| sym_do {
	   Gen_InstrDo1();
  } instruction sym_while '(' expression ')' {
		 Gen_InstrDo2($6);
	}

| sym_for '(' expression_optionnelle ';' 
              expression_optionnelle ';' 
              expression_optionnelle ')' {
			Gen_InstrFor1($3,$5);
	} instruction {
		  Gen_InstrFor2($7);
	}
;

instruction_de_saut:
  sym_break ';' { Gen_InstrBreak(); }
|	sym_continue ';' { Gen_InstrContinue(); }
| sym_return ';' { Gen_InstrReturn(NULL); }
| sym_return expression ';' { Gen_InstrReturn($2); }
| sym_goto sym_ident ';' { Gen_InstrGoto($2); }
;

instruction_etiquetee:
  sym_ident ':' {
		 Gen_InstrLabel($1);
	} instruction 
| sym_case expression_constante ':' {
		 Gen_InstrCase($2);
  } instruction 
| sym_default ':' {
	   Gen_InstrCase(NULL);
	} instruction
;

/* Expressions */

expression:
  liste_expressions { $$=Expr_List($1); }
;

liste_expressions:
  expression_affectation { $$=mk_list($1,NULL); }
| liste_expressions ',' expression_affectation { $$=append($1,mk_list($3,NULL)); }
;

expression_affectation:
  expression_conditionnelle 
    { $$=$1; }
| expression_unaire '=' expression_affectation 
    { $$=Expr_Assign($1,$3); }
| expression_unaire operateur_affectation_operation expression_affectation { 
	 $$=Expr_AssignOp(hd_tag($2),$1,$3); 
	 list_free($2);
   }
;

operateur_affectation_operation:
    sym_assign_add { $$=mk_tag(EXPR_ADD,NULL); }
|   sym_assign_sub { $$=mk_tag(EXPR_SUB,NULL); }
|   sym_assign_mul { $$=mk_tag(EXPR_MUL,NULL); }
|   sym_assign_div { $$=mk_tag(EXPR_DIV,NULL); }
|   sym_assign_mod { $$=mk_tag(EXPR_MOD,NULL); }
|   sym_assign_and { $$=mk_tag(EXPR_AND,NULL); }
|   sym_assign_or  { $$=mk_tag(EXPR_OR,NULL); }
|   sym_assign_xor { $$=mk_tag(EXPR_XOR,NULL); }
|   sym_assign_shr { $$=mk_tag(EXPR_SHR,NULL); }
|   sym_assign_shl { $$=mk_tag(EXPR_SHL,NULL); }
;			 

expression_constante:
  expression_conditionnelle { 
		 $$=Expr_ConstEval($1);
	}
;
	
expression_conditionnelle:
  expression_OU_logique { $$=$1; }
| expression_OU_logique '?' expression ':' expression_conditionnelle {
		 $$=Expr_Cond($1,$3,$5);
	}
;

expression_OU_logique:
  expression_ET_logique { $$=$1; }
| expression_OU_logique sym_lor expression_ET_logique { $$=Expr_Binary(EXPR_LOR,$1,$3); }
;

expression_ET_logique:
  expression_OU_inclusive { $$=$1; }
| expression_ET_logique sym_land expression_OU_inclusive { $$=Expr_Binary(EXPR_LAND,$1,$3); }
;

expression_OU_inclusive:
  expression_OU_exclusive { $$=$1; }
| expression_OU_inclusive '|' expression_OU_exclusive { $$=Expr_Binary(EXPR_OR,$1,$3); }
;		  

expression_OU_exclusive:
  expression_ET { $$=$1; }
| expression_OU_exclusive '^' expression_ET { $$=Expr_Binary(EXPR_XOR,$1,$3); }
;

expression_ET:
  expression_egalite { $$=$1; }
| expression_ET '&' expression_egalite { $$=Expr_Binary(EXPR_AND,$1,$3); }
;

expression_egalite:
  expression_relationnelle { $$=$1; }
| expression_egalite sym_eq expression_relationnelle { $$=Expr_Binary(EXPR_EQ,$1,$3); }
| expression_egalite sym_ne expression_relationnelle { $$=Expr_Binary(EXPR_NE,$1,$3); }
;

expression_relationnelle:
  expression_de_decalage { $$=$1; }
| expression_relationnelle '<' expression_de_decalage { $$=Expr_Binary(EXPR_LT,$1,$3); }
| expression_relationnelle '>' expression_de_decalage { $$=Expr_Binary(EXPR_GT,$1,$3); }
| expression_relationnelle sym_le expression_de_decalage { $$=Expr_Binary(EXPR_LE,$1,$3); }
| expression_relationnelle sym_ge expression_de_decalage { $$=Expr_Binary(EXPR_GE,$1,$3); }
;

expression_de_decalage:
  expression_additive
| expression_de_decalage sym_shl expression_additive { $$=Expr_Binary(EXPR_SHL,$1,$3); }
| expression_de_decalage sym_shr expression_additive { $$=Expr_Binary(EXPR_SHR,$1,$3); }
;

expression_additive:
  expression_multiplicative
     { $$=$1; }
|	expression_additive '+' expression_multiplicative
     { $$=Expr_Binary(EXPR_ADD,$1,$3); }
|	expression_additive '-' expression_multiplicative
     { $$=Expr_Binary(EXPR_SUB,$1,$3); }
;

expression_multiplicative:
  expression_conversion { $$=$1; }
| expression_multiplicative '*' expression_conversion { $$=Expr_Binary(EXPR_MUL,$1,$3); } 
| expression_multiplicative '/' expression_conversion { $$=Expr_Binary(EXPR_DIV,$1,$3); } 
| expression_multiplicative '%' expression_conversion { $$=Expr_Binary(EXPR_MOD,$1,$3); }
;

expression_conversion:
  expression_unaire { $$=$1; }
| '(' nom_de_type ')' expression_conversion { $$=Expr_Cast($2,$4); }
;

expression_unaire:
  expression_postfixee { $$=$1; }
| sym_inc expression_unaire {
	 $$=Expr_AssignOp(EXPR_ADD,$2,Expr_ConstInteger(1));
  }  
| sym_dec expression_unaire { 
	 $$=Expr_AssignOp(EXPR_SUB,$2,Expr_ConstInteger(1)); 
  }
| '&' expression_conversion { $$=Expr_Addr($2); }
| '*' expression_conversion { $$=Expr_Indir($2); }
| '+' expression_conversion { $$=Expr_Unary(EXPR_PLUS,$2); }
| '-' expression_conversion { $$=Expr_Unary(EXPR_NEG,$2); }
| '~' expression_conversion { $$=Expr_Unary(EXPR_NOT,$2); }
| '!' expression_conversion { $$=Expr_Unary(EXPR_LNOT,$2); }
| sym_sizeof expression_unaire { $$=Expr_Sizeof($2,1); }
| sym_sizeof '(' nom_de_type ')' { $$=Expr_Sizeof($3,0); }
;

expression_postfixee:
  expression_primaire { $$=$1; }
| expression_postfixee '[' expression ']' { $$=Expr_Index($1,$3); }
| expression_postfixee '(' liste_expressions_en_arguments ')' {
       $$=Expr_Call($1,$3);
			 }
| expression_postfixee '(' ')' { $$=Expr_Call($1,NULL); }
| expression_postfixee '.' sym_ident { 
       $$=Expr_Field($1,hd_str($3)); 
			 list_free($3);
			 }
| expression_postfixee sym_arrow sym_ident { 
      $$=Expr_Field(Expr_Indir($1),hd_str($3));
			list_free($3);
      }
| expression_postfixee sym_inc { 
	 $$=Expr_Binary(EXPR_SUB,
									Expr_AssignOp(EXPR_ADD,$1,Expr_ConstInteger(1)),
									Expr_ConstInteger(1));
   }
| expression_postfixee sym_dec { 
	 $$=Expr_Binary(EXPR_ADD,
									Expr_AssignOp(EXPR_SUB,$1,Expr_ConstInteger(1)),
									Expr_ConstInteger(1));
   }
;

expression_primaire:
  sym_ident { 
		 $$=Expr_Ident(hd_str($1)); 
		 list_free($1); 
	}
| sym_const_int  { 
	 $$=mk_list(mk_tag(TYPE_INT,NULL),mk_tag(EXPR_INT,$1));
}
| sym_const_char { 
	 $$=mk_list(mk_tag(TYPE_CHAR,NULL),mk_tag(EXPR_INT,$1));
}
| liste_de_constantes_chaines { 
	 $$=mk_list(mk_tag(TYPE_POINTER,mk_tag(TYPE_CHAR,NULL)),mk_tag(EXPR_STR,$1));
  }
| '(' expression ')' { $$=$2; }
;


liste_expressions_en_arguments:
  expression_affectation { $$=mk_list($1,NULL); }
| liste_expressions_en_arguments ',' expression_affectation {
      $$=append($1,mk_list($3,NULL));
			}
;

liste_de_constantes_chaines:
  sym_const_str { $$=$1; }
| liste_de_constantes_chaines sym_const_str { $$=append($1,$2); }
;	


%%

#include "lex.yy.c"

int enum_val;
		 
