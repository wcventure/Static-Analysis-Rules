static void build_national_strings (void) 
{
  gint i = 0;

  /
  while (dmp_national_values[i].name && i < MAX_NATIONAL_VALUES) {
    nat_pol_id[i].value  = dmp_national_values[i].value;
    nat_pol_id[i].strptr = dmp_national_values[i].description;
    i++;
  }
  nat_pol_id[i].value = 0;
  nat_pol_id[i].strptr = NULL;
}
