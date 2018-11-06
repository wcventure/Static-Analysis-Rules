int
asn1_oid_value_decode ( ASN1_SCK *asn1, int enc_len, subid_t **oid, guint *len)
{
    int          ret;
    int          eoc;
    subid_t      subid;
    guint        size;
    subid_t      *optr;

    eoc = asn1->offset + enc_len;

    /
    if (eoc < asn1->offset || eoc < 0)
	eoc = INT_MAX;

    /
    if (enc_len != 0)
	tvb_get_guint8(asn1->tvb, eoc - 1);

    size = enc_len + 1;
    *oid = g_malloc(size * sizeof(gulong));
    optr = *oid;
 
    ret = asn1_subid_decode (asn1, &subid);
    if (ret != ASN1_ERR_NOERROR) {
	g_free(*oid);
	*oid = NULL;
	return ret;
    }
    if (subid < 40) {
	optr[0] = 0;
	optr[1] = subid;
    } else if (subid < 80) {
	optr[0] = 1;
	optr[1] = subid - 40;
    } else {
	optr[0] = 2;
	optr[1] = subid - 80;
    }
    *len = 2;
    optr += 2;
    while (asn1->offset < eoc) {
	if (++(*len) > size) {
            g_free(*oid);
            *oid = NULL;
	    return ASN1_ERR_WRONG_LENGTH_FOR_TYPE;
	}
	ret = asn1_subid_decode (asn1, optr++);
	if (ret != ASN1_ERR_NOERROR) {
            g_free(*oid);
            *oid = NULL;
	    return ret;
	}
    }
    return ASN1_ERR_NOERROR;
}
