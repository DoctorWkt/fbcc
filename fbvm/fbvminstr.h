
char *vm_instr_str[]=
{
     "zero",
     "ld_b", 
		 "ld_ub",
		 "ld_w",
		 "ld_uw",
		 "ld_i",
		 
		 "st_b",
		 "st_w",
		 "st_i",
		 
		 "add_i",
		 "sub_i",
		 "mul_i",
		 "div_i",
		 "div_ui",
		 "mod_i",
		 "mod_ui",
		 "neg_i",
		 
		 "cmplt_i",
		 "cmple_i",
		 "cmpge_i",
		 "cmpgt_i",
		 "cmpeq_i",
		 "cmpne_i",

		 "cmplt_ui",
		 "cmple_ui",
		 "cmpge_ui",
		 "cmpgt_ui",

		 "and_i",
		 "or_i",
		 "xor_i",
		 "not_i",
		 "shl_i",
		 "shr_i",
		 "shr_ui",
		 
		 /* conversions */
		 "cvt_i_b",
		 "cvt_i_ub",
		 "cvt_i_w",
		 "cvt_i_uw",
		 
		 "cvt_b_i",
		 "cvt_w_i",
		 

 		 "li_i",
		 "libp_i",
  
		 /* sauts conditionnels */
		 "jeq_i",
		 "jne_i",
		 "switch_i",
		 
		 /* sauts */
		 "jmp",
		 
		 /* appel de fonctions */
		 "jsr",
		 "rts",
		 
		 /* gestion de la pile */
		 "dup",
		 "pop",
		 "addsp",

		 "libcall",
		 NULL,
};

