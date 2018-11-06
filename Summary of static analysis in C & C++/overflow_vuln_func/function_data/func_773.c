guint32
dissect_per_constrained_integer(tvbuff_t *tvb, guint32 offset, asn_ctx_t *actx, proto_tree *tree, int hf_index, guint32 min, guint32 max, guint32 *value, gboolean has_extension)
{
	proto_item *it=NULL;
	guint32 range, val;
	gint val_start, val_length;
	nstime_t timeval;
	header_field_info *hfi;
	int num_bits;
	int pad;
	gboolean tmp;

DEBUG_ENTRY("dissect_per_constrained_integer");
	if(has_extension){
		gboolean extension_present;
		offset=dissect_per_boolean(tvb, offset, actx, tree, hf_per_extension_present_bit, &extension_present);
		if (!display_internal_per_fields) PROTO_ITEM_SET_HIDDEN(actx->created_item);
		if(extension_present){
			offset = dissect_per_integer(tvb, offset, actx, tree, hf_index, (gint32*)value);
			return offset;
		}
	}

	hfi = proto_registrar_get_nth(hf_index);

	/
	if(((max-min)>65536)&&(actx->aligned)){
		/
		range=1000000;
	} else {
		range=max-min+1;
	}

	num_bits=0;
	pad=0;
	val=0;
	timeval.secs=val; timeval.nsecs=0;
	/
	if(range==1){
		val_start = offset>>3; val_length = 0;
		val = min; 
	} else if((range<=255)||(!actx->aligned)) {
		/
		char *str;
		int i, bit, length;
		guint32 mask,mask2;
		/
		mask  = 0x80000000;
		mask2 = 0x7fffffff;
		i = 32;
		while ((range & mask)== 0){
			i = i - 1;
			mask = mask>>1;
			mask2 = mask2>>1;
		}
		if ((range & mask2) == 0)
			i = i-1;

		num_bits = i;
		length=1;
		if(range<=2){
			num_bits=1;
		}

		/
		str=ep_alloc(256);
		g_snprintf(str, 256, "%s: ", hfi->name);
		for(bit=0;bit<((int)(offset&0x07));bit++){
			if(bit&&(!(bit%4))){
				strcat(str, " ");
			}
			strcat(str,".");
		}
		/
		for(i=0;i<num_bits;i++){
			if(bit&&(!(bit%4))){
				strcat(str, " ");
			}
			if(bit&&(!(bit%8))){
				length+=1;
				strcat(str, " ");
			}
			bit++;
			offset=dissect_per_boolean(tvb, offset, actx, tree, -1, &tmp);
			val<<=1;
			if(tmp){
				val|=1;
				strcat(str, "1");
			} else {
				strcat(str, "0");
			}
		}
		for(;bit%8;bit++){
			if(bit&&(!(bit%4))){
				strcat(str, " ");
			}
			strcat(str,".");
		}
		val_start = (offset-num_bits)>>3; val_length = length;
		val+=min;
		if ((display_internal_per_fields)&&(str))
			proto_tree_add_text(tree, tvb, val_start,val_length,"Range = %u Bitfiled lengt %u, %s",range, num_bits, str);
	} else if(range==256){
		/
		num_bits=8;
		pad=7-(offset&0x07);

		/
		BYTE_ALIGN_OFFSET(offset);
		val=tvb_get_guint8(tvb, offset>>3);
		offset+=8;

		val_start = (offset>>3)-1; val_length = 1;
		val+=min;
	} else if(range<=65536){
		/
		num_bits=16;
		pad=7-(offset&0x07);

		/
		BYTE_ALIGN_OFFSET(offset);
		val=tvb_get_guint8(tvb, offset>>3);
		val<<=8;
		offset+=8;
		val|=tvb_get_guint8(tvb, offset>>3);
		offset+=8;

		val_start = (offset>>3)-2; val_length = 2;
		val+=min;
	} else {
		int i,num_bytes;
		gboolean bit;

		/
		/
		offset=dissect_per_boolean(tvb, offset, actx, tree, -1, &bit);
		num_bytes=bit;
		offset=dissect_per_boolean(tvb, offset, actx, tree, -1, &bit);
		num_bytes=(num_bytes<<1)|bit;

		num_bytes++;  /
		if (display_internal_per_fields)
			proto_tree_add_uint(tree, hf_per_const_int_len, tvb, (offset>>3), 1, num_bytes);

		/
		BYTE_ALIGN_OFFSET(offset);
		val=0;
		for(i=0;i<num_bytes;i++){
			val=(val<<8)|tvb_get_guint8(tvb,offset>>3);
			offset+=8;
		}
		val_start = (offset>>3)-(num_bytes+1); val_length = num_bytes+1;
		val+=min;
	}

	timeval.secs = val;
	if (IS_FT_UINT(hfi->type)) {
		it = proto_tree_add_uint(tree, hf_index, tvb, val_start, val_length, val);
	} else if (IS_FT_INT(hfi->type)) {
		it = proto_tree_add_int(tree, hf_index, tvb, val_start, val_length, val);
	} else if (IS_FT_TIME(hfi->type)) {
		it = proto_tree_add_time(tree, hf_index, tvb, val_start, val_length, &timeval);
	} else {
		THROW(ReportedBoundsError);
	}
	actx->created_item = it;
	if (value) *value = val;
	return offset;}
