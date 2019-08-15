%{
#define YY_ALWAYS_INTERACTIVE 1
%}

delim [\t\n\r ]
digit [0-9]

ident [a-zA-Z_@][a-zA-Z_0-9@]*

%x COMMENT

%%

"/*"                   { BEGIN(COMMENT); }
<COMMENT>[^*\n]*       { }
<COMMENT>"*"+[^*/\n]*  { }
<COMMENT>\n            { lex_linenum++; }
<COMMENT>"*/"          { BEGIN(INITIAL); }


"\n"  { lex_linenum++; }
{delim} /* rien du tout */
	 
".text"    { return sym_text; }
".data"    { return sym_data; }
".globl"   { return sym_globl; }
".byte"    { return sym_byte; }
".int"     { return sym_int; }
".zero"    { return sym_zero; }
".align"   { return sym_align; }
".equ"     { return sym_equ; }
".short"   { return sym_short; }
".module"  { return sym_module; }

"-"?{digit}+ {
   int sign,n;
	 char *p;
	 p=yytext;
	 n=0;
	 sign=1;
	 if (*p=='-') {
			sign=-1;
			p++;
	 }
	 while (*p!='\0') {
			n=n*10+(*p - '0');
			p++;
	 }
	 n=n*sign;
	 yylval.lex_num=n;
/* sscanf(yytext,"%d",&yylval.lex_num); */
   return sym_num;
}

{ident} {
	 /* recherche si c'est une instruction assembleur */
	 char **p;
	 int c;
	 p=vm_instr_str;
	 while (1) {
			if (*p == NULL) {
				 yylval.lex_ident=malloc(strlen(yytext)+1);
				 strcpy(yylval.lex_ident,yytext);
				 return sym_ident;
			}
			if (strcmp(*p,yytext)==0) {
				 c=p-vm_instr_str;
				 yylval.lex_num=c;
				 switch(c) {
					case jmp:
					case jsr:
					case jeq_i:
					case jne_i:
					case li_i:
					case libp_i:
					case addsp:
					case libcall:
						return sym_opcode1;
					default:
						return sym_opcode;
				 }
			}
			p++;
	 }
}

"+" { return '+'; }
"-" { return '-'; }
":" { return ':'; }
"," { return ','; }

. { yyerror("Caractère non reconnu"); }

%%
	
int yywrap(void)
{
	 return 1;
}
	
