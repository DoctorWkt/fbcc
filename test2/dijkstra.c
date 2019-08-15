#include <stdlib.h>
#include <stdio.h>

#define N 4
#define K 3

int state_nb;
char *state_tab;
int x[5 /* à changer */];

int state_mark(int m)
{
	 int s,i,tmp;
	 s=0;
	 for(i=N;i>=0;i--) s=s*K+x[i];
	 tmp=state_tab[s];
	 state_tab[s]=m;
	 return tmp;
}

int phi(int n)
{
	 return (n+1) % K;
}

void state_test(void) 
{
	 int tmp,i;
	 int priv_nb;
	 
	 priv_nb=0;
	 if (x[0]==x[N]) priv_nb++;
	 for(i=1;i<=N;i++) if (x[i]!=x[i-1]) priv_nb++;

	 for(i=0;i<=N;i++) printf("%d ",x[i]);
	 printf(" priv_nb= %d\n",priv_nb);
	 if (priv_nb<2) return;

	 tmp=state_mark(1);
	 if (tmp) {
			printf("Found!!!\n");
			exit(0);
	 }

	 if (x[0]==x[N]) {
			tmp=x[0];
			x[0]=phi(x[0]);
			state_test();
			x[0]=tmp;
	 } else {
			for(i=1;i<=N;i++) 
				if (x[i]!=x[i-1]) {
					 tmp=x[i];
					 x[i]=x[i-1];
					 state_test();
					 x[i]=tmp;
				}
	 }
	 
}

	 
int main(void)
{
	 int i;
	 
	 state_nb=1;
	 for(i=0;i<=N;i++) state_nb*=K;
	 printf("NUmber of states: %d\n",state_nb);

	 state_tab=malloc(state_nb);
	 for(i=0;i<state_nb;i++) state_tab[i]=0;
	 

	 x[0]=0; x[1]=0; x[2]=2; x[3]=1; x[4]=0;
	 state_test();
	 return(0);
}

	 
						
