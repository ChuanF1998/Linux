#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>


int main()
{
  std::cout << getpid() << std::endl;
  std::cout << getppid() << std::endl;

  return 0;
}
