#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
int main()
{

        //get the current working dirictory
        char* current_dir = getcwd(NULL,0);
        if(current_dir != NULL)
        {

                printf("%s\n",current_dir);
                free(current_dir);
        }
        else{
                perror("getcwd");
                return -1;
	}        
}
