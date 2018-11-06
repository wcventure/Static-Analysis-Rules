#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main (int argc, char *argv[])
{
  FILE *f = fopen ("check_openpf2.c", "rb");
  if (f == NULL)
    abort ();
  close (f);
  printf ("pass\n");
  return 0;
}
