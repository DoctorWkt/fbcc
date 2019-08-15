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
#include "fbcc.h"
#include <fbvminstr.h>



void Gen_Code(int c)
{
	 printf(" %s\n",vm_instr_str[c]);
}

void Gen_Int(unsigned int c)
{
	 printf(" =%d\n",c);
}

int label_num;

int Gen_NewLabel(void)
{
	 label_num++;
	 return label_num;
}

void Gen_Label(int c)
{
	 printf("@L%d:\n",c);
}

void Gen_Jmp(int c,int l)
{
	 printf(" %s @L%d\n",vm_instr_str[c],l);
}

void Gen_LI(int c,int data,SYM *s)
{
	 if (s==NULL) {
			printf(" %s %d\n",vm_instr_str[c],data);
	 } else if (data==0) {
			printf(" %s %s\n",vm_instr_str[c],s->str);
	 } else {
			printf(" %s %s+%d\n",vm_instr_str[c],s->str,data);
	 }
}


/*
 * Génération du code pour les expressions
 */

void Gen_RVal(LIST *e);
void Gen_LVal(LIST *e);

/*
 * Génération du code pour le casting
 */
void Gen_Cast(LIST *expr)
{
	 LIST *e1;
	 int t1,t2;
	 
	 e1=hd_list(tl(tl(expr)));
	 t1=hd_tag(hd_list(expr));
	 t2=hd_tag(hd_list(e1));

	 Gen_RVal(e1);
	 
	 /* génération des instructions de conversion de type */
	 
	 switch (t1) {

		case TYPE_CHAR:
		case TYPE_UCHAR:
			switch(t2) {
			 case TYPE_CHAR:
			 case TYPE_UCHAR:
				 break;
			 default:
				 Gen_Code(cvt_b_i);
			}
			break;

		case TYPE_SHORT:
		case TYPE_USHORT:
			switch(t2) {
			 case TYPE_CHAR:
				 Gen_Code(cvt_i_b);
				 break;
			 case TYPE_UCHAR:
				 Gen_Code(cvt_i_ub);
				 break;
			 default:
				 Gen_Code(cvt_w_i);
			}
			break;

		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_ENUM:
		case TYPE_POINTER:
		case TYPE_ARRAY:
		case TYPE_FUNC:
			switch(t2) {
			 case TYPE_CHAR:
				 Gen_Code(cvt_i_b);
				 break;
			 case TYPE_UCHAR:
				 Gen_Code(cvt_i_ub);
				 break;
			 case TYPE_SHORT:
				 Gen_Code(cvt_i_w);
				 break;
			 case TYPE_USHORT:
				 Gen_Code(cvt_i_uw);
				 break;
			}
			break;
		case TYPE_VOID:
			break;

		default:
			Error_Internal("Conversion vers un type non implémenté");
			break;
	 }
}

void Gen_Binary_Op(LIST *expr,int op_i,int op_ui)
{
	 int t;
	 LIST *e1,*e2;

	 e1=hd_list(tl(tl(expr)));
	 e2=hd_list(tl(tl(tl(expr))));
	 t=hd_tag(hd_list(e1));
	 
	 Gen_RVal(e1);
	 Gen_RVal(e2);
	 switch (t) {
		case TYPE_UINT:
			Gen_Code(op_ui);
			break;
		default:
			Gen_Code(op_i);
			break;
	 }
}

void Gen_ExprUnary(LIST *expr,int op_i,int op_ui)
{
	 int t;
	 LIST *e1;

	 e1=hd_list(tl(tl(expr)));
	 t=hd_tag(hd_list(e1));
	 
	 Gen_RVal(e1);
	 switch (t) {
		case TYPE_UINT:
			Gen_Code(op_ui);
			break;
		default:
			Gen_Code(op_i);
			break;
	 }
}


