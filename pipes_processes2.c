#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    // Basic argument check
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        return 1;
    }

    int pipefd[2], pipefd2[2];
    int pid1, pid2, pid3;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // Create first pipe: cat → grep
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create second pipe: grep → sort
    if (pipe(pipefd2) == -1) {
        perror("pipe");
        close(pipefd[0]);
        close(pipefd[1]);
        exit(1);
    }

    // -------------------------
    // P1: cat scores
    // -------------------------
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        close(pipefd[0]); close(pipefd[1]);
        close(pipefd2[0]); close(pipefd2[1]);
        exit(1);
    }

    if (pid1 == 0) {
        // Redirect stdout → write end of pipefd
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Close all pipe ends (child doesn't need them)
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execvp("cat", cat_args);
        perror("execvp cat");
        exit(1);
    }

    // -------------------------
    // P2: grep <pattern>
    // -------------------------
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        close(pipefd[0]); close(pipefd[1]);
        close(pipefd2[0]); close(pipefd2[1]);
        exit(1);
    }

    if (pid2 == 0) {
        // stdin ← read end of first pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // stdout → write end of second pipe
        if (dup2(pipefd2[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Close all pipe ends (not needed by this process)
        close(pipefd[0]); close(pipefd[1]);
        close(pipefd2[0]); close(pipefd2[1]);

        execvp("grep", grep_args);
        perror("execvp grep");
        exit(1);
    }

    // -------------------------
    // P3: sort
    // -------------------------
    pid3 = fork();
    if (pid3 < 0) {
        perror("fork");
        close(pipefd[0]); close(pipefd[1]);
        close(pipefd2[0]); close(pipefd2[1]);
        exit(1);
    }

    if (pid3 == 0) {
        // stdin ← read end of second pipe
        if (dup2(pipefd2[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Close all pipe ends
        close(pipefd[0]); close(pipefd[1]);
        close(pipefd2[0]); close(pipefd2[1]);

        execvp("sort", sort_args);
        perror("execvp sort");
        exit(1);
    }

    // -------------------------
    // Parent process
    // Close everything and wait
    // -------------------------
    close(pipefd[0]); close(pipefd[1]);
    close(pipefd2[0]); close(pipefd2[1]);

    int status;
    if (waitpid(pid1, &status, 0) == -1) perror("waitpid pid1");
    if (waitpid(pid2, &status, 0) == -1) perror("waitpid pid2");
    if (waitpid(pid3, &status, 0) == -1) perror("waitpid pid3");

    return 0;
}
