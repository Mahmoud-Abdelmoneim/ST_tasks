#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGUMENTS 10

void parseCommand(char *input, char *arguments[]) {
    int i = 0;
    char *token = strtok(input, " \n");

    while (token != NULL && i < MAX_ARGUMENTS) {
        arguments[i++] = token;
        token = strtok(NULL, " \n");
    }

    arguments[i] = NULL; 
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *arguments[MAX_ARGUMENTS];

    while (1) {
        // Print shell prompt
        printf("I am your Shell> ");
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Parse the command
        parseCommand(input, arguments);

        // Check if the user entered a command
        if (arguments[0] != NULL) {
            
            if (strcmp(arguments[0], "echo") == 0) {
                // Print arguments after 'echo'
                for (int i = 1; arguments[i] != NULL; i++) {
                    printf("%s ", arguments[i]);
                }
                printf("\n");
            } else if (strcmp(arguments[0], "pwd") == 0) {
                // Print the current working directory
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("getcwd() error");
                }
            } else if (strcmp(arguments[0], "cd") == 0) {
                // Change directory
                if (arguments[1] != NULL) {
                    if (chdir(arguments[1]) != 0) {
                        perror("cd error");
                    }
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
            } else if (strcmp(arguments[0], "exit") == 0) {
                // Exit the shell
                exit(EXIT_SUCCESS);
            } else {
                //printf("this command not supported now!\n");
                	
               pid_t returned_pid = fork();
               if (returned_pid > 0) {
                   int wstatus;
  
                   wait(&wstatus);
               } else if (returned_pid == 0) {
      
		
                   char *new_program_argv[100] = {arguments[0],arguments[1]};
                   
                   for (int i = 1; arguments[i] != NULL; i++) {
                      new_program_argv[i] = arguments[i];
                      //printf("==========%s",arguments[1]);
                   }

                   
                   execvp(arguments[0], new_program_argv);
                   
                   printf("I am not in the mode of execution. Exec failed\n");
            }
                                    
               
            }
        }
    }

    return 0;
}
