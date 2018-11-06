kind_t * getKindFromId(guint32 id) {
  guint i;
  /
  for (i = 0; i < nreloadkinds; i++) {
    if (id == kindidlist_uats[i].id) {
      return (kindidlist_uats+i);
    }
  }

  /
  {
    guint npredefinedkinds = sizeof(predefined_kinds);
    for (i = 0; i < npredefinedkinds; i++) {
      if (id == predefined_kinds[i].id) {
        return (predefined_kinds+i);
      }
    }
  }
  return NULL;
}
