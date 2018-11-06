static gboolean dissect_ircomm_parameters(tvbuff_t* tvb, unsigned offset, packet_info* pinfo _U_,
                                          proto_tree* tree, unsigned list_index, guint8 attr_type)
{
    unsigned    len;
    unsigned    n = 0;
    proto_item* ti;
    proto_tree* p_tree;
    char        buf[256];
    guint8      pv;


    if (!check_iap_octet_result(tvb, tree, offset, "Parameters", attr_type))
        return TRUE;

    if (tree)
    {
        len = tvb_get_ntohs(tvb, offset) + offset + 2;
        offset += 2;

        while (offset < len)
        {
            guint8  p_len = tvb_get_guint8(tvb, offset + 1);


            ti = proto_tree_add_item(tree, hf_ircomm_param, tvb, offset, p_len + 2, FALSE);
            p_tree = proto_item_add_subtree(ti, ett_param[list_index * MAX_PARAMETERS + n]);

            buf[0] = 0;

            switch (tvb_get_guint8(tvb, offset))
            {
                case IRCOMM_SERVICE_TYPE:
                    proto_item_append_text(ti, ": Service Type (");

                    pv = tvb_get_guint8(tvb, offset+2);
                    if (pv & IRCOMM_3_WIRE_RAW)
                        strcat(buf, ", 3-Wire raw");
                    if (pv & IRCOMM_3_WIRE)
                        strcat(buf, ", 3-Wire");
                    if (pv & IRCOMM_9_WIRE)
                        strcat(buf, ", 9-Wire");
                    if (pv & IRCOMM_CENTRONICS)
                        strcat(buf, ", Centronics");

                    strcat(buf, ")");

                    proto_item_append_text(ti, buf+2);

                    break;

                case IRCOMM_PORT_TYPE:
                    proto_item_append_text(ti, ": Port Type (");

                    pv = tvb_get_guint8(tvb, offset+2);
                    if (pv & IRCOMM_SERIAL)
                        strcat(buf, ", serial");
                    if (pv & IRCOMM_PARALLEL)
                        strcat(buf, ", parallel");

                    strcat(buf, ")");

                    proto_item_append_text(ti, buf+2);

                    break;

                case IRCOMM_PORT_NAME:
                    proto_item_append_text(ti, ": Port Name (\"");

                    tvb_memcpy(tvb, buf, offset+2, p_len);
                    strcat(buf, "\")");

                    proto_item_append_text(ti, buf);

                    break;

                default:
                    proto_item_append_text(ti, ": unknown");
            }

            offset = dissect_param_tuple(tvb, p_tree, offset);
            n++;
        }

    }

    return TRUE;
}
