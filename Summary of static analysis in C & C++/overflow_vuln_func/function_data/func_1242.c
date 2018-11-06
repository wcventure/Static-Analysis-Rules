int dissect_ber_object_identifier(gboolean implicit_tag, packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset, gint hf_id, char *value_string) {
	gint8 class;
	gboolean pc;
	gint32 tag;
	guint32 i, len;
	int eoffset;
	guint8 byte;
	guint32 value;
	char str[256],*strp, *name;
	proto_item *item;

#ifdef DEBUG_BER
{
char *name;
header_field_info *hfinfo;
if(hf_id>0){
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

	if (value_string) {
		value_string[0] = '\0';
	}

	if (!implicit_tag) {
		/
		offset = get_ber_identifier(tvb, offset, &class, &pc, &tag);
		offset = dissect_ber_length(pinfo, tree, tvb, offset, &len, NULL);
		eoffset = offset + len;
		if( (class!=BER_CLASS_UNI)
		  ||(tag != BER_UNI_TAG_OID) ){
	            tvb_ensure_bytes_exist(tvb, offset-2, 2);
	    	    proto_tree_add_text(tree, tvb, offset-2, 2, "BER Error: Object Identifier expected but Class:%d PC:%d Tag:%d was unexpected", class, pc, tag);
			return eoffset;
		}
	} else {
		len=tvb_length_remaining(tvb,offset);
		eoffset=offset+len;
	}

	value=0;
	for (i=0,strp=str; i<len; i++){
		byte = tvb_get_guint8(tvb, offset);
		offset++;

		if((strp-str)>200){
    	    proto_tree_add_text(tree, tvb, offset, eoffset - offset, "BER Error: too long Object Identifier");
			return offset;
		}

		/
		if (i == 0) {
			strp += sprintf(strp, "%d.%d", byte/40, byte%40);
			continue;
		}

		value = (value << 7) | (byte & 0x7F);
		if (byte & 0x80) {
			continue;
		}

		strp += sprintf(strp, ".%d", value);
		value = 0;
	}
	*strp = '\0';

	if (hf_id != -1) {
		item=proto_tree_add_string(tree, hf_id, tvb, offset - len, len, str);
		/
		if(item){
			name=g_hash_table_lookup(oid_table, str);
			if(name){
				proto_item_append_text(item, " (%s)", name);
			}
		}
	}

	if (value_string) {
		strcpy(value_string, str);
	}

	return eoffset;
}
