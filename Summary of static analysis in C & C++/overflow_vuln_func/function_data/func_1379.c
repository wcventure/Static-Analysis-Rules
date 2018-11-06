static int
epl_wmem_iarray_cmp(const void *a, const void *b)
{
	return *(const guint32*)a - *(const guint32*)b;
}
