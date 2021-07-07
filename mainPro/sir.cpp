#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  struct stat st;
  stat("input.dat", &st);
  int size = st.st_size;

  char *buf = new char[st.st_size];

  int fd = open("input.dat", O_RDONLY);

  read(fd, buf, size);

  close(fd);
close(fd);

  //printf("%s", buf);

  // split it based on 1024

  int no_of_block = size/1024;

  if(size % 1024 > 0)
  {
    no_of_block++;
  }
  printf("total number of blocks:%d\n", no_of_block);
  delete [] buf;
}
