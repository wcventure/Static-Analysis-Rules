static guint32 field32(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf)
{
      guint32 v = VALUE32(tvb, *offsetp);
      header_field_info *hfi = proto_registrar_get_nth(hf);
      gchar *enumValue = NULL;
      gchar *nameAsChar = hfi -> name;

      if (hfi -> strings)
	    enumValue = match_strval(v, cVALS(hfi -> strings));
      if (enumValue)
	    proto_tree_add_uint_format(t, hf, tvb, *offsetp, 4, v, "%s: 0x%08x (%s)", nameAsChar, v, enumValue);
      else
	    proto_tree_add_uint_format(t, hf, tvb, *offsetp, 4, v, 
				       hfi -> display == BASE_DEC ? "%s: %u" : "%s: 0x%08x",
				       nameAsChar, v);
      *offsetp += 4;
      return v;
}
