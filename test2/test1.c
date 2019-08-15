#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void put(char c)
{
	 printf("c=%d\n",c);
}




int main(int argc,char *argv[])
{
	 int a=1,i;
	 char *buf1;
	 char *buf=(char *) 3+4;
	 FILE *f;
	 
	 buf=(void *) 2;
	 
	 a=2;
	 put(a);
	 fprintf(stderr,"stderr %d\n",23 << a);
	 printf("printf %d\n",23);
	 fwrite("hello\n",1,6,stdout);
	 buf=malloc(100);
	 strcpy(buf,"fabrice");
	 printf("strlen %s = %d\n",buf,strlen(buf));

	 f=fopen("tralala","w");
	 
	 for(i=10;i>=0;i--) {
			fprintf(f,"i=%d\n",i);
	 }
	 fclose(f);
	 return 0;
}

