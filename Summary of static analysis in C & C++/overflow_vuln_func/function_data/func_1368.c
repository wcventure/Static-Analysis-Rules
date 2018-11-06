static guint32 field8(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf)
{
      guint32 v = VALUE8(tvb, *offsetp);
      header_field_info *hfi = proto_registrar_get_nth(hf);
      gchar *enumValue = NULL;
      gchar *nameAsChar = hfi -> name;

      if (hfi -> strings)
	    enumValue = match_strval(v, cVALS(hfi -> strings));
      if (enumValue)
	    proto_tree_add_uint_format(t, hf, tvb, *offsetp, 1, v, "%s: %u (%s)", nameAsChar, v, enumValue);
      else
	    proto_tree_add_item(t, hf, tvb, *offsetp, 1, little_endian);
      *offsetp += 1;
      return v;
}
