static void
dis_field_scts_aux(tvbuff_t *tvb, proto_tree *tree, guint32 offset)
{
    guint8	oct, oct2, oct3;
    char sign;


    oct = tvb_get_guint8(tvb, offset);
    oct2 = tvb_get_guint8(tvb, offset+1);
    oct3 = tvb_get_guint8(tvb, offset+2);

    proto_tree_add_text(tree,
	tvb, offset, 3,
	"Year %d%d, Month %d%d, Day %d%d",
	oct & 0x0f,
	(oct & 0xf0) >> 4,
	oct2 & 0x0f,
	(oct2 & 0xf0) >> 4,
	oct3 & 0x0f,
	(oct3 & 0xf0) >> 4);

    offset += 3;

    oct = tvb_get_guint8(tvb, offset);
    oct2 = tvb_get_guint8(tvb, offset+1);
    oct3 = tvb_get_guint8(tvb, offset+2);

    proto_tree_add_text(tree,
	tvb, offset, 3,
	"Hour %d%d, Minutes %d%d, Seconds %d%d",
	oct & 0x0f,
	(oct & 0xf0) >> 4,
	oct2 & 0x0f,
	(oct2 & 0xf0) >> 4,
	oct3 & 0x0f,
	(oct3 & 0xf0) >> 4);

    offset += 3;

    oct = tvb_get_guint8(tvb, offset);

    sign = (oct & 0x08)?'-':'+';
    oct = ((oct >> 4) + (oct & 0x07) * 10) * 15;

    proto_tree_add_text(tree,
	tvb, offset, 1,
	"Timezone: GMT %c %d hours %d minutes",
	sign, oct / 60, oct % 60);
}
