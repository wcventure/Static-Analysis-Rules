	*bl_offset = offset;
	return TRUE;
}
 
int
get_ber_length(tvbuff_t *tvb, int offset, guint32 *length, gboolean *ind) 
{
	int bl_offset = offset;
	guint32 bl_length;


	gint8 save_class;
	gboolean save_pc;
	gint32 save_tag;	

	/
	save_class = last_class;
	save_pc = last_pc;
	save_tag = last_tag;

	try_get_ber_length(tvb, &bl_offset, &bl_length, ind);

	/
	last_class = save_class;
	last_pc = save_pc;
	last_tag = save_tag;

	if (length)
		*length = bl_length;

	return bl_offset;
}
