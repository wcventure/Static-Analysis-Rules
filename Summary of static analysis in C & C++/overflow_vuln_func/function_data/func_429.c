static void
oid_to_repr(fvalue_t *fv, ftrepr_t rtype _U_, char *buf)
{
	oid_to_str_buf(fv->value.bytes->data, fv->value.bytes->len, buf, oid_repr_len(fv, rtype));
}
