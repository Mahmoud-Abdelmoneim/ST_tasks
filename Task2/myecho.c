#include "stdio.h"

int main(int argc, char *argv[])
{
	int y=0;
	if(argc > 1)
	{

		for( y=1;y<argc;y++){
			printf("%s",argv[y]);
	
			printf(" ");}
	}
	printf("\n");

return 0;
}
