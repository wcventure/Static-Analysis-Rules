void clear_usb_conv_tmp_data(usb_conv_info_t *usb_conv_info)
{
    /

    usb_conv_info->direction = P2P_DIR_UNKNOWN;
    usb_conv_info->transfer_type = URB_UNKNOWN;
    usb_conv_info->is_request = FALSE;
    usb_conv_info->is_setup = FALSE;
    usb_conv_info->setup_requesttype = 0;

    /

    if (usb_conv_info->endpoint==0) {
        usb_conv_info->interfaceClass    = IF_CLASS_UNKNOWN;
        usb_conv_info->interfaceSubclass = IF_SUBCLASS_UNKNOWN;
        usb_conv_info->interfaceProtocol = IF_PROTOCOL_UNKNOWN;
    }
}
