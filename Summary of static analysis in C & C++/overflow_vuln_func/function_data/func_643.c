static int
dissect_usb_audio_bulk(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree, void *data)
{
    usb_conv_info_t *usb_conv_info;
    proto_tree      *tree   = NULL;
    guint            offset;
    guint            length = tvb_length(tvb);


    /
    if (data == NULL)
        return 0;
    usb_conv_info = (usb_conv_info_t *)data;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "USBAUDIO");

    if (parent_tree)
    {
        proto_item *ti;

        ti   = proto_tree_add_protocol_format(parent_tree, proto_usb_audio, tvb, 0, -1, "USB Audio");
        tree = proto_item_add_subtree(ti, ett_usb_audio);
    }

    switch (usb_conv_info->interfaceSubclass)
    {
        case AUDIO_IF_SUBCLASS_MIDISTREAMING:
            offset = 0;
            col_set_str(pinfo->cinfo, COL_INFO, "USB-MIDI Event Packets");

            while (offset < length)
            {
                dissect_usb_midi_event(tvb, pinfo, tree, parent_tree, offset);
                offset += 4;
            }
            break;
        default:
            offset = 0;
            proto_tree_add_expert(tree, pinfo, &ei_usb_audio_undecoded, tvb, offset, length - offset);
    }

    return tvb_length(tvb);
}
