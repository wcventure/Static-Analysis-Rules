static void wimaxasncp_dissect_tlv_value(
    tvbuff_t           *tvb,
    packet_info *pinfo  _U_,
    proto_tree         *tree,
    proto_item         *tlv_item,
    const wimaxasncp_dict_tlv_t *tlv_info)
{
    guint        offset          = 0;
    guint        length;
    const guint  max_show_bytes  = 24; /
    static const gchar *hex_note = "[hex]";

    length = tvb_reported_length(tvb);

    switch (tlv_info->decoder)
    {
    case WIMAXASNCP_TLV_ENUM8:
    {
        if (length != 1)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            if (debug_enabled)
            {
                g_print("fix-me: enum values missing for TLV %s (%u)\n",
                        tlv_info->name, tlv_info->type);
            }
        }

        if (tree)
        {
            guint8       value;
            const gchar *s;

            value = tvb_get_guint8(tvb, offset);

            s = wimaxasncp_get_enum_name(tlv_info, value);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %s (%u)", s, value);

            proto_item_append_text(tlv_item, " - %s", s);
        }

        return;
    }
    case WIMAXASNCP_TLV_ENUM16:
    {
        if (length != 2)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            if (debug_enabled)
            {
                g_print("fix-me: enum values missing for TLV %s (%u)\n",
                        tlv_info->name, tlv_info->type);
            }
        }

        if (tree)
        {
            guint16      value;
            const gchar *s;

            value = tvb_get_ntohs(tvb, offset);

            s = wimaxasncp_get_enum_name(tlv_info, value);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %s (%u)", s, value);

            proto_item_append_text(tlv_item, " - %s", s);
        }

        return;
    }
    case WIMAXASNCP_TLV_ENUM32:
    {
        if (length != 4)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            if (debug_enabled)
            {
                g_print("fix-me: enum values missing for TLV %s (%u)\n",
                        tlv_info->name, tlv_info->type);
            }
        }

        if (tree)
        {
            guint32      value;
            const gchar *s;

            value = tvb_get_ntohl(tvb, offset);

            s = wimaxasncp_get_enum_name(tlv_info, value);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %s (%u)", s, value);

            proto_item_append_text(tlv_item, " - %s", s);
        }

        return;
    }
    case WIMAXASNCP_TLV_ETHER:
    {
        if (length != 6)
        {
            /
            break;
        }

        if (tree)
        {
            wimaxasncp_proto_tree_add_ether_value(
                tvb, tree, tlv_item, offset, length, tlv_info);
        }

        return;
    }
    case WIMAXASNCP_TLV_ASCII_STRING:
    {
        if (tree)
        {
            const guint8 *p;
            const gchar  *s = tvb_get_string(wmem_packet_scope(), tvb, offset, length);

            p = tvb_get_ptr(tvb, offset, length);

            proto_tree_add_string_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, p,
                "Value: %s", s);

            proto_item_append_text(
                tlv_item, " - %s", s);
        }

        return;
    }
    case WIMAXASNCP_TLV_FLAG0:
    {
        if (length != 0)
        {
            /
            break;
        }

        return;
    }
    case WIMAXASNCP_TLV_BITFLAGS8:
    {
        if (length != 1)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            /
        }

        if (tree)
        {
            proto_tree *flags_tree;
            proto_item *item;
            guint8      value;
            guint       i;

            value = tvb_get_guint8(tvb, offset);

            item = proto_tree_add_item(
                tree, tlv_info->hf_value,
                tvb, offset, 1, ENC_NA);

            proto_item_append_text(tlv_item, " - 0x%02x", value);

            if (value != 0)
            {
                flags_tree = proto_item_add_subtree(
                    item, ett_wimaxasncp_tlv_value_bitflags8);

                for (i = 0; i < 8; ++i)
                {
                    guint8 mask;
                    mask = 1 << (7 - i);

                    if (value & mask)
                    {
                        const gchar *s;

                        s = wimaxasncp_get_enum_name(tlv_info, value & mask);

                        proto_tree_add_uint_format(
                            flags_tree, hf_wimaxasncp_tlv_value_bitflags8,
                            tvb, offset, length, value,
                            "Bit #%u is set: %s", i, s);
                    }
                }
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_BITFLAGS16:
    {
        if (length != 2)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            /
        }

        if (tree)
        {
            proto_tree *flags_tree;
            proto_item *item;
            guint16     value;
            guint       i;

            value = tvb_get_ntohs(tvb, offset);

            item = proto_tree_add_item(
                tree, tlv_info->hf_value,
                tvb, offset, 2, ENC_BIG_ENDIAN);

            proto_item_append_text(tlv_item, " - 0x%04x", value);

            if (value != 0)
            {
                flags_tree = proto_item_add_subtree(
                    item, ett_wimaxasncp_tlv_value_bitflags16);

                for (i = 0; i < 16; ++i)
                {
                    guint16 mask;
                    mask = 1 << (15 - i);

                    if (value & mask)
                    {
                        const gchar *s;

                        s = wimaxasncp_get_enum_name(tlv_info, value & mask);

                        proto_tree_add_uint_format(
                            flags_tree, hf_wimaxasncp_tlv_value_bitflags16,
                            tvb, offset, length, value,
                            "Bit #%u is set: %s", i, s);
                    }
                }
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_BITFLAGS32:
    {
        if (length != 4)
        {
            /
            break;
        }

        if (tlv_info->enums == NULL)
        {
            /
        }

        if (tree)
        {
            proto_tree *flags_tree;
            proto_item *item;
            guint32     value;
            guint       i;

            value = tvb_get_ntohl(tvb, offset);

            item = proto_tree_add_item(
                tree, tlv_info->hf_value,
                tvb, offset, 4, ENC_BIG_ENDIAN);

            proto_item_append_text(tlv_item, " - 0x%08x", value);

            if (value != 0)
            {
                flags_tree = proto_item_add_subtree(
                    item, ett_wimaxasncp_tlv_value_bitflags32);

                for (i = 0; i < 32; ++i)
                {
                    guint32 mask;
                    mask = 1 << (31 - i);

                    if (value & mask)
                    {
                        const gchar *s;
                        s = wimaxasncp_get_enum_name(tlv_info, value & mask);

                        proto_tree_add_uint_format(
                            flags_tree, hf_wimaxasncp_tlv_value_bitflags32,
                            tvb, offset, length, value,
                            "Bit #%u is set: %s", i, s);
                    }
                }
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_ID:
    {
        if (length == 4)
        {
            if (tree)
            {
                wimaxasncp_proto_tree_add_tlv_ipv4_value(
                    tvb, tree, tlv_item, offset, tlv_info);
            }

            return;
        }
        else if (length == 6)
        {
            if (tree)
            {
                wimaxasncp_proto_tree_add_ether_value(
                    tvb, tree, tlv_item, offset, length, tlv_info);
            }

            return;
        }
        else if (length == 16)
        {
            if (tree)
            {
                wimaxasncp_proto_tree_add_tlv_ipv6_value(
                    tvb, tree, tlv_item, offset, tlv_info);
            }

            return;
        }
        else
        {
            /
            break;
        }
    }
    case WIMAXASNCP_TLV_BYTES:
    {
        if (tree)
        {
            const gchar  *format1;
            const gchar  *format2;
            const guint8 *p = tvb_get_ptr(tvb, offset, length);
            const gchar  *s = bytestring_to_str(wmem_packet_scope(), p, MIN(length, max_show_bytes), 0);

            if (length <= max_show_bytes)
            {
                format1 = "Value: %s";
                format2 = " - %s";
            }
            else
            {
                format1 = "Value: %s...";
                format2 = " - %s...";
            }

            proto_tree_add_bytes_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, p,
                format1, s);

            proto_item_append_text(
                tlv_item, format2, s);
        }

        return;
    }
    case WIMAXASNCP_TLV_HEX8:
    {
        if (length != 1)
        {
            /
            break;
        }

        if (tree)
        {
            guint8 value;

            value = tvb_get_guint8(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: 0x%02x", value);

            proto_item_append_text(tlv_item, " - 0x%02x", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_HEX16:
    {
        if (length != 2)
        {
            /
            break;
        }

        if (tree)
        {
            guint16 value;

            value = tvb_get_ntohs(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: 0x%04x", value);

            proto_item_append_text(tlv_item, " - 0x%04x", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_HEX32:
    {
        if (length != 4)
        {
            /
            break;
        }

        if (tree)
        {
            guint32 value;

            value = tvb_get_ntohl(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: 0x%08x", value);

            proto_item_append_text(tlv_item, " - 0x%08x", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_DEC8:
    {
        if (length != 1)
        {
            /
            break;
        }

        if (tree)
        {
            guint8 value;

            value = tvb_get_guint8(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %u", value);

            proto_item_append_text(tlv_item, " - %u", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_DEC16:
    {
        if (length != 2)
        {
            /
            break;
        }

        if (tree)
        {
            guint16 value;

            value = tvb_get_ntohs(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %u", value);

            proto_item_append_text(tlv_item, " - %u", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_DEC32:
    {
        if (length != 4)
        {
            /
            break;
        }

        if (tree)
        {
            guint32 value;

            value = tvb_get_ntohl(tvb, offset);

            proto_tree_add_uint_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, value,
                "Value: %u", value);

            proto_item_append_text(tlv_item, " - %u", value);
        }

        return;
    }
    case WIMAXASNCP_TLV_TBD:
    {
        if (debug_enabled)
        {
            g_print(
                "fix-me: TBD: TLV %s (%u)\n", tlv_info->name, tlv_info->type);
        }

        if (tree)
        {
            const gchar  *format;
            const guint8 *p = tvb_get_ptr(tvb, offset, length);
            const gchar  *s = bytestring_to_str(wmem_packet_scope(), p, MIN(length, max_show_bytes), 0);

            if (length <= max_show_bytes)
            {
                format = "Value: %s %s";
            }
            else
            {
                format = "Value: %s %s...";
            }

            proto_tree_add_bytes_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, p,
                format, hex_note, s);

            proto_item_append_text(tlv_item, " - TBD");
        }

        return;
    }
    case WIMAXASNCP_TLV_IP_ADDRESS:
    {
        if (length == 4)
        {
            if (tree)
            {
                wimaxasncp_proto_tree_add_tlv_ipv4_value(
                    tvb, tree, tlv_item, offset, tlv_info);
            }

            return;
        }
        else if (length == 16)
        {
            if (tree)
            {
                wimaxasncp_proto_tree_add_tlv_ipv6_value(
                    tvb, tree, tlv_item, offset, tlv_info);
            }

            return;
        }
        else
        {
            /
            break;
        }
    }
    case WIMAXASNCP_TLV_IPV4_ADDRESS:
    {
        if (length != 4)
        {
            /
            break;
        }

        if (tree)
        {
            wimaxasncp_proto_tree_add_tlv_ipv4_value(
                tvb, tree, tlv_item, offset, tlv_info);
        }

        return;
    }
    case WIMAXASNCP_TLV_PROTOCOL_LIST:
    {
        if (length % 2 != 0)
        {
            /
            break;
        }

        if (tree && length > 0)
        {
            proto_tree  *protocol_list_tree;
            proto_item  *item;
            const guint  max_protocols_in_tlv_item = 8; /

            item = proto_tree_add_text(
                tree, tvb, offset, length,
                "Value");

            protocol_list_tree = proto_item_add_subtree(
                item, ett_wimaxasncp_tlv_protocol_list);

            /
            item = proto_tree_add_item(
                protocol_list_tree, tlv_info->hf_value,
                tvb, offset, length, ENC_NA);

            PROTO_ITEM_SET_HIDDEN(item);

            while (offset < tvb_length(tvb))
            {
                guint16      protocol;
                const gchar *protocol_name;

                protocol = tvb_get_ntohs(tvb, offset);
                protocol_name = ipprotostr(protocol);

                proto_tree_add_uint_format(
                    protocol_list_tree, tlv_info->hf_protocol,
                    tvb, offset, 2, protocol,
                    "Protocol: %s (%u)", protocol_name, protocol);

                if (offset == 0)
                {
                    proto_item_append_text(tlv_item, " - %s", protocol_name);
                }
                else if (offset < 2 * max_protocols_in_tlv_item)
                {
                    proto_item_append_text(tlv_item, ", %s", protocol_name);
                }
                else if (offset == 2 * max_protocols_in_tlv_item)
                {
                    proto_item_append_text(tlv_item, ", ...");
                }

                offset += 2;
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_PORT_RANGE_LIST:
    {
        if (length % 4 != 0)
        {
            /
            break;
        }

        if (tree && length > 0)
        {
            proto_tree  *port_range_list_tree;
            proto_item  *item;
            const guint  max_port_ranges_in_tlv_item = 3; /

            item = proto_tree_add_text(
                tree, tvb, offset, length,
                "Value");

            port_range_list_tree = proto_item_add_subtree(
                item, ett_wimaxasncp_tlv_port_range_list);

            /
            item = proto_tree_add_item(
                port_range_list_tree, tlv_info->hf_value,
                tvb, offset, length, ENC_NA);

            PROTO_ITEM_SET_HIDDEN(item);

            while (offset < tvb_length(tvb))
            {
                guint16 portLow;
                guint16 portHigh;

                portLow  = tvb_get_ntohs(tvb, offset);
                portHigh = tvb_get_ntohs(tvb, offset + 2);

                proto_tree_add_text(
                    port_range_list_tree, tvb, offset, 4,
                    "Port Range: %u-%u", portLow, portHigh);

                /

                item = proto_tree_add_item(
                    port_range_list_tree, tlv_info->hf_port_low,
                    tvb, offset, 2, ENC_BIG_ENDIAN);

                PROTO_ITEM_SET_HIDDEN(item);

                item = proto_tree_add_item(
                    port_range_list_tree, tlv_info->hf_port_high,
                    tvb, offset + 2, 2, ENC_BIG_ENDIAN);

                PROTO_ITEM_SET_HIDDEN(item);

                if (offset == 0)
                {
                    proto_item_append_text(
                        tlv_item, " - %u-%u", portLow, portHigh);
                }
                else if (offset < 4 * max_port_ranges_in_tlv_item)
                {
                    proto_item_append_text(
                        tlv_item, ", %u-%u", portLow, portHigh);
                }
                else if (offset == 4 * max_port_ranges_in_tlv_item)
                {
                    proto_item_append_text(tlv_item, ", ...");
                }

                offset += 4;
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_IP_ADDRESS_MASK_LIST:
    {
        /

        if (length % 8 != 0)
        {
            /
            break;
        }

        if (tree && length > 0)
        {
            proto_tree *ip_address_mask_list_tree;
            proto_item *item;

            item = proto_tree_add_text(
                tree, tvb, offset, length,
                "Value");

            ip_address_mask_list_tree = proto_item_add_subtree(
                item, ett_wimaxasncp_tlv_ip_address_mask_list);

            /
            item = proto_tree_add_item(
                ip_address_mask_list_tree, tlv_info->hf_value,
                tvb, offset, length, ENC_NA);

            PROTO_ITEM_SET_HIDDEN(item);

            if (length % 32 == 0)
            {
                /

                while (offset < tvb_length(tvb))
                {
                    proto_tree        *ip_address_mask_tree;
                    struct e_in6_addr  ip;
                    const gchar       *s;

                    item = proto_tree_add_text(
                        ip_address_mask_list_tree, tvb, offset, 32,
                        "IPv6 Address and Mask");

                    ip_address_mask_tree = proto_item_add_subtree(
                        item, ett_wimaxasncp_tlv_ip_address_mask);

                    /

                    tvb_get_ipv6(tvb, offset, &ip);

                    proto_tree_add_item(
                        ip_address_mask_tree,
                        tlv_info->hf_ipv6,
                        tvb, offset, 16, ENC_NA);

                    /

                    offset += 16;

                    /

                    tvb_get_ipv6(tvb, offset, &ip);

                    s = ip6_to_str(&ip);

                    proto_tree_add_ipv6_format_value(
                        ip_address_mask_tree,
                        tlv_info->hf_ipv6_mask,
                        tvb, offset, 16, (const guint8*)&ip,
                        "%s", s);

                    /

                    offset += 16;
                }
            }
            else
            {
                /

                while (offset < tvb_length(tvb))
                {
                    proto_tree  *ip_address_mask_tree;
                    guint32      ip;
                    const gchar *s;

                    item = proto_tree_add_text(
                        ip_address_mask_list_tree, tvb, offset, 8,
                        "IPv4 Address and Mask");

                    ip_address_mask_tree = proto_item_add_subtree(
                        item, ett_wimaxasncp_tlv_ip_address_mask);

                    /

                    ip = tvb_get_ipv4(tvb, offset);

                    proto_tree_add_item(
                        ip_address_mask_tree,
                        tlv_info->hf_ipv4,
                        tvb, offset, 4, ENC_BIG_ENDIAN);

                    proto_item_append_text(
                        item, " - %s (%s)",
                        get_hostname(ip), ip_to_str((guint8 *)&ip));

                    offset += 4;

                    /

                    ip = tvb_get_ipv4(tvb, offset);

                    s = ip_to_str((guint8 *)&ip);

                    proto_tree_add_ipv4_format_value(
                        ip_address_mask_tree,
                        tlv_info->hf_ipv4_mask,
                        tvb, offset, 4, ip,
                        "%s", s);

                    proto_item_append_text(
                        item, " / %s", s);

                    offset += 4;
                }
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_EAP:
    {
        /
        guint8 eap_code;
        guint8 eap_type = 0;

        /
        eap_code = tvb_get_guint8(tvb, offset);
        if (eap_code == EAP_REQUEST || eap_code == EAP_RESPONSE)
        {
            /
            eap_type = tvb_get_guint8(tvb, offset + 4);
        }

        /
        col_append_str(pinfo->cinfo, COL_INFO, " [");
        col_append_str(pinfo->cinfo, COL_INFO,
                        val_to_str(eap_code, eap_code_vals, "Unknown code (0x%02X)"));

        if (eap_code == EAP_REQUEST || eap_code == EAP_RESPONSE)
        {
            col_append_str(pinfo->cinfo, COL_INFO, ", ");
            col_append_str(pinfo->cinfo, COL_INFO,
                            val_to_str_ext(eap_type, &eap_type_vals_ext, "Unknown type (0x%02X)"));
        }

        col_append_str(pinfo->cinfo, COL_INFO, "]");


        {
            proto_tree *eap_tree;
            proto_item *item;
            gboolean save_writable;
            tvbuff_t *eap_tvb;

            /
            item = proto_tree_add_item(tree, tlv_info->hf_value, tvb,
                                       offset, length, ENC_NA);
            proto_item_set_text(item, "Value");
            eap_tree = proto_item_add_subtree(item, ett_wimaxasncp_tlv_eap);

            /
            proto_item_append_text(item, " (%s",
                                   val_to_str(eap_code, eap_code_vals,
                                              "Unknown code (0x%02X)"));
            if (eap_code == EAP_REQUEST || eap_code == EAP_RESPONSE)
            {
                proto_item_append_text(item, ", %s",
                                       val_to_str_ext(eap_type, &eap_type_vals_ext,
                                       "Unknown type (0x%02X)"));
            }
            proto_item_append_text(item, ")");


            /
            eap_tvb = tvb_new_subset(tvb, offset, length,
                                     tvb_length_remaining(tvb, offset));

            /
            save_writable = col_get_writable(pinfo->cinfo);
            col_set_writable(pinfo->cinfo, FALSE);

            /
            call_dissector(eap_handle, eap_tvb, pinfo, eap_tree);

            /
            col_set_writable(pinfo->cinfo, save_writable);
        }

        return;
    }

    case WIMAXASNCP_TLV_VENDOR_SPECIFIC:
    {
        /

        if (length < 3)
        {
            /
            break;
        }

        if (tree)
        {
            proto_tree *vsif_tree;
            proto_item *item;
            guint32 vendorId;
            const gchar *vendorName;

            item = proto_tree_add_text(
                tree, tvb, offset, length,
                "Value");

            vsif_tree = proto_item_add_subtree(
                item, ett_wimaxasncp_tlv_vendor_specific_information_field);

            /
            item = proto_tree_add_item(
                vsif_tree, tlv_info->hf_value,
                tvb, offset, length, ENC_NA);

            PROTO_ITEM_SET_HIDDEN(item);

            /

            vendorId = tvb_get_ntoh24(tvb, offset);

            vendorName = val_to_str_ext_const(vendorId, &sminmpec_values_ext, "Unknown");
            proto_tree_add_uint_format(
                vsif_tree, tlv_info->hf_vendor_id,
                tvb, offset, 3, vendorId,
                "Vendor ID: %s (%u)", vendorName, vendorId);

            proto_item_append_text(tlv_item, " - %s", vendorName);

            offset += 3;

            /

            if (offset < tvb_length(tvb))
            {
                proto_tree_add_item(
                    vsif_tree, tlv_info->hf_vendor_rest_of_info,
                    tvb, offset, length - offset, ENC_NA);
            }
        }

        return;
    }
    case WIMAXASNCP_TLV_UNKNOWN:
    {
        if (tree)
        {
            const gchar *format1;
            const gchar *format2;
            const guint8 *p = tvb_get_ptr(tvb, offset, length);
            const gchar *s =
                bytestring_to_str(wmem_packet_scope(), p, MIN(length, max_show_bytes), 0);

            if (length <= max_show_bytes)
            {
                format1 = "Value: %s %s";
                format2 = " - %s %s";
            }
            else
            {
                format1 = "Value: %s %s...";
                format2 = " - %s %s...";
            }

            proto_tree_add_bytes_format(
                tree, tlv_info->hf_value,
                tvb, offset, length, p,
                format1, hex_note, s);

            proto_item_append_text(
                tlv_item, format2, hex_note, s);

        }

        return;
    }
    default:
        if (debug_enabled)
        {
            g_print(
                "fix-me: unknown decoder: %d\n", tlv_info->decoder);
        }
        break;
    }

    /

    if (tree)
    {
        const gchar *format;
        const guint8 *p = tvb_get_ptr(tvb, offset, length);
        const gchar *s = bytestring_to_str(wmem_packet_scope(), p, MIN(length, max_show_bytes), 0);

        if (length <= max_show_bytes)
        {
            format = "Value: %s %s";
        }
        else
        {
            format = "Value: %s %s...";
        }

        proto_tree_add_bytes_format(
            tree, hf_wimaxasncp_tlv_value_bytes,
            tvb, offset, length, p,
            format, hex_note, s);
    }
}
