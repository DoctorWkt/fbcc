#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

void print_help(void)
{
  printf("This is the help.\n");
}


int main(int argc,char *argv[])
{
  int i,c;

  printf("argc=%d ",argc);
  for(i=0;i<argc;i++) printf(" %s",argv[i]);
  printf("\n");
  
  while (1) {
    c=getopt(argc,argv,"h?abc:");
    if (c==-1) break;
    switch (c) {
     case 'h':
     case '?':
      print_help();
      exit(0);
      break;
     case 'a':
      printf("Option 'a'\n");
      break;
     case 'b':
      printf("Option 'b'\n");
      break;
     case 'c':
      printf("Option 'c' with %s\n",optarg);
      break;
    }
  }
  
  printf("parameters=");
  for(i=optind;i<argc;i++) printf(" %s",argv[i]);
  printf("\n");
  return 0;
}

