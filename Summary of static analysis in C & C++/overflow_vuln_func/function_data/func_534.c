static int
dissect_tcap_dlg_user_info(ASN1_SCK *asn1, proto_tree *tree, packet_info *pinfo)
{
    guint tag, len;
    guint saved_offset = 0;
    gboolean def_len;
    gboolean user_info_def_len;
	int ret;
	proto_item *para_item;
	proto_tree *sub_tree;
	char oid_str[64]; /
	tvbuff_t *next_tvb;

#define TC_USR_INFO_TAG 0xbe
    if (tcap_check_tag(asn1, TC_USR_INFO_TAG))
    {
	tag = -1;
	dissect_tcap_tag(asn1, tree, &tag, "User Info Tag");
	dissect_tcap_len(asn1, tree, &user_info_def_len, &len);

#define TC_EXT_TAG 0x28
	if (tcap_check_tag(asn1, TC_EXT_TAG))
	{
	    saved_offset = asn1->offset;
	    asn1_id_decode1(asn1, &tag);
	    proto_tree_add_uint_format(tree, hf_tcap_length, asn1->tvb, saved_offset, asn1->offset - saved_offset,
		tag, "External Tag: 0x%x", tag);

	    dissect_tcap_len(asn1, tree, &def_len, &len);
	}
	para_item = proto_tree_add_text(tree, asn1->tvb, asn1->offset, len, "Parameter Data");
	sub_tree = proto_item_add_subtree(para_item, ett_para_portion);
	asn1->offset = dissect_ber_object_identifier(FALSE, pinfo, sub_tree, asn1->tvb, asn1->offset, hf_tcap_oid, oid_str);

    saved_offset = asn1->offset;
    ret = asn1_id_decode1(asn1, &tag);

    proto_tree_add_uint_format(sub_tree, hf_tcap_tag, asn1->tvb,
	saved_offset, asn1->offset - saved_offset, tag,
	"Single-ASN.1-type Tag");
	dissect_tcap_len(asn1, sub_tree, &def_len, &len);

	next_tvb = tvb_new_subset(asn1->tvb, asn1->offset, len, len);

	call_ber_oid_callback(oid_str, next_tvb,0, pinfo, sub_tree);


	asn1->offset += len;

	if (!user_info_def_len)
	{
	    /
	    dissect_tcap_eoc(asn1, tree);
	}
    }

    return TC_DS_OK;
}
