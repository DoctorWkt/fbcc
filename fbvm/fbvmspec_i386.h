/* Types de base de la machine

   b  = signed 8 bits
   ub = unsigned 8 bits
   w  = signed 16 bits
   uw = unsigned 16 bits
   i  = signed 32 bits
   ui = unsigned 32 bits
 
   p  = pointer (32 bits) 
*/   
   
/* nom de l'architecture */
#define VM_ARCH_NAME "i386"


/* taille des types (sert aussi pour l'alignement) */
#define VM_CHAR_SIZE    1
#define VM_SHORT_SIZE   2
#define VM_INT_SIZE     4
#define VM_POINTER_SIZE 4

/* taille minimum d'un élément mis sur la pile */
#define VM_STACK_ALIGN  4

/* alignement des segments */
#define VM_SEG_ALIGN  16

/* Endianité */
#define VM_LITTLE_ENDIAN 1

/* modèle de pile de la machine virtuelle */
#define VM_LOCAL_START        4
#define VM_LOCAL_PARAM_END    -8

/* instructions */

enum {
	   ld_b=1,
		 ld_ub,
		 ld_w,
		 ld_uw,
		 ld_i,
		 
		 st_b,
		 st_w,
		 st_i,
		 
		 add_i,
		 sub_i,
		 mul_i,
		 div_i,
		 div_ui,
		 mod_i,
		 mod_ui,
		 neg_i,
		 
		 cmplt_i,
		 cmple_i,
		 cmpge_i,
		 cmpgt_i,
		 cmpeq_i,
		 cmpne_i,

		 cmplt_ui,
		 cmple_ui,
		 cmpge_ui,
		 cmpgt_ui,

		 and_i,
		 or_i,
		 xor_i,
		 not_i,
		 shl_i,
		 shr_i,
		 shr_ui,
		 
		 /* conversions */
		 cvt_i_b,
		 cvt_i_ub,
		 cvt_i_w,
		 cvt_i_uw,
		 
		 cvt_b_i,
		 cvt_w_i,
		 

 		 li_i,
		 libp_i,
  
		 /* sauts conditionnels */
		 jeq_i,
		 jne_i,
		 switch_i,
		 
		 /* sauts */
		 jmp,
		 
		 /* appel de fonctions */
		 jsr,
		 rts,
		 
		 /* gestion de la pile */
		 op_dup,
		 pop,
		 addsp,

		 /* appel de fonctions externes */
		 libcall,
};

#define FBVM_MAGIC (('f' << 24)|('b' << 16)|('v' << 8)|'m')

