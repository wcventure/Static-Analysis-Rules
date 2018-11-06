int
asn1_string_value_decode ( ASN1_SCK *asn1, int enc_len, guchar **octets)
{
    int          ret;
    int          eoc;
    guchar       *ptr;

    eoc = asn1->offset + enc_len;

    /
    if (eoc < asn1->offset || eoc < 0)
	eoc = INT_MAX;

    /
    if (enc_len != 0) {
	tvb_get_guint8(asn1->tvb, eoc - 1);
	*octets = g_malloc (enc_len);
    } else {
	/
	*octets = g_malloc (1);
    }
    ptr = *octets;
    while (asn1->offset < eoc) {
	ret = asn1_octet_decode (asn1, (guchar *)ptr++);
	if (ret != ASN1_ERR_NOERROR) {
	    g_free(*octets);
	    *octets = NULL;
	    return ret;
	}
    }
    return ASN1_ERR_NOERROR;
}
