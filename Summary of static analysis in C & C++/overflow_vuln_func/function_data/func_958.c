static void
dissect_prism(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    struct prism_hdr hdr;
    proto_tree *prism_tree;
    proto_item *ti;
    tvbuff_t *next_tvb;
    int offset;

    if(check_col(pinfo->cinfo, COL_PROTOCOL))
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "Prism");
    if(check_col(pinfo->cinfo, COL_INFO))
        col_clear(pinfo->cinfo, COL_INFO);

    offset = 0;
    tvb_memcpy(tvb, (guint8 *)&hdr, offset, sizeof hdr);

    if(check_col(pinfo->cinfo, COL_INFO))
        col_add_fstr(pinfo->cinfo, COL_INFO, "Device: %.16s  "
                     "Message 0x%x, Length %d", hdr.devname,
                     hdr.msgcode, hdr.msglen);

    if(tree) {
        ti = proto_tree_add_protocol_format(tree, proto_prism,
            tvb, 0, sizeof hdr, "Prism Monitoring Header");
        prism_tree = proto_item_add_subtree(ti, ett_prism);

        INTFIELD(4, msgcode, "Message Code: %d");
        INTFIELD(4, msglen, "Message Length: %d");
        proto_tree_add_text(prism_tree, tvb, offset, sizeof hdr.devname,
            "Device: %s", hdr.devname);
        offset += sizeof hdr.devname;

        VALFIELD(hosttime, "Host Time");
        VALFIELD(mactime, "MAC Time");
        VALFIELD(channel, "Channel Time");
        VALFIELD(rssi, "RSSI");
        VALFIELD(sq, "SQ");
        VALFIELD(signal, "Signal");
        VALFIELD(noise, "Noise");
        VALFIELD(rate, "Rate");
        VALFIELD(istx, "IsTX");
        VALFIELD(frmlen, "Frame Length");
    }

    /
    next_tvb = tvb_new_subset(tvb, sizeof hdr, -1, -1);
    call_dissector(ieee80211_handle, next_tvb, pinfo, tree);
}
