int dissect_ber_object_identifier(gboolean implicit_tag, asn1_ctx_t *actx, proto_tree *tree, tvbuff_t *tvb, int offset, gint hf_id, tvbuff_t **value_tvb)
{
	gint8 class;
	gboolean pc;
	gint32 tag;
	guint32 len;
	int eoffset;
	int hoffset;
	char *str;
	proto_item *cause;
	header_field_info *hfi;
	const gchar *name;

#ifdef DEBUG_BER
{
const char *name;
header_field_info *hfinfo;
if(hf_id>=0){
hfinfo = proto_registrar_get_nth(hf_id);
name=hfinfo->name;
} else {
name="unnamed";
}
if(tvb_length_remaining(tvb,offset)>3){
printf("OBJECT IDENTIFIER dissect_ber_object_identifier(%s) entered implicit_tag:%d offset:%d len:%d %02x:%02x:%02x\n",name,implicit_tag,offset,tvb_length_remaining(tvb,offset),tvb_get_guint8(tvb,offset),tvb_get_guint8(tvb,offset+1),tvb_get_guint8(tvb,offset+2));
}else{
printf("OBJECT IDENTIFIER dissect_ber_object_identifier(%s) entered\n",name);
}
}
#endif

	if(!implicit_tag) {
		hoffset = offset;
		/
		offset = dissect_ber_identifier(actx->pinfo, tree, tvb, offset, &class, &pc, &tag);
		offset = dissect_ber_length(actx->pinfo, tree, tvb, offset, &len, NULL);
		eoffset = offset + len;
		if( (class!=BER_CLASS_UNI)
		  ||(tag != BER_UNI_TAG_OID) ){
	            tvb_ensure_bytes_exist(tvb, hoffset, 2);
		    cause = proto_tree_add_text(tree, tvb, offset, len, "BER Error: Object Identifier expected but Class:%d PC:%d Tag:%d was unexpected", class, pc, tag);
		    proto_item_set_expert_flags(cause, PI_MALFORMED, PI_WARN);
		    expert_add_info_format(actx->pinfo, cause, PI_MALFORMED, PI_WARN, "BER Error: Object Identifier expected");
		    if (decode_unexpected) {
		      proto_tree *unknown_tree = proto_item_add_subtree(cause, ett_ber_unknown);
		      dissect_unknown_ber(actx->pinfo, tvb, hoffset, unknown_tree);
		    }
		    return eoffset;
		}
	} else {
		len=tvb_length_remaining(tvb,offset);
		eoffset=offset+len;
	}

	actx->created_item=ber_last_created_item=NULL;
	hfi = proto_registrar_get_nth(hf_id);
	if (hfi->type == FT_OID) {
		actx->created_item = proto_tree_add_item(tree, hf_id, tvb, offset, len, FALSE);
	} else if (IS_FT_STRING(hfi->type)) {
		str = oid_to_str(tvb_get_ptr(tvb, offset, len), len);
		actx->created_item = proto_tree_add_string(tree, hf_id, tvb, offset, len, str);
		if(actx->created_item){
			/
			name = get_oid_name(tvb_get_ptr(tvb, offset, len), len);
			if(name){
				proto_item_append_text(actx->created_item, " (%s)", name);
			}
		}
	} else {
		DISSECTOR_ASSERT_NOT_REACHED();
	}
	ber_last_created_item=actx->created_item;
	if (value_tvb)
		*value_tvb = tvb_new_subset(tvb, offset, len, len);

	return eoffset;
}
