static int
dissect_usb_string_descriptor(packet_info *pinfo _U_, proto_tree *parent_tree,
                              tvbuff_t *tvb, int offset,
                              usb_conv_info_t  *usb_conv_info)
{
    proto_item *item;
    proto_tree *tree;
    int         old_offset = offset;
    guint8      len;
    proto_item *len_item;
    usb_trans_info_t *usb_trans_info;

    usb_trans_info = usb_conv_info->usb_trans_info;

    tree = proto_tree_add_subtree(parent_tree, tvb, offset, -1, ett_descriptor_device, &item, "STRING DESCRIPTOR");

    len = tvb_get_guint8(tvb, offset);
    /
    if (len & 0x1) {
        /
        len_item = proto_tree_add_item(tree, hf_usb_bLength, tvb, offset, 1, ENC_LITTLE_ENDIAN);
        expert_add_info(pinfo, len_item, &ei_usb_bLength_even);

        /
        proto_tree_add_item(tree, hf_usb_bDescriptorType, tvb, offset+1, 1, ENC_LITTLE_ENDIAN);
    }
    else
       len_item = dissect_usb_descriptor_header(tree, tvb, offset, NULL);
    offset += 2;

    /
    if (len < 2) {
        expert_add_info(pinfo, len_item, &ei_usb_bLength_too_short);
        return offset;
    }

    if (!usb_trans_info->u.get_descriptor.index) {
        /
        while(len>(offset-old_offset)) {
            /
            proto_tree_add_item(tree, hf_usb_wLANGID, tvb, offset, 2, ENC_LITTLE_ENDIAN);
            offset+=2;
        }
    } else {
        /
        proto_tree_add_item(tree, hf_usb_bString, tvb, offset, len-2, ENC_UTF_16 | ENC_LITTLE_ENDIAN);
        offset += len-2;
    }

    proto_item_set_len(item, offset-old_offset);

    return offset;
}
