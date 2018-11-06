char *
range_convert_range(range_t *range, char *string)
{
   guint32 i, k;

   k = 0;
   string[k] = '\0';

   for (i=0; i < range->nranges; i++) {
      if (i != 0)
	 string[k++] = ',';

      k += sprintf(&string[k], "%d-%d", range->ranges[i].low,
		   range->ranges[i].high);
   }

   return(string);

}
