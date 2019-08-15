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
#include "fbvmspec.h"
#include "fbvminstr.h"

/* #define DEBUG */

typedef unsigned char VMCodeData;
typedef unsigned char VMData;
typedef int VMStackData;


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


int vm_argc;
char **vm_argv;

void VMLibCall(int *sp,int *bp,int c)
{
	 int a;
	 int *p;

	 switch(c) {
		case 0:
			/* getvars */
			p=(void *)bp[-3];
			p[0]=(int)stdin;
			p[1]=(int)stdout;
			p[2]=(int)stderr;
			p[3]=vm_argc;
			p[4]=(int)vm_argv;
			sp[0]=0;
			break;
		case 1:
			/* malloc */
			sp[0]=(int)malloc(bp[-3]);
			break;
		case 2:
			/* free */
			free((void *)bp[-3]);
			sp[0]=0;
			break;
		case 3:
			/* exit */
			exit(bp[-3]);
			break;
		case 4:
			/* realloc */
			p=(void *)bp[-3];
			a=bp[-4];
			sp[0]=(int) realloc(p,a);
			break;
		case 5:
			/* fputc */
			sp[0]=fputc(bp[-3],(FILE *)bp[-4]);
			break;
		case 6:
			/* fgetc */
			sp[0]=fgetc((FILE *)bp[-3]);
			break;
		case 7:
			/* fread */
			sp[0]=fread((void *)bp[-3],bp[-4],bp[-5],(FILE *)bp[-6]);
			break;
		case 8:
			/* fwrite */
			sp[0]=fwrite((void *)bp[-3],bp[-4],bp[-5],(FILE *)bp[-6]);
			break;
		case 9:
			/* ferror */
			sp[0]=ferror((FILE *)bp[-3]);
			break;
		case 10:
			/* fopen */
			sp[0]=(int)fopen((char *)bp[-3],(char *)bp[-4]);
			break;
		case 11:
			/* fclose */
			sp[0]=fclose((FILE *)bp[-3]);
			break;
		default:
			fprintf(stderr,"libcall %d non implémenté\n",c);
			break;
	 }
}



