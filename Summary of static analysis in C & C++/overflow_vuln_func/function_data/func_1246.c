static field_info *
alloc_field_info(proto_tree *tree, int hfindex, tvbuff_t *tvb, gint start, gint length)
{
	header_field_info	*hfinfo;
	field_info		*fi;

	/
	g_assert(tvb != NULL || length == 0);

	g_assert(hfindex >= 0 && (guint) hfindex < gpa_hfinfo->len);
	hfinfo = proto_registrar_get_nth(hfindex);
	g_assert(hfinfo != NULL);

	if (length == -1) {
		/
		g_assert(hfinfo->type == FT_PROTOCOL ||
			 hfinfo->type == FT_NONE);
		length = tvb_length_remaining(tvb, start);
	}

	fi = g_mem_chunk_alloc(gmc_field_info);
	fi->hfinfo = hfinfo;
	fi->start = start;
	if (tvb) {
		fi->start += tvb_raw_offset(tvb);
	}
	fi->length = length;
	fi->tree_type = ETT_NONE;
	fi->visible = PTREE_DATA(tree)->visible;
	fi->representation = NULL;

	fi->value = fvalue_new(fi->hfinfo->type);

	/
	if (tvb) {
		fi->ds_name = tvb_get_name(tvb);
	} else {
		fi->ds_name = NULL;
	}

	return fi;
}
