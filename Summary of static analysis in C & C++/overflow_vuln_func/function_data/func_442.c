static void
print_field_hex_value(print_data *pdata, field_info *fi)
{
	int i;
	const guint8 *pd;

	/
	pd = get_field_data(pdata->src_list, fi);

	/
	for (i = 0 ; i < fi->length; i++) {
		fprintf(pdata->fh, "%02x", pd[i]);
	}
}
