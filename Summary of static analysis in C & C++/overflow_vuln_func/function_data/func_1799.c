static int
dissect_bthci_evt_eir_ad_data(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, guint8 size)
{
    guint16 i;
    guint8 j, length, type;
    proto_item *ti_eir=NULL;
    proto_item *ti_eir_subtree=NULL;

    if(tree){
        if(size == 240 ) { /
            ti_eir=proto_tree_add_text(tree, tvb, offset, 240, "Extended Inquiry Response Data");
        }
        else { /
            ti_eir=proto_tree_add_text(tree, tvb, offset, size, "Advertising Data");
        }
        ti_eir_subtree=proto_item_add_subtree(ti_eir, ett_eir_subtree);
    }

    i=0;
    while(i<size){
        length = tvb_get_guint8(tvb, offset+i);
        if( length != 0 ){

            proto_item *ti_eir_struct=NULL;
            proto_tree *ti_eir_struct_subtree=NULL;

            ti_eir_struct = proto_tree_add_text(ti_eir_subtree, tvb, offset+i, length+1, "%s", "");
            ti_eir_struct_subtree = proto_item_add_subtree(ti_eir_struct, ett_eir_struct_subtree);

            type = tvb_get_guint8(tvb, offset+i+1);

            proto_item_append_text(ti_eir_struct,"%s", val_to_str_ext_const(type, &bthci_cmd_eir_data_type_vals_ext, "Unknown"));

            proto_tree_add_item(ti_eir_struct_subtree,hf_bthci_evt_eir_struct_length, tvb, offset+i, 1, ENC_LITTLE_ENDIAN);
            proto_tree_add_item(ti_eir_struct_subtree,hf_bthci_evt_eir_struct_type, tvb, offset+i+1, 1, ENC_LITTLE_ENDIAN);

            switch(type) {
                case 0x01: /
                    if(length-1 > 0)
                    {
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_limited_disc_mode, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_general_disc_mode, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_bredr_not_support, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_bredr_support_ctrl, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_bredr_support_host, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    }
                    break;
                case 0x02: /
                case 0x03: /
                case 0x14: /
                    j=0;
                    while(j<(guint8)(length-1))
                    {
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_sc_uuid16, tvb, offset+i+j+2, 2, ENC_LITTLE_ENDIAN);
                        j+=2;
                    }
                    break;
                case 0x04: /
                case 0x05: /
                    j=0;
                    while(j<(guint8)(length-1))
                    {
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_sc_uuid32, tvb, offset+i+j+2, 4, ENC_LITTLE_ENDIAN);
                        j+=4;
                    }
                    break;
                case 0x06: /
                case 0x07: /
                    j=0;
                    while(j<(guint8)(length-1))
                    {
                        proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_sc_uuid128, tvb, offset+i+j+2, 16, ENC_NA);
                        j+=16;
                    }
                    break;
                case 0x08: /
                case 0x09: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_device_name, tvb, offset+i+2, length-1, ENC_ASCII|ENC_NA);
                    proto_item_append_text(ti_eir_struct,": %s", tvb_format_text(tvb,offset+i+2,length-1));
                    break;
                case 0x0A: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_transmit_power_level, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    break;
                case 0x0D: /
                    dissect_bthci_evt_cod(tvb, offset+i+2, pinfo, ti_eir_struct_subtree);
                    break;
                case 0x0E: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_hash_c, tvb, offset+i+2, 16, ENC_NA);
                    break;
                case 0x0F: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_randomizer_r, tvb, offset+i+2, 16, ENC_NA);
                    break;
                case 0x11: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_oob_data_present, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_oob_le_supported_host, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_oob_le_bredr_support, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_flags_le_oob_address_type, tvb, offset+i+2, 1, ENC_LITTLE_ENDIAN);
                    break;
                case 0x12: /
                {
                    proto_item *item;

                    item = proto_tree_add_item(tree, hf_bthci_evt_le_con_interval, tvb, offset+i+2, 2, ENC_LITTLE_ENDIAN);
                    proto_item_append_text(item, " Min (%g msec)",  tvb_get_letohs(tvb, offset+i+2)*1.25);
                    item = proto_tree_add_item(tree, hf_bthci_evt_le_con_interval, tvb, offset+i+4, 2, ENC_LITTLE_ENDIAN);
                    proto_item_append_text(item, " Max (%g msec)",  tvb_get_letohs(tvb, offset+i+4)*1.25);
                    proto_item_append_text(ti_eir_struct,": %g - %g msec", tvb_get_letohs(tvb, offset+i+2)*1.25, tvb_get_letohs(tvb, offset+i+4)*1.25);
                    break;
                }
                case 0x16: /
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_sc_uuid16, tvb, offset+i+2, 2, ENC_LITTLE_ENDIAN);
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_data, tvb, offset+i+4, length-3, ENC_LITTLE_ENDIAN);
                    break;
                case 0x17: /
                case 0x18: /
                {
                    j=0;
                    while(j<(guint8)(length-1))
                    {
                        dissect_bthci_evt_bd_addr(tvb, offset+i+j+2, pinfo, ti_eir_struct_subtree);
                        j+=6;
                    }
                    break;
                }
                case 0x19: /
                {
                    guint16 appearance = tvb_get_letohs(tvb, offset+i+2);
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_appearance, tvb, offset+i+2, 2, ENC_LITTLE_ENDIAN);
                    proto_item_append_text(ti_eir_struct,": %s", val_to_str_ext(appearance, &bthci_cmd_appearance_vals_ext, "Unknown"));
                    break;
                }
                default:
                    proto_tree_add_item(ti_eir_struct_subtree, hf_bthci_evt_data, tvb, offset+i+2, length-1, ENC_NA);
                    break;
            }
            i += length+1;
        }
        else {
            break;
        }
    }

    return offset+size;
}
