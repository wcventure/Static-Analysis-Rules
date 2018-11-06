convert_ret_t range_convert_str(range_t **rangep, const gchar *es,
				guint32 max_value)
{
   range_t       *range;
   guint         nranges;
   const gchar   *p;
   char          *endp;
   gchar         c;
   guint         i;
   guint32       tmp;
   unsigned long val;

   /
   range = g_malloc(RANGE_HDR_SIZE + sizeof (range_admin_t));
   range->nranges = 0;
   nranges = 1;

   /

   p = es;
   for (;;) {
      /
      while ((c = *p) == ' ' || c == '\t')
	 p++;
      if (c == '\0')
	 break;

      /
      if (range->nranges >= nranges) {
	 /
	 if (nranges == 1)
	    nranges = 4;
	 else
	    nranges += 4;
	 range = g_realloc(range, RANGE_HDR_SIZE +
			   nranges*sizeof (range_admin_t));
      }

      if (c == '-') {
	 /
	 range->ranges[range->nranges].low = 1;
      } else if (isdigit((unsigned char)c)) {
	 /
	 val = strtol(p, &endp, 10);
	 if (p == endp) {
	    /
	    g_free(range);
	    return CVT_SYNTAX_ERROR;
	 }
	 if (val > G_MAXUINT32) {
	    /
	    g_free(range);
	    return CVT_NUMBER_TOO_BIG;
	 } 
	 p = endp;
	 range->ranges[range->nranges].low = val;

	 /
	 while ((c = *p) == ' ' || c == '\t')
	    p++;
      } else {
	 /
	 g_free(range);
	 return CVT_SYNTAX_ERROR;
      }

      if (c == '-') {
	 /
	 p++;

	 /
	 while ((c = *p) == ' ' || c == '\t')
	    p++;

	 if (c == ',' || c == '\0') {
	   /
	   range->ranges[range->nranges].high = max_value;
	 } else if (isdigit((unsigned char)c)) {
	    /
	   val = strtoul(p, &endp, 10);
	   if (p == endp) {
	      /
	      g_free(range);
	      return CVT_SYNTAX_ERROR;
	   }
	   if (val > G_MAXUINT32) {
	      /
	      g_free(range);
	      return CVT_NUMBER_TOO_BIG;
	   } 
	   p = endp;
	   range->ranges[range->nranges].high = val;

	   /
	   while ((c = *p) == ' ' || c == '\t')
	      p++;
	 } else {
	    /
	    g_free(range);
	    return CVT_SYNTAX_ERROR;
	 }
      } else if (c == ',' || c == '\0') {
	 /
	 range->ranges[range->nranges].high = range->ranges[range->nranges].low;
      } else {
	 / 
	 g_free(range);
	 return CVT_SYNTAX_ERROR;
      }
      range->nranges++;

      if (c == ',') {
	 /
	 p++;
      }
   }

   /
   for (i=0; i < range->nranges; i++) {
      if (range->ranges[i].low > range->ranges[i].high) {
	 tmp = range->ranges[i].low;
	 range->ranges[i].low  = range->ranges[i].high;
	 range->ranges[i].high = tmp;
      }
   }

   /
   *rangep = range;
   return CVT_NO_ERROR;
} /
