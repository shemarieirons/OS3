#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>  // For wait

/**
 * Executes the command "cat scores | grep <argv[1]> | sort".
 * Creates three processes: P1 (executes cat scores), P2 (executes grep <argv[1]>),
 * and P3 (executes sort). The parent waits for all children to finish.
 */

int main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
    return 1;
  }

  int pipefd[2], pipefd2[2];
  int pid1, pid2, pid3;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  // Create pipes
  pipe(pipefd);
  pipe(pipefd2);

  // Fork P1 for cat
  pid1 = fork();
  if (pid1 == 0) {
    // P1: cat scores
    dup2(pipefd[1], 1);  // stdout to pipefd[1]
    close(pipefd[0]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    execvp("cat", cat_args);
  }

  // Fork P2 for grep
  pid2 = fork();
  if (pid2 == 0) {
    // P2: grep <argv[1]>
    dup2(pipefd[0], 0);   // stdin from pipefd[0]
    dup2(pipefd2[1], 1);  // stdout to pipefd2[1]
    close(pipefd[1]);
    close(pipefd2[0]);
    execvp("grep", grep_args);
  }

  // Fork P3 for sort
  pid3 = fork();
  if (pid3 == 0) {
    // P3: sort
    dup2(pipefd2[0], 0);  // stdin from pipefd2[0]
    close(pipefd[0]);
    close(pipefd[1]);
    close(pipefd2[1]);
    execvp("sort", sort_args);
  }

  // Parent: Close all pipe ends and wait for children
  close(pipefd[0]);
  close(pipefd[1]);
  close(pipefd2[0]);
  close(pipefd2[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  waitpid(pid3, NULL, 0);

  return 0;
}