void Gen_Assign(LIST *expr)
{
  LIST *e1,*e2;
  int t,size;
  
  t=hd_tag(hd_list(expr));
  e1=hd_list(tl(tl(expr)));
  e2=hd_list(tl(tl(tl(expr))));
  
  switch(t) {
   case TYPE_CHAR:
   case TYPE_UCHAR:
    Gen_RVal(e2);
    Gen_LVal(e1);
    Gen_Code(st_b);
    break;
   case TYPE_SHORT:
   case TYPE_USHORT:
    Gen_RVal(e2);
    Gen_LVal(e1);
    Gen_Code(st_w);
    break;
   case TYPE_INT:
   case TYPE_UINT:
   case TYPE_ENUM:
   case TYPE_POINTER:
   case TYPE_ARRAY:
   case TYPE_FUNC:
    Gen_RVal(e2);
    Gen_LVal(e1);
    Gen_Code(st_i);
    break;
    /* structures & unions */
   case TYPE_STRUCT:
   case TYPE_UNION:
    size=Type_Size(hd_list(e2));
    switch(size) {
     case 4:
      Gen_RVal(e2);
      Gen_Code(ld_i);
      Gen_LVal(e1);
      Gen_Code(st_i);
      break;
     default:
      Error_Internal("Gen_Assign: Taille de struct non implémentée");
    }
    break;
   default:
    Error_Internal("Gen_Assign: Type non implémenté");
  }
}

void Gen_ExprBinaryLogical(int op,LIST *expr)
{
	 LIST *e1,*e2;
	 int l1,l2;
	 
	 e1=hd_list(tl(tl(expr)));
	 e2=hd_list(tl(tl(tl(expr))));
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();
	 
	 Gen_RVal(e1);
	 if (op==EXPR_LAND) Gen_Jmp(jeq_i,l1); else Gen_Jmp(jne_i,l1); 

	 Gen_RVal(e2);
	 if (op==EXPR_LAND) Gen_Jmp(jeq_i,l1); else Gen_Jmp(jne_i,l1); 

	 if (op==EXPR_LAND) Gen_LI(li_i,1,NULL); else Gen_LI(li_i,0,NULL);
	 Gen_Jmp(jmp,l2);

	 Gen_Label(l1);
	 if (op==EXPR_LAND) Gen_LI(li_i,0,NULL); else Gen_LI(li_i,1,NULL);
	 
	 Gen_Label(l2);
}


void Gen_ExprCond(LIST *expr)
{
	 LIST *e,*e1,*e2;
	 int l1,l2;
	 
	 e=hd_list(tl(tl(expr)));
	 e1=hd_list(tl(tl(tl(expr))));
	 e2=hd_list(tl(tl(tl(tl(expr)))));
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();

	 Gen_RVal(e);
	 Gen_Jmp(jeq_i,l1);
	 
	 Gen_RVal(e1);
	 Gen_Jmp(jmp,l2);

	 Gen_Label(l1);
	 Gen_RVal(e2);

	 Gen_Label(l2);
}


void Gen_LIdent(LIST *expr)
{
	 SYM *s;
	 LIST *var_adr;
	 
	 s=hd_sym(tl(tl(expr)));
	 var_adr=hd_list(tl(tl(tl(s->list))));
	 
	 switch(hd_tag(var_adr)) {
		case VAR_STACK:
			Gen_LI(libp_i,hd_int(tl(var_adr)),NULL);
			break;
		default:
			Gen_LI(li_i,0,s);
			break;
	 }
}

void Gen_RIdent(LIST *expr)
{
  int t;
  
  Gen_LIdent(expr);
  t=hd_tag(hd_list(expr));
  switch(t) {
   case TYPE_CHAR:
    Gen_Code(ld_b);
    break;
   case TYPE_UCHAR:
    Gen_Code(ld_ub);
    break;
   case TYPE_SHORT:
    Gen_Code(ld_w);
    break;
   case TYPE_USHORT:
    Gen_Code(ld_uw);
    break;
   case TYPE_INT:
   case TYPE_UINT:
   case TYPE_POINTER:
   case TYPE_ENUM:
    Gen_Code(ld_i);
    break;
   case TYPE_FUNC:
   case TYPE_STRUCT:
   case TYPE_UNION:
   case TYPE_ARRAY:
    break;
   default:
    Error_Internal("Gen_RIdent: type non géré");
    break;
  }
}


