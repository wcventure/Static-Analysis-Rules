static void
slice_func(gpointer data, gpointer user_data)
{
	drange_node	*drnode = data;
	slice_data_t	*slice_data = user_data;
	gint		start_offset;
	gint		length = 0;
	gint		end_offset = 0;
	guint		field_length;
	fvalue_t	*fv;
	drange_node_end_t	ending;

	if (slice_data->slice_failure) {
		return;
	}

	start_offset = drange_node_get_start_offset(drnode);
	ending = drange_node_get_ending(drnode);

	fv = slice_data->fv;
	field_length = fvalue_length(fv);

	/
	if (start_offset < 0) {
		start_offset = field_length + start_offset;
	}

	/
	if (ending == TO_THE_END) {
		end_offset = field_length;
		length = end_offset - start_offset;
	}
	else if (ending == LENGTH) {
		length = drange_node_get_length(drnode);
		if (length < 0) {
			end_offset = field_length + length;
			if (end_offset > start_offset) {
				length = end_offset - start_offset + 1;
			}
			else {
				slice_data->slice_failure = TRUE;
				return;
			}
		}
		else {
			end_offset = start_offset + length;
		}
	}
	else if (ending == OFFSET) {
		end_offset = drange_node_get_end_offset(drnode);
		if (end_offset < 0) {
			end_offset = field_length + end_offset;
			if (end_offset > start_offset) {
				length = end_offset - start_offset + 1;
			}
			else {
				slice_data->slice_failure = TRUE;
				return;
			}
		}
		else {
			length = end_offset - start_offset + 1;
		}
	}
	else {
		g_assert_not_reached();
	}

/

	if (start_offset > (int) field_length || end_offset > (int) field_length) {
		slice_data->slice_failure = TRUE;
		return;
	}

	fv->ftype->slice(fv, slice_data->bytes, start_offset, length);
}
