static void dissect_ospf_v3_address_prefix(tvbuff_t *tvb, int offset, int prefix_length, proto_tree *tree)
{

    guint8 value;
    guint8 position;
    guint8 bufpos;
    gchar  *buffer;
    gchar  *bytebuf;
    guint8 bytes_to_process;
    int start_offset;

    start_offset=offset;
    position=0;
    bufpos=0;
    bytes_to_process=((prefix_length+31)/32)*4;

    buffer=ep_alloc(32+7);
    while (bytes_to_process > 0 ) {

        value=tvb_get_guint8(tvb, offset);

        if ( (position > 0) && ( (position%2) == 0 ) )
	    buffer[bufpos++]=':';

	bytebuf=ep_alloc(3);
        g_snprintf(bytebuf, 3, "%02x",value);
        buffer[bufpos++]=bytebuf[0];
        buffer[bufpos++]=bytebuf[1];

	position++;
	offset++;
        bytes_to_process--;
    }

    buffer[bufpos]=0;
    proto_tree_add_text(tree, tvb, start_offset, ((prefix_length+31)/32)*4, "Address Prefix: %s",buffer);

}
