static void get_opt60_tlv(tvbuff_t *tvb, guint off, guint8 *tlvtype, guint8 *tlvlen, guint8 **value)
{
	guint  i;
	guint8  *val_asc;
	val_asc = (guint8 *)ep_alloc0(4);
	/
	tvb_memcpy(tvb, val_asc, off, 2);
	*tlvtype = (guint8)strtoul((gchar*)val_asc, NULL, 16);
	/
	tvb_memcpy(tvb, val_asc, off + 2, 2);
	*tlvlen = (guint8)strtoul((gchar*)val_asc, NULL, 16);
	/
	*value = (guint8 *)ep_alloc0(*tlvlen);
	for (i=0; i<*tlvlen; i++)
	{
		memset(val_asc, 0, sizeof (val_asc));
		tvb_memcpy(tvb, val_asc, off + ((i*2) + 4), 2);
		(*value)[i] = (guint8)strtoul((gchar*)val_asc, NULL, 16);
	}
}
