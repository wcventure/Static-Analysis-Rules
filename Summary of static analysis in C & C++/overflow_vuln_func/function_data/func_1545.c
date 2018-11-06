void dissector_delete_uint_range(const char *abbrev, range_t *range,
				 dissector_handle_t handle)
{
	guint32 i, j;

	if (range) {
		for (i = 0; i < range->nranges; i++) {
			for (j = range->ranges[i].low; j <= range->ranges[i].high; j++)
				dissector_delete_uint(abbrev, j, handle);
		}
	}
}
