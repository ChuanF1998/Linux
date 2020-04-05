#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void show_ids(void)
{
  printf ("real uid: %d\n", getuid());
  printf ("effective uid: %d\n", geteuid());

}

int main(int argc, char *argv[])
{

  int uid;
  show_ids();
  uid = atoi(argv[1]);

  if (setuid (uid) < 0) 
    perror ("setuid error");

  show_ids();

  return (0);

}

