static void
dis_field_addr(tvbuff_t *tvb, proto_tree *tree, guint32 *offset_p, const gchar *title)
{
    static gchar	digit_table[] = {"0123456789*#abc\0"};
    proto_item		*item;
    proto_tree		*subtree = NULL;
    const gchar		*str = NULL;
    guint8		oct;
    guint32		offset;
    guint32		numdigocts;
    guint32		length;
    guint32		i, j;
    char                addrbuf[20];

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
    case 0x03: str = "Network specific"; break;
    case 0x04: str = "Subscriber"; break;
    case 0x05: str = "Alphanumeric (coded according to 3GPP TS 23.038 GSM 7-bit default alphabet)"; break;
    case 0x06: str = "Abbreviated number"; break;
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
    case 0x01: str = "ISDN/telephone (E.164/E.163)"; break;
    case 0x03: str = "Data numbering plan (X.121)"; break;
    case 0x04: str = "Telex numbering plan"; break;
    case 0x05: str = "Service Centre Specific plan"; break;
    case 0x06: str = "Service Centre Specific plan"; break;
    case 0x08: str = "National numbering plan"; break;
    case 0x09: str = "Private numbering plan"; break;
    case 0x0a: str = "ERMES numbering plan (ETSI DE/PS 3 01-3)"; break;
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
    switch ((oct & 0x70) >> 4)
    {
    case 0x05: /
	i = gsm_sms_char_7bit_unpack(0, numdigocts, sizeof(addrbuf), tvb_get_ptr(tvb, offset, numdigocts), addrbuf);
	addrbuf[i] = '\0';
	gsm_sms_char_ascii_decode(bigbuf, addrbuf, i);
	break;
    default:
	for (i = 0; i < numdigocts; i++)
	{
	    oct = tvb_get_guint8(tvb, offset + i);

	    bigbuf[j++] = digit_table[oct & 0x0f];
	    bigbuf[j++] = digit_table[(oct & 0xf0) >> 4];
	}
	bigbuf[j++] = '\0';
	break;
    }

    proto_tree_add_text(subtree,
	tvb, offset, numdigocts,
	"Digits: %s",
	bigbuf);

    proto_item_append_text(item, " - (%s)", bigbuf);

    *offset_p = offset + numdigocts;
}
