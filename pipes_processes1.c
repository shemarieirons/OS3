// C program to demonstrate use of fork() and pipe() 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() 
{
    int fd1[2];  // Parent → Child
    int fd2[2];  // Child → Parent

    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";

    char input_str[100];
    char second_input[100];

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe Failed");
        exit(1);
    }

    printf("Input : ");
    if (scanf("%99s", input_str) != 1) {
        fprintf(stderr, "Input Failed\n");
        exit(1);
    }

    pid_t p = fork();

    if (p < 0) {
        perror("fork Failed");
        exit(1);
    }

    /* ===========================
         PARENT PROCESS (P1)
       =========================== */
    if (p > 0) 
    {
        close(fd1[0]);  // P1 doesn't read from pipe1
        close(fd2[1]);  // P1 doesn't write to pipe2

        // Send first input to child
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        // Wait for P2 to finish
        wait(NULL);

        // Receive processed string from P2
        char received_str[300];
        read(fd2[0], received_str, sizeof(received_str));
        close(fd2[0]);

        // Append gobison.org
        strcat(received_str, fixed_str2);

        printf("Output : %s\n", received_str);
    } 

    /* ===========================
         CHILD PROCESS (P2)
       =========================== */
    else 
    {
        close(fd1[1]);  // P2 doesn't write to pipe1
        close(fd2[0]);  // P2 doesn't read from pipe2

        // Read initial string from P1
        char concat_str[300];
        read(fd1[0], concat_str, sizeof(concat_str));
        close(fd1[0]);

        // Append howard.edu
        strcat(concat_str, fixed_str1);

        // Print first concatenation
        printf("Output : %s\n", concat_str);

        // Prompt for second input
        printf("Second Input : ");
        if (scanf("%99s", second_input) != 1) {
            fprintf(stderr, "Input Failed\n");
            exit(1);
        }

        // Append second input
        strcat(concat_str, second_input);

        // Send result back to P1
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}