static int
dissect_pvfs_opaque_data(tvbuff_t *tvb, int offset,
	proto_tree *tree,
	packet_info *pinfo _U_,
	int hfindex,
	gboolean fixed_length, guint32 length,
	gboolean string_data, char const **string_buffer_ret)
{
	int data_offset;
	proto_item *string_item = NULL;
	proto_tree *string_tree = NULL;

	guint32 string_length;
	guint32 string_length_full;
	guint32 string_length_packet;
	guint32 string_length_captured;
	guint32 string_length_copy;

	int fill_truncated;
	guint32 fill_length;
	guint32 fill_length_packet;
	guint32 fill_length_captured;
	guint32 fill_length_copy;

	int exception = 0;

	char *string_buffer = NULL;
	const char *string_buffer_print = NULL;

	if (fixed_length) {
		string_length = length;
		data_offset = offset;
	} else {
		string_length = tvb_get_letohl(tvb,offset+0);
		data_offset = offset + 4;

		/

		if (string_data)
			string_length += 1;
	}

	string_length_captured = tvb_length_remaining(tvb, data_offset);
	string_length_packet = tvb_reported_length_remaining(tvb, data_offset);

	/

	if (!string_data)
		string_length_full = roundup4(string_length);
	else
		string_length_full = roundup8(4 + string_length);

	if (string_length_captured < string_length) {
		/
		string_length_copy = string_length_captured;
		fill_truncated = 2;
		fill_length = 0;
		fill_length_copy = 0;

		if (string_length_packet < string_length)
			exception = ReportedBoundsError;
		else
			exception = BoundsError;
	}
	else {
		/
		string_length_copy = string_length;

		if (!string_data)
			fill_length = string_length_full - string_length;
		else
			fill_length = string_length_full - string_length - 4;

		fill_length_captured = tvb_length_remaining(tvb,
		    data_offset + string_length);
		fill_length_packet = tvb_reported_length_remaining(tvb,
		    data_offset + string_length);

		if (fill_length_captured < fill_length) {
			/
			fill_length_copy = fill_length_packet;
			fill_truncated = 1;
			if (fill_length_packet < fill_length)
				exception = ReportedBoundsError;
			else
				exception = BoundsError;
		}
		else {
			/
			fill_length_copy = fill_length;
			fill_truncated = 0;
		}
	}

	if (string_data) {
		char *tmpstr;

		tmpstr = (char *) tvb_get_string(wmem_packet_scope(), tvb, data_offset,
				string_length_copy);

		string_buffer = (char *)memcpy(wmem_alloc(wmem_packet_scope(), string_length_copy+1), tmpstr, string_length_copy);
	} else {
		string_buffer = (char *) tvb_memcpy(tvb,
				wmem_alloc(wmem_packet_scope(), string_length_copy+1), data_offset, string_length_copy);
	}

	string_buffer[string_length_copy] = '\0';

	/
	if (string_length) {
		if (string_length != string_length_copy) {
			if (string_data) {
				char *formatted;
				guint16 string_buffer_size = 0;

				formatted = format_text((guint8 *)string_buffer,
						(int)strlen(string_buffer));

				string_buffer_size = (guint16)strlen(formatted) + 12 + 1;

				/
				string_buffer_print = (char*) wmem_alloc(wmem_packet_scope(), string_buffer_size);
				/
				g_snprintf((char *)string_buffer_print, string_buffer_size,
						"%s<TRUNCATED>", formatted);
				/
				/
			} else {
				string_buffer_print="<DATA><TRUNCATED>";
			}
		} else {
			if (string_data) {
				string_buffer_print =
				    wmem_strdup(wmem_packet_scope(), format_text((guint8 *) string_buffer,
								 (int)strlen(string_buffer)));
			} else {
				string_buffer_print="<DATA>";
			}
		}
	} else {
		string_buffer_print="<EMPTY>";
	}

	if (tree) {
		string_item = proto_tree_add_text(tree, tvb,offset+0, -1,
		    "%s: %s", proto_registrar_get_name(hfindex),
		    string_buffer_print);

		if (string_item)
			string_tree = proto_item_add_subtree(string_item,
		    	ett_pvfs_string);
	}
	if (!fixed_length) {
		if (string_tree)
			proto_tree_add_text(string_tree, tvb,offset+0,4,
				"length: %u (excl. NULL terminator)", string_length - 1);
		offset += 4;
	}

	if (string_tree) {
		if (string_data) {
			proto_tree_add_string_format(string_tree,
			    hfindex, tvb, offset, string_length_copy,
			    string_buffer,
			    "contents: %s", string_buffer_print);
		} else {
			proto_tree_add_bytes_format(string_tree,
			    hfindex, tvb, offset, string_length_copy,
			    (guint8 *) string_buffer,
			    "contents: %s", string_buffer_print);
		}
	}

	offset += string_length_copy;

	if (fill_length) {
		if (string_tree) {
			if (fill_truncated) {
				proto_tree_add_text(string_tree, tvb,
				offset,fill_length_copy,
				"fill bytes: opaque data<TRUNCATED>");
			}
			else {
				proto_tree_add_text(string_tree, tvb,
				offset,fill_length_copy,
				"fill bytes: opaque data");
			}
		}
		offset += fill_length_copy;
	}

	if (string_item)
		proto_item_set_end(string_item, tvb, offset);

	if (string_buffer_ret != NULL)
		*string_buffer_ret = string_buffer_print;

	/
	if (exception != 0)
		THROW(exception);

	return offset;
}