void Gen_Indir(LIST *expr)
{
	 LIST *e1;
	 int t;
	 
	 e1=hd_list(tl(tl(expr)));
	 Gen_RVal(e1);
	 t=hd_tag(hd_list(expr));
	 switch(t) {
		case TYPE_CHAR:
			Gen_Code(ld_b);
			break;
		case TYPE_UCHAR:
			Gen_Code(ld_ub);
			break;
		case TYPE_SHORT:
			Gen_Code(ld_w);
			break;
		case TYPE_USHORT:
			Gen_Code(ld_uw);
			break;
		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_POINTER:
			Gen_Code(ld_i);
			break;
		case TYPE_FUNC:
		case TYPE_STRUCT:
		case TYPE_UNION:
		case TYPE_ENUM:
		case TYPE_ARRAY:
			break;
		default:
			Error_Internal("Gen_Indir: type non géré");
			break;
	 }
}


void Gen_Const(LIST *expr)
{
	 int c;
	 LIST *type;
	 SYM *s;
	 LIST *buf;
	 int buf_size,i,tag;
	 char bufstr[SYM_LENMAX];
	 
	 type=hd_list(expr);
	 tag=hd_tag(tl(expr));
	 if (tag == EXPR_STR) {
			/* génération d'une chaine */
			Sym_NewName(bufstr);
			s=Sym_New(bufstr,TABLE_VAR,NULL);
			printf(".data\n");
			printf("%s:\n",s->str);
			buf=tl(tl(expr));
			while (buf!=NULL) {
				 buf_size=buf->data.buf_size;
				 for(i=0;i<buf_size;i++) {
						printf(" .byte %d\n",buf->str[i]);
				 }
				 buf=tl(buf);
			}
			printf(" .byte 0\n");
			printf(".text\n");
			Gen_LI(li_i,0,s);
	 } else {
			c=hd_int(tl(tl(expr)));
			Gen_LI(li_i,c,NULL);
	 }
}

void Gen_ExprCall(LIST *expr)
{
	 int param_size,param_nb,i,j;
	 LIST *params,*param,*func_expr;
	 
	 func_expr=hd_list(tl(tl(expr)));
	 param_nb=hd_int(tl(tl(tl(expr))));
	 params=tl(tl(tl(tl(expr))));

	 /* Evaluation de chacun des parametres et mise sur la pile 
	  * On doit evaluer les parametres en commençant par le dernier
	  * On calcule en même temps la taille allouée sur la pile par
		* les paramètres 
		*/
	 param_size=0;
	 for(i=param_nb-1;i>=0;i--) {
			param=params;
			for(j=0;j<i;j++) param=tl(param);
			Gen_RVal(hd_list(param));
			/* pour l'instant, la taille sur la pile est de VM_STACK_ALIGN */
			param_size+=VM_STACK_ALIGN;
	 }
	 
	 /* Evaluation de l'adresse de la fonction */
	 Gen_RVal(func_expr);
	 
	 /* Génération de l'appel */
	 Gen_LI(jsr,param_size,NULL);
	 
}

void Gen_ExprList(LIST *expr)
{
	 LIST *e;
	 expr=tl(tl(expr));
	 do {
			e=hd_list(expr);
			Gen_RVal(e);
			expr=tl(expr);
			if (expr!=NULL) Gen_LI(addsp,-4,NULL);
	 } while (expr!=NULL);
}

	 
	 

void Gen_LVal(LIST *e)
{
	 int tag;
	 
	 tag=hd_tag(tl(e));
	 switch(tag) {
		case EXPR_IDENT:
			Gen_LIdent(e);
			break;
		case EXPR_INDIR:
			Gen_RVal(hd_list(tl(tl(e))));
			break;
		default:
			Error_Internal("Gen_LVal: opération %d non implémentée",tag);
			break;
	 }
}

