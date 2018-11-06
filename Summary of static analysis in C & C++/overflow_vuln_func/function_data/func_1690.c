static int
find_index_from_string_array(const char *needle, const char **haystack, int default_value)
{
  int i = 0;

  while (haystack[i] != NULL) {
    if (strcmp(needle, haystack[i]) == 0) {
      return i;
    }
    i++;
  }
  return default_value;
}
