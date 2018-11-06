static int
oid_repr_len(fvalue_t *fv _U_, ftrepr_t rtype _U_)
{
	/
	return fv->value.bytes->len * 3 + 16;
}
