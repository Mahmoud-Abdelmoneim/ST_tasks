#include <stdio.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        printf("I am here > ");
        fflush(stdout);

        // Read user input
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("Error reading input");
            break;
        }

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';

        // Check for exit command
        if (strcmp(command, "exit") == 0) {
            printf("Good Bye :)\n");
            break;
        }

        // Echo the command
        printf("You said: %s\n", command);
    }

    return 0;
}
