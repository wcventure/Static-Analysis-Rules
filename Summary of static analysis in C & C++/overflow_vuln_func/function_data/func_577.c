PRIVATE int strhash(const char *x)
{
  int h = 0;
  while( *x) h = h*13 + *(x++);
  return h;
}
