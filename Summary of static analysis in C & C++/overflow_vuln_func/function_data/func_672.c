static gint
dissect_enttec_dmx_data(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	const char* chan_format[] = {
		"%2u ",
		"%02x ",
		"%3u "
	};
	const char* string_format[] = {
		"%03x: %s",
		"%3u: %s"
	};

	static guint8 dmx_data[512];
	static guint16 dmx_data_offset[513]; /
	emem_strbuf_t *dmx_epstr;

	proto_tree *hi,*si;
	proto_item *item;
	guint16 length,r,c,row_count;
	guint8 v,type,count;
	guint16 ci,ui,i,start_offset,end_offset;

	proto_tree_add_item(tree, hf_enttec_dmx_data_universe, tvb,
					offset, 1, FALSE);
	offset += 1;

	proto_tree_add_item(tree, hf_enttec_dmx_data_start_code, tvb,
					offset, 1, FALSE);
	offset += 1;

	type = tvb_get_guint8(tvb, offset);	
	proto_tree_add_item(tree, hf_enttec_dmx_data_type, tvb,
					offset, 1, FALSE);
	offset += 1;

	length = tvb_get_ntohs(tvb, offset);
	proto_tree_add_item(tree, hf_enttec_dmx_data_size, tvb,
					offset, 2, FALSE);
	offset += 2;

	if (length > 512)
		length = 512;

	if (type == ENTTEC_DATA_TYPE_RLE) {
		/
		ui = 0;
		ci = 0;
		while (ci < length) {
			v = tvb_get_guint8(tvb, offset+ci);
			if (v == 0xFE) {
				ci++;
				count = tvb_get_guint8(tvb, offset+ci);
				ci++;
				v = tvb_get_guint8(tvb, offset+ci);
				ci++;
				for (i=0;i < count;i++) {
					dmx_data[ui] = v;
					dmx_data_offset[ui] = ci-3;
					ui++;
				}
			} else if (v == 0xFD) {
				ci++;
				v = tvb_get_guint8(tvb, offset+ci);				
				dmx_data[ui] = v;
				dmx_data_offset[ui] = ci;
				ci++;
				ui++;
			} else {
				dmx_data[ui] = v;
				dmx_data_offset[ui] = ci;
				ui++;
				ci++;
			}
		}
		dmx_data_offset[ui] = ci;
	} else {
		for (ui=0; ui < length;ui++) {
			dmx_data[ui] =  tvb_get_guint8(tvb, offset+ui);
			dmx_data_offset[ui] = ui;
		}
		dmx_data_offset[ui] = ui;
	} 


	if (type == ENTTEC_DATA_TYPE_DMX || type == ENTTEC_DATA_TYPE_RLE) {
		hi = proto_tree_add_item(tree,
					hf_enttec_dmx_data_data,
					tvb,
					offset,
					length,
					FALSE);

		si = proto_item_add_subtree(hi, ett_enttec);
			
		row_count = (ui/global_disp_col_count) + ((ui%global_disp_col_count) == 0 ? 0 : 1);
		dmx_epstr = ep_strbuf_new_label(NULL);
		for (r=0; r < row_count;r++) {
			for (c=0;(c < global_disp_col_count) && (((r*global_disp_col_count)+c) < ui);c++) {
				if ((c % (global_disp_col_count/2)) == 0) {
					ep_strbuf_append_c(dmx_epstr, ' ');
				}
				v = dmx_data[(r*global_disp_col_count)+c];
				if (global_disp_chan_val_type == 0) {
					v = (v * 100) / 255;
					if (v == 100) {
						ep_strbuf_append(dmx_epstr, "FL ");
					} else {
						ep_strbuf_append_printf(dmx_epstr, chan_format[global_disp_chan_val_type], v);
					}
				} else {
					ep_strbuf_append_printf(dmx_epstr, chan_format[global_disp_chan_val_type], v);
				}
			}

			start_offset = dmx_data_offset[(r*global_disp_col_count)];
			end_offset = dmx_data_offset[(r*global_disp_col_count)+c];		

			proto_tree_add_none_format(si,hf_enttec_dmx_data_dmx_data, tvb,
						offset+start_offset, 
						end_offset-start_offset,
						string_format[global_disp_chan_nr_type], (r*global_disp_col_count)+1, dmx_epstr->str);
			ep_strbuf_truncate(dmx_epstr, 0);
		}
		
		item = proto_tree_add_item(si, hf_enttec_dmx_data_data_filter, tvb,
				offset, length, FALSE );
		PROTO_ITEM_SET_HIDDEN(item);
		
		offset += length;
	} else if (type == ENTTEC_DATA_TYPE_CHAN_VAL) {
		proto_tree_add_item(tree, hf_enttec_dmx_data_data_filter, tvb,
					offset, length, FALSE);
		offset += length;
	} else {
		proto_tree_add_item(tree, hf_enttec_dmx_data_data_filter, tvb,
					offset, length, FALSE);
		offset += length;
	}		

	
		
	return offset;
}
