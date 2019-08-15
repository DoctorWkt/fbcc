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
 
 #define YY_ALWAYS_INTERACTIVE 1
%}

delim [\t\r\x0C ]
hexdigit [0-9A-Fa-f]

ident [a-zA-Z_][a-zA-Z_0-9]*

char_esc n|t|v|b|r|f|a|\\|\?|\"|\'|[0-7]+|x{hexdigit}+

%x COMMENT
%x STRING
%x PREPROCESS

%%

"/*"                   { /* gestion de commentaires C reprise de la
													documentation de flex */
	                      BEGIN(COMMENT); 
                        }
<COMMENT>[^*\n]*       /* eat anything that's not a '*' */
<COMMENT>"*"+[^*/\n]*  /* eat up '*'s not followed by '/'s */
<COMMENT>\n            { line_current++; }
<COMMENT>"*/"          { BEGIN(INITIAL); }

^"#"          { /* commentaire préprocesseur */ 
                BEGIN(PREPROCESS);
							}
<PREPROCESS>.  { }
<PREPROCESS>\n { line_current++; BEGIN(INITIAL); }

\"                 { 
	 BEGIN(STRING); 
	 lex_string_size=0;
}

<STRING>[^\"\\]      { 
	 Lex_AddString(yytext[0]);
}

<STRING>\\{char_esc} {
	 Lex_AddString(Lex_CharEsc(yytext));
}

<STRING>\"         {
	 BEGIN(INITIAL); 
	 yylval=mk_buf(lex_string,lex_string_size,NULL);
   return sym_const_str; 
}

{delim} { }
\n      { line_current++; }

0[xX]{hexdigit}+ { /* entier hexadécimal */
	 int sym,num;
	 Lex_Integer(&sym,&num,yytext+2,16);
	 yylval=mk_int(num,NULL);
	 return sym;
}

[1-9][0-9]*  { /* entier décimal */
	 int sym,num;
	 Lex_Integer(&sym,&num,yytext,10);
	 yylval=mk_int(num,NULL);
	 return sym;
}

[0-7]+ { /* entier octal */
	 int sym,num;
	 Lex_Integer(&sym,&num,yytext,8);
	 yylval=mk_int(num,NULL);
	 return sym;
}
	 
\'.\'            { 
	 yylval=mk_int(yytext[1],NULL); 
	 return sym_const_char; 
}
\'\\{char_esc}\' {
	 yylval=mk_int(Lex_CharEsc(yytext+1),NULL);
	 return sym_const_char;
}

if       { return sym_if; }
else     { return sym_else; }
while    { return sym_while; }
do       { return sym_do; }
for      { return sym_for; }
break    { return sym_break; }
continue { return sym_continue; }
switch   { return sym_switch; }
case     { return sym_case; }
default  { return sym_default; }
return   { return sym_return; }
goto     { return sym_goto; }
sizeof   { return sym_sizeof; }

void     { return sym_void; }
char     { return sym_char; }
short    { return sym_short; }
int      { return sym_int; }
struct   { return sym_struct; }
union    { return sym_union; }
enum     { return sym_enum; }
unsigned { return sym_unsigned; }
signed   { return sym_signed; }

const    { return sym_const; }
volatile { return sym_volatile; }

auto     { return sym_auto; }
register { return sym_register; }
static   { return sym_static; }
extern   { return sym_extern; }
typedef  { return sym_typedef; }


{ident}   {  
	 SYM *s;
	 s=Sym_Search(yytext,TABLE_VAR);
	 if (s!=NULL && hd_tag(s->list)==TYPE_TYPEDEF_IDENT) {
		 yylval=mk_sym(s,NULL);
		 return sym_typedef_ident;
	 } else { 
			yylval=mk_str(yytext,NULL);
			return sym_ident;
	 }
}

";"    { return ';'; }
","    { return ','; }
"."    { return '.'; }
"("    { return '('; }
")"    { return ')'; }
"{"    { return '{'; }
"}"    { return '}'; }
"["    { return '['; }
"]"    { return ']'; }


"<"    { return '<'; }
">"    { return '>'; }
"<="   { return sym_le; }
">="   { return sym_ge; }
"=="   { return sym_eq; }
"!="   { return sym_ne; }


"&&"   { return sym_land; }
"||"   { return sym_lor; }
"+"    { return '+'; }
"-"    { return '-'; }
"*"    { return '*'; }
"/"    { return '/'; }
"%"    { return '%'; }
"&"    { return '&'; }
"|"    { return '|'; }
"!"    { return '!'; }
"~"    { return '~'; }
"^"    { return '^'; }
"<<"   { return sym_shl; }
">>"   { return sym_shr; }

"="    { return '='; }
"++"   { return sym_inc; }
"--"   { return sym_dec; }
"+="   { return sym_assign_add; }
"-="   { return sym_assign_sub; }
"*="   { return sym_assign_mul; }
"/="   { return sym_assign_div; }
"%="   { return sym_assign_mod; }
"&="   { return sym_assign_and; }
"|="   { return sym_assign_or; }
"^="   { return sym_assign_xor; }
"<<="   { return sym_assign_shl; }
">>="   { return sym_assign_shr; }


"?"    { return '?'; }
":"    { return ':'; }
"..."  { return sym_three_points; }
"->"   { return sym_arrow; }


.      { Error("Caractère non reconnu: '%s'",yytext); }

%%

int yywrap(void)
{
	 return 1;
}

char lex_string[STRING_SIZE_MAX];
int lex_string_size;


void Lex_AddString(int c)
{
  if (lex_string_size<STRING_SIZE_MAX) {
			lex_string[lex_string_size]=c;
			lex_string_size++;
	 } else {
			Error("Constante chaîne trop longue");
	 }
}

int Lex_CharEsc(char *str)
{
	 int a,c;
	 char *p;

	 a=str[1];
	 switch(a) {
		case 'n':
			c='\n';
			break;
		case 'r':
			c='\r';
			break;
		case 't':
			c='\t';
			break;
		case '\\':
			c='\\';
			break;
		case '\'':
			c='\'';
			break;
		default:
			if (a>='0' && a<='7') {
				 c=a - '0';
				 p=&str[2];
				 while ( *p >= '0' && *p <='7' ) {
						c=c*8 + (*p - '0');
						p++;
				 }
			} else {
				 Error_Internal("Constante 'char' non implémentée");
				 c=0;
			}
			break;
	 }
	 return c;
}

void Lex_Integer(int *sym_ptr,int *num_ptr,char *str,int base)
{
	 char *p;
	 int num,n;

	 p=str;
	 num=0;
	 while (*p != 0) {
			n=*p;
			if (n>='a') n=n-'a'+10;
			else if (n>='A') n=n-'A'+10;
			else n=n-'0';
			num=num*base+n;
			p++;
	 }
	 
	 *num_ptr=num;
	 *sym_ptr=sym_const_int;
}
