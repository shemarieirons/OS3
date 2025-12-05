#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int fd1[2];
    int fd2[2];
    int pid;
    char input_str[100];
    char buffer[200];

    pipe(fd1);
    pipe(fd2);

    pid = fork();

    if (pid == 0)
    {
        // Child reads from fd1 and writes to fd2
        close(fd1[1]);
        close(fd2[0]);

        read(fd1[0], buffer, sizeof(buffer));
        close(fd1[0]);

        strcat(buffer, "howard.edu");
        printf("Output : %s\n", buffer);

        printf("Input : ");
        fgets(input_str, sizeof(input_str), stdin);
        input_str[strcspn(input_str, "\n")] = 0;

        strcat(buffer, input_str);

        write(fd2[1], buffer, strlen(buffer) + 1);
        close(fd2[1]);
    }
    else
    {
        // Parent writes to fd1 and reads from fd2
        close(fd1[0]);
        close(fd2[1]);

        printf("Input : ");
        fgets(input_str, sizeof(input_str), stdin);
        input_str[strcspn(input_str, "\n")] = 0;

        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        wait(NULL);  // ensures child prints its output before parent continues

        read(fd2[0], buffer, sizeof(buffer));
        close(fd2[0]);

        // Parent prints final result
        strcat(input_str, "gobison.org");
        printf("Output : %s\n", input_str);
    }

    return 0;
}
