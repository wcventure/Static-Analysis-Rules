#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int main (int argc, char *argv[])
{
  char *fnam = argv[0];
  FILE *f;
  if (argv[0][0] != '/')
    {
      fnam = malloc (strlen (argv[0]) + 2);
      if (fnam == NULL)
	abort ();
      strcpy (fnam, "/");
      strcat (fnam, argv[0]);
    }

  f = fopen (fnam, "rb");
  if (f == NULL)
    abort ();
  close (f);

  /
  if (fopen ("/nonexistent", "rb") != NULL
      || errno != ENOENT)
    abort ();
  printf ("pass\n");
  return 0;
}
