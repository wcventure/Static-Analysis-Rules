static int
dissect_tcap_dlg_application_context_name(ASN1_SCK *asn1, proto_tree *tcap_tree,packet_info *pinfo)
{
    guint saved_offset = 0;
    guint name_len, len, len2;
    guint tag;
    subid_t *oid;
	char oid_str[64]; /
    int ret;
    gboolean def_len;

    saved_offset = asn1->offset;
    ret = asn1_id_decode1(asn1, &tag);
    proto_tree_add_uint_format(tcap_tree, hf_tcap_tag, asn1->tvb, saved_offset, asn1->offset - saved_offset, tag,
			    "Application Context Name Tag: 0x%x", tag);

    dissect_tcap_len(asn1, tcap_tree, &def_len, &name_len);

    saved_offset = asn1->offset;
    ret = asn1_oid_decode (asn1, &oid, &len, &len2);
	/
	asn1->offset = dissect_ber_object_identifier(FALSE, pinfo, tcap_tree, asn1->tvb, saved_offset, hf_tcap_oid, oid_str);
    if (ret == ASN1_ERR_NOERROR) g_free(oid);

    if (!def_len)
    {
	/
	dissect_tcap_eoc(asn1, tcap_tree);
    }
	pinfo->private_data = g_strdup(oid_str);

    return TC_DS_OK;
}
