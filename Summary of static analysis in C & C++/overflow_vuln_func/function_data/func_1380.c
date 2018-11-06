static int
find_in_range(const void *_a, const void *_b)
{
	const range_admin_t *a = (const range_admin_t*)_a,
	                    *b = (const range_admin_t*)_b;

	if (a->low <= b->high && b->low <= a->high) /
		return 0;

	return a->low - b->low;
}
