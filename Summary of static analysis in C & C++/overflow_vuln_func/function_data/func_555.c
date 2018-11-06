static int
dissect_freebsd_usb(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent, void *data _U_)
{
    int offset = 0;
    proto_item *ti;
    proto_tree *tree = NULL, *frame_tree = NULL;
    guint32 nframes;
    guint32 i;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "USB");

    /
    if (parent) {
      ti = proto_tree_add_protocol_format(parent, proto_usb, tvb, 0, 128,
                                          "USB URB");
      tree = proto_item_add_subtree(ti, ett_usb_hdr);
    }

    proto_tree_add_item(tree, hf_usb_totlen, tvb, 0, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_busunit, tvb, 4, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_address, tvb, 8, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_mode, tvb, 9, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_freebsd_urb_type, tvb, 10, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_freebsd_transfer_type, tvb, 11, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_bitmask(tree, tvb, 12, hf_usb_xferflags, ett_usb_xferflags,
                           usb_xferflags_fields, ENC_LITTLE_ENDIAN);
    proto_tree_add_bitmask(tree, tvb, 16, hf_usb_xferstatus, ett_usb_xferstatus,
                           usb_xferstatus_fields, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_error, tvb, 20, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_interval, tvb, 24, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item_ret_uint(tree, hf_usb_nframes, tvb, 28, 4, ENC_LITTLE_ENDIAN, &nframes);
    proto_tree_add_item(tree, hf_usb_packet_size, tvb, 32, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_packet_count, tvb, 36, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_endpoint_number, tvb, 40, 4, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(tree, hf_usb_speed, tvb, 44, 1, ENC_LITTLE_ENDIAN);

    offset += 128;
    for (i = 0; i < nframes; i++) {
        guint32 framelen;
        guint64 frameflags;

        frame_tree = proto_tree_add_subtree_format(tree, tvb, offset, -1, ett_usb_frame, &ti, "Frame %u", i);
        proto_tree_add_item_ret_uint(frame_tree, hf_usb_frame_length, tvb, offset, 4, ENC_LITTLE_ENDIAN, &framelen);
        proto_tree_add_bitmask_ret_uint64(frame_tree, tvb, offset + 4,
                                          hf_usb_frame_flags,
                                          ett_usb_frame_flags,
                                          usb_frame_flags_fields,
                                          ENC_LITTLE_ENDIAN, &frameflags);
        offset += 8;
        if (frameflags & FREEBSD_FRAMEFLAG_DATA_FOLLOWS)
            offset += (framelen + 3) & ~3;
        proto_item_set_end(ti, tvb, offset);
    }

    return tvb_captured_length(tvb);
}
