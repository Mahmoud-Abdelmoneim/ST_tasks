#include "stdio.h"
#include "stdlib.h"
int main(int argc,char* argv[])
{

 if(argc != 3 )
 {
	printf("Usage Case: %s <source> <Distenation>\n",argv[0]);
 }	 
 char* src = argv[1];
 char* dit = argv[2];
 if(rename(src,dit) == 0)
 {
	 printf("file %s moved to '%s'\n)",src,dit);
 }
 else{
printf("error");
 }
 return 0;

}