void VMExec(VMCodeData *init_ip,VMStackData *init_sp)
{
	 register VMCodeData *ip;
	 register int *sp;
	 int *bp;
	 int op_code;
	 VMCodeData *ip1;
	 int a,b;
	 
	 ip=init_ip;
	 sp=init_sp;
	 bp=init_sp;
	 
	 while (1) {
			op_code=*ip++;
#ifdef DEBUG
			fprintf(stderr,"debug: sp=%d bp=%d sp[0]=%d %s\n",
							sp-init_sp,bp-init_sp,sp[0],vm_instr_str[op_code]);
#endif
			switch(op_code) {
				 /* memory read */
			 case ld_b:
				 sp[0]=*((char *)sp[0]);
				 break;
			 case ld_ub:
				 sp[0]=*((unsigned char *)sp[0]);
				 break;
			 case ld_w:
				 sp[0]=*((short *)sp[0]);
				 break;
			 case ld_uw:
				 sp[0]=*((unsigned short *)sp[0]);
				 break;
			 case ld_i:
				 sp[0]=*((int *)sp[0]);
				 break;
				 
				 /* memory write */
			 case st_i:
				 *((int *)sp[0])=sp[-1];
				 sp--;
				 break;
			 case st_w:
				 *((short *)sp[0])=sp[-1];
				 sp--;
				 break;
			 case st_b:
				 *((char *)sp[0])=sp[-1];
				 sp--;
				 break;
				 
				 /* load immediate pointers */
			 case libp_i:
				 sp++;
				 sp[0]=(int)bp + mget_i(ip);
				 ip+=4;
				 break;

				 /* load immediate data */
			 case li_i:
				 sp++;
				 sp[0]=mget_i(ip);
				 ip+=4;
				 break;

				 /* arithmétique */
			 case add_i:
				 sp[-1]+=sp[0];
				 sp--;
				 break;
			 case sub_i:
				 sp[-1]-=sp[0];
				 sp--;
				 break;
			 case mul_i:
				 sp[-1]*=sp[0];
				 sp--;
				 break;
			 case div_i:
				 sp[-1]/=sp[0];
				 sp--;
				 break;
			 case mod_i:
				 sp[-1]%=sp[0];
				 sp--;
				 break;
			 case div_ui:
				 sp[-1] = (unsigned int)sp[-1] / (unsigned int)sp[0];
				 sp--;
				 break;
			 case mod_ui:
				 sp[-1] = (unsigned int)sp[-1] % (unsigned int)sp[0];
				 sp--;
				 break;
			 case neg_i:
				 sp[0]=-sp[0];
				 break;

				 /* opérations logiques */
			 case and_i:
				 sp[-1]&=sp[0];
				 sp--;
				 break;
			 case or_i:
				 sp[-1]|=sp[0];
				 sp--;
				 break;
			 case xor_i:
				 sp[-1]^=sp[0];
				 sp--;
				 break;
			 case not_i:
				 sp[0]=~sp[0];
				 break;
			 case shl_i:
				 sp[-1]<<=sp[0];
				 sp--;
				 break;
			 case shr_i:
				 sp[-1]>>=sp[0];
				 sp--;
				 break;
			 case shr_ui:
                                 sp[-1] = (unsigned int)sp[-1] >> sp[0];
				 sp--;
				 break;
				 
				 /* conversions */
			 case cvt_i_b:
				 sp[0]=(char)sp[0];
				 break;
			 case cvt_b_i:
			 case cvt_i_ub:
				 sp[0]=(unsigned char)sp[0];
				 break;
			 case cvt_i_w:
				 sp[0]=(short)sp[0];
				 break;
			 case cvt_w_i:
			 case cvt_i_uw:
				 sp[0]=(unsigned short)sp[0];
				 break;
				 
				 
				 
				 /* tests */
			 case cmplt_i:
				 sp[-1]=sp[-1] < sp[0];
				 sp--;
				 break;
			 case cmple_i:
				 sp[-1]=sp[-1] <= sp[0];
				 sp--;
				 break;
			 case cmpgt_i:
				 sp[-1]=sp[-1] > sp[0];
				 sp--;
				 break;
			 case cmpge_i:
				 sp[-1]=sp[-1] >= sp[0];
				 sp--;
				 break;

			 case cmplt_ui:
				 sp[-1]=(unsigned int)sp[-1] < (unsigned int)sp[0];
				 sp--;
				 break;
			 case cmple_ui:
				 sp[-1]=(unsigned int)sp[-1] <= (unsigned int)sp[0];
				 sp--;
				 break;
			 case cmpgt_ui:
				 sp[-1]=(unsigned int)sp[-1] > (unsigned int)sp[0];
				 sp--;
				 break;
			 case cmpge_ui:
				 sp[-1]=(unsigned int)sp[-1] >= (unsigned int)sp[0];
				 sp--;
				 break;
			 
			 case cmpeq_i:
				 sp[-1]=sp[-1] == sp[0];
				 sp--;
				 break;
			 case cmpne_i:
				 sp[-1]=sp[-1] != sp[0];
				 sp--;
				 break;
				 

				 /* subroutines */
			 case jsr:
				 a=mget_i(ip);
				 ip+=4;
				 ip1=(void *)sp[0];
				 sp[0]=(int)ip;
				 sp[1]=(int)bp;
				 sp[2]=a;
				 sp+=2;
				 ip=ip1;
				 bp=sp;
				 break;

			 case rts:
				 a=sp[0];  /* valeur retournée */
				 sp=bp;
				 ip=(void *)sp[-2];
				 bp=(void *)sp[-1];
				 b=sp[0];
				 sp=(int *)((long)sp-(b+8));
				 sp[0]=a;
				 break;

			 /* jumps */
			 case jmp:
				 ip1=(void *)mget_i(ip);
				 ip=ip1;
				 break;
			 
			 case jeq_i:
				 ip1=(void *)mget_i(ip);
				 ip+=4;
				 if (sp[0]==0) ip=ip1;
				 sp--;
				 break;
			 
			 case jne_i:
				 ip1=(void *)mget_i(ip);
				 ip+=4;
				 if (sp[0]!=0) ip=ip1;
				 sp--;
				 break;
			 
				 /* gestion de la pile */
			 case addsp:
				 a=mget_i(ip);
				 ip+=4;
				 sp=(void *)((int)sp + a);
				 break;
				 
			 case op_dup:
				 sp++;
				 sp[0]=sp[-1];
				 break;
				 
			 case pop:
				 sp--;
				 break;
				 
				 /* appels systeme */
			 case libcall:
				 a=mget_i(ip);
				 ip+=4;
				 sp++;
				 VMLibCall(sp,bp,a);
				 break;
				 
				 /* gestion des switchs : inspiré de java :) */
			 case switch_i:
				 {
						int val,count;
						int *tab,*p,*p_end;
						
						val=sp[-1];
						tab=(int *)sp[0];
						sp-=2;
						
						p=tab;
						count=*p++;
						p_end=tab+count;
						while (p<p_end) {
							 if (val == *p) {
									ip=(VMCodeData *) *(p_end+(p-tab));
									goto switch_end;
							 }
							 p++;
						}
						ip=(VMCodeData *) *p_end;
						switch_end:
						break;
				 }
			 default:
				 fprintf(stderr,"opcode not implemented: %s\n",vm_instr_str[op_code]);
				 return;
			}
	 }
}


