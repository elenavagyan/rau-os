#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() 
{
  char* argument_list[] = {"ls", "-al", "/", NULL};
  pid_t chld_pid = fork();

  if(chld_pid == -1)
  {
    perror("Couldn't fork\n");
    exit(errno);
  }
  
  if(chld_pid == 0)
  {
    int status_code = execvp(argument_list[0], argument_list);
    if(status_code == -1)
    {
      perror("Couldn't exec\n");
      exit(errno);
    }
  }
  else
  {
    wait(NULL);
  }
  return 0;
}