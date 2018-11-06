static const guint8*
composite_get_ptr(tvbuff_t *tvb, guint abs_offset, guint abs_length)
{
	struct tvb_composite *composite_tvb = (struct tvb_composite *) tvb;
	guint	    i, num_members;
	tvb_comp_t *composite;
	tvbuff_t   *member_tvb = NULL;
	guint	    member_offset, member_length;
	GSList	   *slist;

	DISSECTOR_ASSERT(tvb->ops == get_tvb_composite_ops());

	/
	composite = &composite_tvb->composite;
	num_members = g_slist_length(composite->tvbs);

	for (i = 0; i < num_members; i++) {
		if (abs_offset <= composite->end_offsets[i]) {
			slist = g_slist_nth(composite->tvbs, i);
			member_tvb = (tvbuff_t *)slist->data;
			break;
		}
	}
	DISSECTOR_ASSERT(member_tvb);

	if (check_offset_length_no_exception(member_tvb,
					     abs_offset - composite->start_offsets[i],
					     abs_length, &member_offset, &member_length, NULL)) {

		/
		DISSECTOR_ASSERT(!tvb->real_data);
		return ensure_contiguous_no_exception(member_tvb, member_offset, member_length, NULL);
	}
	else {
		tvb->real_data = (guint8 *)tvb_memdup(tvb, 0, -1);
		return tvb->real_data + abs_offset;
	}

	DISSECTOR_ASSERT_NOT_REACHED();
}
