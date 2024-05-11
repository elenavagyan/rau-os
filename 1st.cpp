#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    perror("invalid arguments\n");
    exit(EXIT_FAILURE);
  }

  int fd = open(argv[1], O_RDWR, 0666);
  if(fd == -1)
  {
    perror("couldn't open file\n");
    exit(EXIT_FAILURE);
  }
  
  struct stat file_stat;
  if(fstat(fd, &file_stat) == -1)
  {
    perror("couldn't get file's info\n");
    exit(EXIT_FAILURE);
  }
  std::string str;
  for(int x = 0; x < file_stat.st_size; ++x)
    {
    str += "\0";
    }
  int wr = write(fd, str.c_str(), file_stat.st_size);
  if(wr == -1)
  {
    perror("couldn't write to file\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}