void Gen_RVal(LIST *e)
{
	 int tag;
	 
	 tag=hd_tag(tl(e));
	 switch(tag) {
		case EXPR_INT:
		case EXPR_STR:
			Gen_Const(e);
			break;
		case EXPR_ASSIGN:
			Gen_Assign(e);
			break;
		case EXPR_IDENT:
			Gen_RIdent(e);
			break;
		case EXPR_INDIR:
			Gen_Indir(e);
			break;
		case EXPR_ADDR:
			Gen_LVal(hd_list(tl(tl(e))));
			break;
		case EXPR_CALL:
			Gen_ExprCall(e);
			break;
		case EXPR_CAST:
			Gen_Cast(e);
			break;
		case EXPR_LIST:
			Gen_ExprList(e);
			break;
			/* arithmétique */
		case EXPR_ADD:
			Gen_Binary_Op(e,add_i,add_i);
			break;
		case EXPR_SUB:
			Gen_Binary_Op(e,sub_i,sub_i);
			break;
		case EXPR_MUL:
			Gen_Binary_Op(e,mul_i,mul_i);
			break;
		case EXPR_DIV:
			Gen_Binary_Op(e,div_i,div_ui);
			break;
		case EXPR_MOD:
			Gen_Binary_Op(e,mod_i,mod_ui);
			break;
		case EXPR_AND:
			Gen_Binary_Op(e,and_i,and_i);
			break;
		case EXPR_OR:
			Gen_Binary_Op(e,or_i,or_i);
			break;
		case EXPR_XOR:
			Gen_Binary_Op(e,xor_i,xor_i);
			break;
		case EXPR_SHR:
			Gen_Binary_Op(e,shr_i,shr_ui);
			break;
		case EXPR_SHL:
			Gen_Binary_Op(e,shl_i,shl_i);
			break;
		
		case EXPR_NOT:
			Gen_ExprUnary(e,not_i,not_i);
			break;
		case EXPR_NEG:
			Gen_ExprUnary(e,neg_i,neg_i);
			break;
			
			/* comparaisons */
		case EXPR_LT:
			Gen_Binary_Op(e,cmplt_i,cmplt_ui);
			break;
		case EXPR_LE:
			Gen_Binary_Op(e,cmple_i,cmple_ui);
			break;
		case EXPR_GT:
			Gen_Binary_Op(e,cmpgt_i,cmpgt_ui);
			break;
		case EXPR_GE:
			Gen_Binary_Op(e,cmpge_i,cmpge_ui);
			break;
		case EXPR_EQ:
			Gen_Binary_Op(e,cmpeq_i,cmpeq_i);
			break;
		case EXPR_NE:
			Gen_Binary_Op(e,cmpne_i,cmpne_i);
			break;

			/* opérations logiques (EXPR_LNOT à été transformé en comparaison) */
		case EXPR_LAND:
		case EXPR_LOR:
			Gen_ExprBinaryLogical(tag,e);
			break;
			
		case EXPR_COND:
			Gen_ExprCond(e);
			break;
			
		default:
			Error_Internal("Gen_RVal: opération %d non implémentée",tag);
			break;
	 }
}


void Gen_Expr(LIST *e)
{
	 if (debug_print_expr) {
			printf("/* Gen_Expr=");
			List_Print(e);
			printf("*/\n");
	 }
	 
	 Gen_RVal(e);
}

void Gen_InstrIf1(LIST *expr)
{
	 int l1,l2;
	 
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();

	 Gen_Expr(expr);
	 Gen_Jmp(jeq_i,l1);
	 
	 Block_Enter(BLOCK_IF);
	 block_current->label_break=l1;
	 block_current->label_continue=l2;

	 list_free(expr);
}

void Gen_InstrIf2(void)
{
	 Gen_Label(block_current->label_break);
	 Block_Leave();
}

void Gen_InstrIfElse2(void) 
{
	 Gen_Jmp(jmp,block_current->label_continue);
	 Gen_Label(block_current->label_break);
}
	 
