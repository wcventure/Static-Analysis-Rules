static void
dis_field_addr(tvbuff_t *tvb, proto_tree *tree, guint32 *offset_p, gchar *title)
{
    static gchar	digit_table[] = {"0123456789*#abc\0"};
    proto_item		*item;
    proto_tree		*subtree = NULL;
    gchar		*str = NULL;
    guint8		oct;
    guint32		offset;
    guint32		numdigocts;
    guint32		length;
    guint32		i, j;


    offset = *offset_p;

    oct = tvb_get_guint8(tvb, offset);
    numdigocts = (oct + 1) / 2;

    length = tvb_length_remaining(tvb, offset);

    if (length <= numdigocts)
    {
	proto_tree_add_text(tree,
	    tvb, offset, length,
	    "%s: Short Data (?)",
	    title);

	*offset_p += length;
	return;
    }

    item =
	proto_tree_add_text(tree, tvb,
	    offset, numdigocts + 2,
	    title);

    subtree = proto_item_add_subtree(item, ett_addr);

    proto_tree_add_text(subtree,
	tvb, offset, 1,
	"Length: %d address digits",
	oct);

    offset++;
    oct = tvb_get_guint8(tvb, offset);

    other_decode_bitfield_value(bigbuf, oct, 0x80, 8);
    proto_tree_add_text(subtree, tvb,
	offset, 1,
	"%s :  %s",
	bigbuf,
	(oct & 0x80) ? "No extension" : "Extended");

    switch ((oct & 0x70) >> 4)
    {
    case 0x00: str = "Unknown"; break;
    case 0x01: str = "International"; break;
    case 0x02: str = "National"; break;
    case 0x07: str = "Reserved for extension"; break;
    default: str = "Unknown, reserved (?)"; break;
    }

    other_decode_bitfield_value(bigbuf, oct, 0x70, 8);
    proto_tree_add_text(subtree,
	tvb, offset, 1,
	"%s :  Type of number: (%d) %s",
	bigbuf,
	(oct & 0x70) >> 4,
	str);

    switch (oct & 0x0f)
    {
    case 0x00: str = "Unknown"; break;
    case 0x01: str = "ISDN/telephone"; break;
    case 0x0f: str = "Reserved for extension"; break;
    default: str = "Unknown, reserved (?)"; break;
    }

    other_decode_bitfield_value(bigbuf, oct, 0x0f, 8);
    proto_tree_add_text(subtree,
	tvb, offset, 1,
	"%s :  Numbering plan: (%d) %s",
	bigbuf,
	oct & 0x0f,
	str);

    offset++;

    j = 0;
    for (i = 0; i < numdigocts; i++)
    {
	oct = tvb_get_guint8(tvb, offset + i);

        bigbuf[j++] = digit_table[oct & 0x0f];
        bigbuf[j++] = digit_table[(oct & 0xf0) >> 4];
    }

    bigbuf[j++] = '\0';

    proto_tree_add_text(subtree,
	tvb, offset, numdigocts,
	"Digits: %s",
	bigbuf);

    proto_item_append_text(item, " - (%s)", bigbuf);

    *offset_p = offset + i;
}
