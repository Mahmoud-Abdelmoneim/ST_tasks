#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

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

int main() {
    char *input = NULL;
    size_t input_size = 0;
    char **arguments = NULL;

    while (1) {
        
        printf("I am your Shell> ");
        fflush(stdout);

        
        if (getline(&input, &input_size, stdin) == -1) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        
        input[strcspn(input, "\n")] = '\0';

        
        arguments = (char **)malloc(MAX_ARGUMENTS * sizeof(char *));
        parseCommand(input, &arguments);

        
        if (arguments[0] != NULL) {
            if (strcmp(arguments[0], "echo") == 0) {
                // Handle 'echo' command
                for (int i = 1; arguments[i] != NULL; i++) {
                    printf("%s ", arguments[i]);
                }
                printf("\n");
            } 
            else if (strcmp(arguments[0], "pwd") == 0) {
                // Handle 'pwd' command
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("%s\n", cwd);
                } else {
                    perror("getcwd() error");
                }
            } 
            else if (strcmp(arguments[0], "cd") == 0) {
                // Handle 'cd' command
                if (arguments[1] != NULL) {
                    if (chdir(arguments[1]) != 0) {
                        perror("chdir error");
                    }
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
            } 
            else if (strcmp(arguments[0], "exit") == 0) {
                // Handle 'exit' command
                freeArguments(arguments);
                free(input);
                exit(EXIT_SUCCESS);
            } 
            else {
                int pipe_index = -1;
                int input_fd = STDIN_FILENO;
                int output_fd = STDOUT_FILENO;
                int redirection_type = 0;
                char *output_file = NULL;

                // Check for I/O redirection
                for (int i = 0; arguments[i] != NULL; i++) {
                    if (strcmp(arguments[i], "<") == 0) {
                        redirection_type = 1; // Input redirection
                        input_fd = open(arguments[i+1], O_RDONLY);
                        if (input_fd < 0) {
                            perror("open input file error");
                            exit(EXIT_FAILURE);
                        }
                        arguments[i] = NULL;
                    } 
                    else if (strcmp(arguments[i], ">") == 0) {
                        redirection_type = 2; // Output redirection
                        output_fd = open(arguments[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        if (output_fd < 0) {
                            perror("open output file error");
                            exit(EXIT_FAILURE);
                        }
                        arguments[i] = NULL;
                    } 
                    else if (strcmp(arguments[i], ">>") == 0) {
                        redirection_type = 3; // Output redirection (append)
                        output_fd = open(arguments[i+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
                        if (output_fd < 0) {
                            perror("open output file error");
                            exit(EXIT_FAILURE);
                        }
                        arguments[i] = NULL;
                    } 
                    else if (strcmp(arguments[i], "|") == 0) {
                        pipe_index = i;
                    }
                }

                if (pipe_index != -1) {
                    // Handle piping
                    int pipefd[2];
                    pipe(pipefd);

                    pid_t pid1 = fork();
                    if (pid1 == 0) {
                       
                        close(pipefd[0]); 
                        dup2(pipefd[1], STDOUT_FILENO); 
                        close(pipefd[1]); 

                        if (redirection_type == 1) {
                            dup2(input_fd, STDIN_FILENO); 
                            close(input_fd);
                        }

                        if (execvp(arguments[0], arguments) == -1) {
                            perror("execvp error");
                            exit(EXIT_FAILURE);
                        }
                    }

                    pid_t pid2 = fork();
                    if (pid2 == 0) {
                        
                        close(pipefd[1]); 
                        dup2(pipefd[0], STDIN_FILENO); 
                        close(pipefd[0]); 

                        if (redirection_type == 2 || redirection_type == 3) {
                            dup2(output_fd, STDOUT_FILENO); 
                            close(output_fd);
                        }

                        if (execvp(arguments[pipe_index+1], &arguments[pipe_index+1]) == -1) {
                            perror("execvp error");
                            exit(EXIT_FAILURE);
                        }
                    }

                    close(pipefd[0]);
                    close(pipefd[1]);

                    waitpid(pid1, NULL, 0);
                    waitpid(pid2, NULL, 0);
                } else {
                    // Execute external command
                    pid_t pid = fork();
                    if (pid == 0) {
                        
                        if (redirection_type == 1) {
                            dup2(input_fd, STDIN_FILENO); 
                            close(input_fd);
                        }
                        if (redirection_type == 2 || redirection_type == 3) {
                            dup2(output_fd, STDOUT_FILENO); 
                            close(output_fd);
                        }
                        if (execvp(arguments[0], arguments) == -1) {
                            perror("execvp error");
                            exit(EXIT_FAILURE);
                        }
                    } else if (pid < 0) {
                        
                        perror("fork error");
                        exit(EXIT_FAILURE);
                    } else {
                        /
                        wait(NULL);
                    }
                }

                if (input_fd != STDIN_FILENO) {
                    close(input_fd);
                }
                if (output_fd != STDOUT_FILENO) {
                    close(output_fd);
                }
                if (output_file != NULL) {
                    free(output_file);
                }
            }

            
            freeArguments(arguments);
        }
    }

    free(input);  

    return 0;
}
