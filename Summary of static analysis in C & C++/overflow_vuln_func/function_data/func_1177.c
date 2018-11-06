static gchar *dmp_national_sec_class (guint nation, guint dmp_sec_class)
{
  guint i;
  
  for (i = 0; i < num_dmp_security_classes; i++) {
    dmp_security_class_t *u = &(dmp_security_classes[i]);

    if (u->nation == nation && u->class == dmp_sec_class) {
      return u->name;
    }
  }

  return NULL;
}
