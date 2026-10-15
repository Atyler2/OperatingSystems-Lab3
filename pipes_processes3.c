#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  int pipefd[2];
  int pipefd2[2];



  char *search_term = (argc >1) ? argv[1] : "Lakers";
  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", search_term, NULL};
  char *sort_args[] = {"sort",NULL};


  if (pipe(pipefd2) == -1 ||pipe(pipefd) == -1 ){
    perror("pipe");
    exit(1);
  }

  // make a pipe (fds go in pipefd[0] and pipefd[1])


  pid_t pid = fork();
  if (pid == -1){
    perror("fork");
    exit(1);
  }
  if (pid == 0)
    {
      dup2(pipefd[1], STDOUT_FILENO);

      close(pipefd[0]);
      close(pipefd[1]);
      close(pipefd2[0]);
      close(pipefd2[1]);

      execvp("cat", cat_args);
      perror("execvp failed for cat");
      exit(1);

    }
    pid_t pid2 = fork();
      if (pid2 == 0){
        dup2(pipefd[0],STDIN_FILENO);

        dup2(pipefd2[1],STDOUT_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execvp("grep", grep_args);
        perror("execvp failed for grep");
        exit(1);
      }
      

    
    pid_t pid3 = fork();

    if (pid3 == -1){
    perror("fork");
    exit(1);
    }
  if (pid3 == 0){
    dup2(pipefd2[0],STDIN_FILENO);
    


     close(pipefd[0]);
     close(pipefd[1]);
     close(pipefd2[0]);
     close(pipefd2[1]);

      execvp("sort", sort_args);
      perror("execvp sort");
      exit(1);
      

  }

  close(pipefd[0]);
  close(pipefd[1]);
  close(pipefd2[0]);
  close(pipefd2[1]);

  waitpid(pid, NULL, 0);
  waitpid(pid2, NULL, 0);
  waitpid(pid3, NULL, 0);
  return 0; 

  

}