void Gen_InstrIfElse3(void) 
{	 
	 Gen_Label(block_current->label_continue);
	 Block_Leave();
}

	 
void Gen_InstrWhile1(LIST *expr) 
{
	 int l1,l2;
	 
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();

	 Gen_Label(l1);
	 Gen_Expr(expr);
	 Gen_Jmp(jeq_i,l2);

	 Block_Enter(BLOCK_WHILE);
	 block_current->label_break=l2;
	 block_current->label_continue=l1;

	 list_free(expr);
}



void Gen_InstrWhile2(void)
{
	 Gen_Jmp(jmp,block_current->label_continue);
	 Gen_Label(block_current->label_break);
	 Block_Leave();
}

void Gen_InstrDo1(void)
{
	 int l1,l2,l3;
	 
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();
	 l3=Gen_NewLabel();

	 Gen_Label(l1);
	 
	 Block_Enter(BLOCK_WHILE);
	 block_current->label_restart=l1;
	 block_current->label_break=l2;
	 block_current->label_continue=l3;
}

void Gen_InstrDo2(LIST *expr)
{
	 Gen_Label(block_current->label_continue);

	 Gen_Expr(expr);
	 Gen_Jmp(jne_i,block_current->label_restart);

	 Gen_Label(block_current->label_break);
	 
	 Block_Leave();

	 list_free(expr);
}


void Gen_InstrFor1(LIST *expr1,LIST *expr2)
{
	 int l1,l2,l3;
	 
	 l1=Gen_NewLabel();
	 l2=Gen_NewLabel();
	 l3=Gen_NewLabel();

	 if (expr1!=NULL) Gen_InstrExpr(expr1);
	 
	 Gen_Label(l1);
	 
	 if (expr2!=NULL) {
			Gen_Expr(expr2);
			Gen_Jmp(jeq_i,l2);
			list_free(expr2);
	 }
	 
	 Block_Enter(BLOCK_WHILE);
	 block_current->label_restart=l1;
	 block_current->label_break=l2;
	 block_current->label_continue=l3;
}

void Gen_InstrFor2(LIST *expr3)
{
	 Gen_Label(block_current->label_continue);
	 if (expr3!=NULL) Gen_InstrExpr(expr3);
	 Gen_Jmp(jmp,block_current->label_restart);
	 
	 Gen_Label(block_current->label_break);
	 
	 Block_Leave();
	 
}


									 
									 


void Gen_InstrBreak(void)
{
	 BLOCK *b;
	 b=block_current;
	 while (1) {
			if (b->type == BLOCK_GLOBAL) Error("Instruction 'break' sans boucle ni 'switch'");
			if (b->type == BLOCK_WHILE || b->type == BLOCK_SWITCH) break;
			b=b->dad;
	 }
	 Gen_Jmp(jmp,b->label_break);
}


void Gen_InstrContinue(void)
{
	 BLOCK *b;
	 b=block_current;
	 while (1) {
			if (b->type == BLOCK_GLOBAL) Error("Instruction 'continue' sans boucle");
			if (b->type == BLOCK_WHILE) break;
			b=b->dad;
	 }
	 Gen_Jmp(jmp,b->label_continue);
}


void Gen_InstrGoto(LIST *ident)
{
	 SYM *s;
	 int l1;
	 char *label;
	 
	 label=hd_str(ident);
	 
	 s=Sym_Search(label,TABLE_LABEL);
	 if (s==NULL) {
			l1=Gen_NewLabel();
			Sym_New1(label,TABLE_LABEL,block_function,mk_int(-1,mk_int(l1,NULL)));
	 } else {
			l1=hd_int(tl(s->list));
	 }
	 Gen_Jmp(jmp,l1);

	 list_free(ident);
}

void Gen_InstrReturn(LIST *expr)
{
	 if (expr!=NULL) {
			Gen_Expr(expr);
	 } else {
			Gen_LI(li_i,0,NULL);
	 }
	 Gen_Code(rts);

	 list_free(expr);
}


