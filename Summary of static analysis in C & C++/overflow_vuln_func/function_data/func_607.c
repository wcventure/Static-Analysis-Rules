static void
dis_msg_status_report(tvbuff_t *tvb, proto_tree *tree, guint32 offset)
{
    guint32	saved_offset;
    guint32	length;
    guint8	oct;
    guint8	pi;
    guint8	udl;
    gboolean	seven_bit;
    gboolean	eight_bit;
    gboolean	ucs2;
    gboolean	compressed;
    gboolean	udhi;


    udl = 0;
    saved_offset = offset;
    length = tvb_length_remaining(tvb, offset);

    oct = tvb_get_guint8(tvb, offset);

    DIS_FIELD_UDHI(tree, 0x40, offset, udhi);

    DIS_FIELD_SRQ(tree, 0x20, offset);

    DIS_FIELD_MMS(tree, 0x04, offset);

    DIS_FIELD_MTI(tree, 0x03, offset);

    offset++;
    oct = tvb_get_guint8(tvb, offset);

    DIS_FIELD_MR(tree, offset);

    offset++;

    dis_field_addr(tvb, tree, &offset, "TP-Recipient-Address");

    dis_field_scts(tvb, tree, &offset);

    dis_field_dt(tvb, tree, &offset);

    oct = tvb_get_guint8(tvb, offset);

    dis_field_st(tvb, tree, offset, oct);

    offset++;
	/
	if (length <= (offset - saved_offset))
	{
	    return;
	}
    pi = tvb_get_guint8(tvb, offset);

    dis_field_pi(tvb, tree, offset, pi);

    if (pi & 0x01)
    {
	if (length <= (offset - saved_offset))
	{
	    proto_tree_add_text(tree,
		tvb, offset, -1,
		"Short Data (?)");
	    return;
	}

	offset++;
	oct = tvb_get_guint8(tvb, offset);

	dis_field_pid(tvb, tree, offset, oct);
    }

    if (pi & 0x02)
    {
	if (length <= (offset - saved_offset))
	{
	    proto_tree_add_text(tree,
		tvb, offset, -1,
		"Short Data (?)");
	    return;
	}

	offset++;
	oct = tvb_get_guint8(tvb, offset);

	dis_field_dcs(tvb, tree, offset, oct, &seven_bit, &eight_bit, &ucs2, &compressed);
    }

    if (pi & 0x04)
    {
	if (length <= (offset - saved_offset))
	{
	    proto_tree_add_text(tree,
		tvb, offset, -1,
		"Short Data (?)");
	    return;
	}

	offset++;
	oct = tvb_get_guint8(tvb, offset);
	udl = oct;

	DIS_FIELD_UDL(tree, offset);
    }

    if (udl > 0)
    {
	offset++;

	dis_field_ud(tvb, tree, offset, length - (offset - saved_offset), udhi, udl,
	    seven_bit, eight_bit, ucs2, compressed);
    }
}
