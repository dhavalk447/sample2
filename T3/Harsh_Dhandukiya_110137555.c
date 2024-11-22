#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// This function prints an error message and exits the program
void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int cat_to_grep[2], grep_to_wc[2];

    // Setting up the first pipe for cat to grep communication
    if (pipe(cat_to_grep) == -1) {
        handle_error("Failed to create the first pipe");
    }

    pid_t first_child = fork(); // Create the first child process
    if (first_child == -1) {
        handle_error("Failed to fork the first child process");
    }

    if (first_child == 0) {  // Code executed by the first child process
        close(cat_to_grep[0]); // Close the read end of the first pipe

        // Open the sample.txt file for reading
        int file_descriptor = open("sample.txt", O_RDONLY);
        if (file_descriptor == -1) {
            handle_error("Failed to open sample.txt");
        }

        // Redirect standard input to the file and standard output to the pipe
        dup2(file_descriptor, STDIN_FILENO);
        dup2(cat_to_grep[1], STDOUT_FILENO);

        close(file_descriptor);  
        close(cat_to_grep[1]);    

        // Execute the 'cat' command
        execlp("cat", "cat", (char *)NULL);
        handle_error("Failed to execute 'cat'");
    }

    // Setting up the second pipe for grep to wc communication
    if (pipe(grep_to_wc) == -1) {
        handle_error("Failed to create the second pipe");
    }

    pid_t second_child = fork(); // Create the second child process
    if (second_child == -1) {
        handle_error("Failed to fork the second child process");
    }

    if (second_child == 0) {  // Code executed by the second child process
        close(cat_to_grep[1]); 
        close(grep_to_wc[0]);  

        dup2(cat_to_grep[0], STDIN_FILENO);
        dup2(grep_to_wc[1], STDOUT_FILENO);

        close(cat_to_grep[0]); 
        close(grep_to_wc[1]);  

        // Execute the 'grep' command
        execlp("grep", "grep", "to", (char *)NULL);
        handle_error("Failed to execute 'grep'");
    }

    // Close all pipe descriptors in the parent process
    close(cat_to_grep[0]);
    close(cat_to_grep[1]);
    close(grep_to_wc[1]);

    // Redirect standard input to the read end of the second pipe
    dup2(grep_to_wc[0], STDIN_FILENO);
    close(grep_to_wc[0]);

    // Execute the 'wc' command with the '-w' option
    execlp("wc", "wc", "-w", (char *)NULL);
    handle_error("Failed to execute 'wc'");

    // Wait for both child processes to finish
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    return 0;
}