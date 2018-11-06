static void *
composite_memcpy(tvbuff_t *tvb, void* _target, guint abs_offset, size_t abs_length)
{
	struct tvb_composite *composite_tvb = (struct tvb_composite *) tvb;
	guint8 *target = (guint8 *) _target;

	guint	    i, num_members;
	tvb_comp_t *composite;
	tvbuff_t   *member_tvb = NULL;
	guint	    member_offset, member_length;
	gboolean    retval;
	GSList	   *slist;

	DISSECTOR_ASSERT(tvb->ops == get_tvb_composite_ops());

	/
	composite   = &composite_tvb->composite;
	num_members = g_slist_length(composite->tvbs);

	for (i = 0; i < num_members; i++) {
		if (abs_offset <= composite->end_offsets[i]) {
			slist = g_slist_nth(composite->tvbs, i);
			member_tvb = (tvbuff_t *)slist->data;
			break;
		}
	}
	DISSECTOR_ASSERT(member_tvb);

	if (check_offset_length_no_exception(member_tvb, abs_offset - composite->start_offsets[i],
				(gint) abs_length, &member_offset, &member_length, NULL)) {

		DISSECTOR_ASSERT(!tvb->real_data);
		return tvb_memcpy(member_tvb, target, member_offset, member_length);
	}
	else {
		/
		retval = compute_offset_length(member_tvb, abs_offset - composite->start_offsets[i], -1,
				&member_offset, &member_length, NULL);
		DISSECTOR_ASSERT(retval);

		/
		DISSECTOR_ASSERT(member_length);

		tvb_memcpy(member_tvb, target, member_offset, member_length);
		abs_offset	+= member_length;
		abs_length	-= member_length;

		/
		if (abs_length > 0) {
			composite_memcpy(tvb, target + member_length, abs_offset, abs_length);
		}

		return target;
	}

	DISSECTOR_ASSERT_NOT_REACHED();
}
