.module
 
.text
/* on reprend les variables globales */
  li_i _vars
	li_i _getvars
	jsr 4
	pop
	
/* lancement de 'main' puis 'exit' */
	li_i _argv
	ld_i
	li_i _argc
	ld_i
	li_i main
	jsr 8
	li_i exit
	jsr 4
	
/* variables globales */

.data
 .align 4
_vars:
stdin:
 .globl stdin
 .int 0
stdout:
 .globl stdout
 .int 0
stderr:
 .globl stderr
 .int 0
_argc:
 .int 0
_argv:
 .int 0



/* librairie */
.text

_getvars:
 libcall 0
 rts

malloc:
 .globl malloc
 libcall 1
 rts

free:
 .globl free
 libcall 2
 rts

exit:
 .globl exit
 libcall 3
 rts

realloc:
 .globl realloc
 libcall 4
 rts
 
fputc:
 .globl fputc
 libcall 5
 rts

fgetc:
 .globl fgetc
 libcall 6
 rts

fread:
 .globl fread
 libcall 7
 rts

fwrite:
 .globl fwrite
 libcall 8
 rts

ferror:
 .globl ferror
 libcall 9
 rts

fopen:
 .globl fopen
 libcall 10
 rts

fclose:
 .globl fclose
 libcall 11
 rts

