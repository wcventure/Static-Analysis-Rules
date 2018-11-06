static int
dissect_usb_com_bulk(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    usb_conv_info_t *usb_conv_info = (usb_conv_info_t *)data;

    if (usb_conv_info) {
        switch (usb_conv_info->interfaceProtocol)
        {
            case 0x01: /
            case 0x02: /
                return call_dissector_only(mbim_bulk_handle, tvb, pinfo, tree, NULL);
                break;
            default:
                break;
        }
    }

    /
    return call_dissector_only(eth_withoutfcs_handle, tvb, pinfo, tree, NULL);
}