int fget_i(FILE *f)
{
	 int a;
	 a=fgetc(f) << 24;
	 a|=fgetc(f) << 16;
	 a|=fgetc(f) << 8;
	 a|=fgetc(f);
	 return a;
}

/*
 * Chargement et relogeage de l'éxécutable
 */

void VMLoadCode(unsigned char **code1,int *stack_size1,char *filename)
{
	 FILE *f;
	 int i,offset,adr,code_size,stack_size,reloc_nb,magic;
	 unsigned char *code;
	 
	 f=fopen(filename,"r");
	 if (f==NULL) {
			perror(filename);
			exit(1);
	 }
	 
	 magic=fget_i(f);
	 if (magic!=FBVM_MAGIC) {
			fprintf(stderr,"Bad magic number\n");
			exit(1);
	 }
	 
	 reloc_nb=fget_i(f);
	 code_size=fget_i(f);
	 stack_size=fget_i(f);
#ifdef DEBUG
	 fprintf(stderr,
					 "Code size=%d Stack size=%d Relocations=%d\n",
					 code_size,stack_size,reloc_nb);
#endif
	 code=malloc(code_size);
	 fread(code,code_size,1,f);
	 
	 /* relocation */
	 for(i=0;i<reloc_nb;i++) {
			offset=fget_i(f);
			adr=mget_i(code+offset);
			adr+=(int)(code);
			mput_i(code+offset,adr);
	 }
	 
	 fclose(f);
	 
	 *code1=code;
	 *stack_size1=stack_size;
}


void print_help(void)
{
  printf("usage: fbvm code_file\n"
	 "Virtual Machine Interpreter (version 1.00) (c) 1996 Fabrice Bellard\n"
	 "(compiled for %s)\n"
	 "\n",VM_ARCH_NAME
	 );
}

int main(int argc,char *argv[])
{
	 VMData *code;
	 VMStackData *stack;
	 int stack_size;
	 
	 if (argc < 2) {
			print_help();
			exit(0);
	 }

	 VMLoadCode(&code,&stack_size,argv[1]);
	 stack=malloc(stack_size);

	 vm_argc=argc-1;
	 vm_argv=argv+1;

	 VMExec(code,stack);

	 free(code);
	 free(stack);

	 return 0;
}