void Gen_InstrLabel(LIST *ident)
{
	 SYM *s;
	 int l1,flag;
	 char *label;

	 label=hd_str(ident);
	 
	 s=Sym_Search(label,TABLE_LABEL);
	 if (s!=NULL) {
			flag=hd_int(s->list);
			if (flag!=-1) Error("Label '%s' déjà définie",label);
			l1=hd_int(tl(s->list));
			list_free(s->list);
			s->list=mk_int(0,mk_int(l1,NULL));
	 } else {
			l1=Gen_NewLabel();
			Sym_New1(label,TABLE_LABEL,block_function,mk_int(0,mk_int(l1,NULL)));
	 }
	 Gen_Label(l1);

	 list_free(ident);
}

void Gen_InstrExpr(LIST *expr)
{
	 Gen_Expr(expr);
	 Gen_Code(pop);
	 list_free(expr);
}

/*
 * Génération de code pour les switchs
 */

void Gen_InstrSwitch1(LIST *expr)
{
	 int l1;
	 SYM *s;
	 char bufstr[SYM_LENMAX];
	 
	 l1=Gen_NewLabel();
	 Sym_NewName(bufstr);
	 s=Sym_New(bufstr,TABLE_VAR,NULL);
	 
	 Gen_Expr(expr);
	 Gen_LI(li_i,0,s);
	 Gen_Code(switch_i);
	 
	 Block_Enter(BLOCK_SWITCH);
	 block_current->label_break=l1;
	 block_current->label_restart=-1;   /* si != -1 : il y a une étiquette default */
	 block_current->switch_values=mk_sym(s,NULL);

	 list_free(expr);
}


void Gen_InstrSwitch2(void)
{
	 LIST *sv,*sv1;
	 SYM *s;
	 int count;
	 
	 sv=block_current->switch_values;
	 s=hd_sym(sv);
	 sv=tl(sv);
	 
	 sv1=sv;
	 count=1;
	 while (sv1!=NULL) {
			count++;
			sv1=tl(tl(sv1));
	 }

	 printf(".data\n");
	 printf(" .align 4\n");
	 printf("%s:\n",s->str);
	 
	 /* génération de la table de valeurs */
	 printf(" .int %d\n",count);
	 sv1=sv;
	 while (sv1!=NULL) {
			printf(" .int %d\n",hd_int(sv1));
			sv1=tl(tl(sv1));
	 }
	 
	 /* table de pointeurs */
	 if (block_current->label_restart != -1) {
			printf(" .int @L%d\n",block_current->label_restart);
	 } else {
			printf(" .int @L%d\n",block_current->label_break);
	 }
	 sv1=sv;
	 while (sv1!=NULL) {
			printf(" .int @L%d\n",hd_int(tl(sv1)));
			sv1=tl(tl(sv1));
	 }
	 
	 printf(".text\n");
	 list_free(block_current->switch_values);

	 Gen_Label(block_current->label_break);
	 Block_Leave();
}

/*
 * On ne détecte pas ici s'il y a des étiquettes case redondantes.
 * A faire !
 */

void Gen_InstrCase(LIST *expr)
{
	 BLOCK *b;
	 int l1;
	 
	 b=block_current;
	 while (1) {
			if (b->type == BLOCK_GLOBAL) Error("Instruction 'case' ou 'default' sans switch");
			if (b->type == BLOCK_SWITCH) break;
			b=b->dad;
	 }

	 l1=Gen_NewLabel();
	 Gen_Label(l1);

	 if (expr==NULL) {
			/* default */
			if (b->label_restart != -1) Error("Un seul 'default:' autorisé");
			b->label_restart=l1;
	 } else {
			/* case */
			/* test de type non fait */
			if (hd_tag(tl(expr)) != EXPR_INT ) 
				Error("Expression constante entière attendue après un case");
			
			b->switch_values=append(b->switch_values,
															mk_int(hd_int(tl(tl(expr))),mk_int(l1,NULL)));
	 }
	 
	 list_free(expr);
}

	 
