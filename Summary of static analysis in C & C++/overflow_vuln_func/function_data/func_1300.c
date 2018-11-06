static int 
ospf_has_lls_block(tvbuff_t *tvb, int offset, guint8 packet_type)
{
    guint8 flags;

    /
    switch (packet_type) {
    case OSPF_HELLO:
	flags = tvb_get_guint8 (tvb, offset + 6);
	return flags & OSPF_V2_OPTIONS_L;
    case OSPF_DB_DESC:
	flags = tvb_get_guint8 (tvb, offset + 2);
	return flags & OSPF_V2_OPTIONS_L;
    }

    return 0;
}
