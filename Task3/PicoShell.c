#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGUMENTS 10

void parseCommand(char *input, char ***arguments) {
    int i = 0;
    char *token = strtok(input, " \n");

    while (token != NULL && i < MAX_ARGUMENTS - 1) {
        (*arguments)[i++] = strdup(token);
        token = strtok(NULL, " \n");
    }

    (*arguments)[i] = NULL;
}

void freeArguments(char **arguments) {
    for (int i = 0; arguments[i] != NULL; i++) {
        free(arguments[i]);
    }
    free(arguments);
}

void executeCommand(char **arguments) {
    pid_t returned_pid = fork();

    if (returned_pid < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (returned_pid == 0) {
        if (execvp(arguments[0], arguments) == -1) {
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
    } else {
        int wstatus;
        if (waitpid(returned_pid, &wstatus, 0) == -1) {
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    char *input = NULL;
    size_t input_size = 0;
    char **arguments = NULL;

    while (1) {
        // Print shell prompt
        printf("I am your Shell> ");
        fflush(stdout);

        // Read user input
        if (getline(&input, &input_size, stdin) == -1) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Parse the command
        arguments = (char **)malloc(MAX_ARGUMENTS * sizeof(char *));
        parseCommand(input, &arguments);

        // Check if the user entered a command
        if (arguments[0] != NULL) {
            if (strcmp(arguments[0], "echo") == 0) {
                // Handle 'echo' command
                for (int i = 1; arguments[i] != NULL; i++) {
                    printf("%s ", arguments[i]);
                }
                printf("\n");
            } else if (strcmp(arguments[0], "pwd") == 0) {
                // Handle 'pwd' command
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("getcwd() error");
                }
            } else if (strcmp(arguments[0], "cd") == 0) {
                // Handle 'cd' command
                if (arguments[1] != NULL) {
                    if (chdir(arguments[1]) != 0) {
                        perror("chdir error");
                    }
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
            } else if (strcmp(arguments[0], "exit") == 0) {
                // Handle 'exit' command
                freeArguments(arguments);
                free(input);
                exit(EXIT_SUCCESS);
            } else {
                // Execute external command
                executeCommand(arguments);
            }

            // Free memory allocated for arguments
            freeArguments(arguments);
        }
    }

    free(input);  // Free memory allocated for input

    return 0;
}
