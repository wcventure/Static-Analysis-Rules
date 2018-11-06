int main (int argc, char *argv[])
{
  char path[1024] = "/";
  struct stat buf;

  strcat (path, argv[0]);
  if (lstat (".", &buf) != 0
      || !S_ISDIR (buf.st_mode))
    abort ();
  if (lstat (path, &buf) != 0
      || !S_ISREG (buf.st_mode))
    abort ();
  printf ("pass\n");
  exit (0);
}
