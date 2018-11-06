static void
dis_field_ud(tvbuff_t *tvb, proto_tree *tree, guint32 offset, guint32 length, gboolean udhi, guint8 udl,
    gboolean seven_bit, gboolean eight_bit, gboolean ucs2, gboolean compressed)
{
    static guint8	fill_bits_mask[] = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc };
    proto_item	*item;
    proto_item	*udh_item;
    proto_tree	*subtree = NULL;
    proto_tree	*udh_subtree = NULL;
    guint8	oct;
    guint8	fill_bits;
    guint32	out_len;
    char	*ustr;

    fill_bits = 0;

    item =
	proto_tree_add_text(tree, tvb,
	    offset, length,
	    "TP-User-Data");

    subtree = proto_item_add_subtree(item, ett_ud);

    oct = tvb_get_guint8(tvb, offset);

    if (udhi)
    {

		/

		udh_item =
		    proto_tree_add_text(subtree, tvb,
			offset, oct + 1,
			"User-Data Header");

		udh_subtree = proto_item_add_subtree(udh_item, ett_udh);

		proto_tree_add_text(udh_subtree,
		    tvb, offset, 1,
		    "User Data Header Length (%d)",
		    oct);

		offset++;
		udl--;
		length--;

		dis_field_ud_iei(tvb, udh_subtree, offset, oct);

		offset += oct;
		udl -= oct;
		length -= oct;

		if (seven_bit)
			{
		    /

		    fill_bits = 7 - (((oct + 1) * 8) % 7);
		    if (fill_bits != 7)
			    {
				oct = tvb_get_guint8(tvb, offset);

				other_decode_bitfield_value(bigbuf, oct, fill_bits_mask[fill_bits], 8);
				proto_tree_add_text(udh_subtree,
					tvb, offset, 1,
					"%s :  Fill bits",
					bigbuf);
			}
		}
    }

    if (compressed)
    {
		proto_tree_add_text(subtree, tvb,
		    offset, length,
		    "Compressed data");
    }
    else
    {
		if (seven_bit)
		{
		    out_len =
			gsm_sms_char_7bit_unpack(fill_bits, length, sizeof(bigbuf),
		    tvb_get_ptr(tvb, offset, length), bigbuf);
		    bigbuf[out_len] = '\0';
		    gsm_sms_char_ascii_decode(bigbuf, bigbuf, out_len);
			bigbuf[udl] = '\0';

			proto_tree_add_text(subtree, tvb, offset, length, "%s", bigbuf);
		}
		else if (eight_bit)
			{
			proto_tree_add_text(subtree, tvb, offset, length, "%s",
	        tvb_format_text(tvb, offset, length));
		}
		else if (ucs2)
			{
			/
			ustr = tvb_get_ephemeral_faked_unicode(tvb, offset, (length>>1), FALSE);
			proto_tree_add_text(subtree, tvb, offset, length, "%s", ustr);
		}
    }
}
