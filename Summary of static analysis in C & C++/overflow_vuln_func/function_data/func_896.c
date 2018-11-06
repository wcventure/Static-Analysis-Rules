static void
dissect_amqp_0_10_dtx(tvbuff_t *tvb,
                      packet_info *pinfo,
                      proto_tree *tree,
                      int offset, guint16 length)
{
    proto_item  *args_tree;
    proto_item  *ti;
    proto_item  *flags_item;
    guint8       method;
    guint8       flag1, flag2;
    guint16      xid_length;
    int          flags_offset;
    const gchar *method_name;

    method = tvb_get_guint8(tvb, offset+1);
    method_name = val_to_str_const(method, amqp_0_10_dtx_methods,
                                   "<invalid dtx method>");
    col_append_str(pinfo->cinfo, COL_INFO, method_name);
    col_append_str(pinfo->cinfo, COL_INFO, " ");
    col_set_fence(pinfo->cinfo, COL_INFO);

    proto_tree_add_item(tree, hf_amqp_0_10_dtx_method,
                        tvb, offset+1, 1, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);
    /
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    ti = proto_tree_add_item(tree, hf_amqp_0_10_session_header, tvb, offset, 2, ENC_BIG_ENDIAN);
    if ((flag1 != 1) || ((flag2 & 0xfe) != 0))
        proto_item_append_text(ti, " (Invalid)");
    else
        proto_tree_add_item(tree, hf_amqp_0_10_session_header_sync,
                            tvb, offset + 1, 1, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);

    /
    if ((method == AMQP_0_10_METHOD_DTX_SELECT) ||
        (method == AMQP_0_10_METHOD_DTX_RECOVER))
        return;

    ti = proto_tree_add_item(tree, hf_amqp_method_arguments,
                             tvb, offset, length - 4, ENC_NA);
    args_tree = proto_item_add_subtree(ti, ett_args);

    /
    flags_offset = offset;
    flag1 = tvb_get_guint8(tvb, offset);
    flag2 = tvb_get_guint8(tvb, offset+1);
    flags_item = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_argument_packing_flags,
                                     tvb, offset, 2, ENC_BIG_ENDIAN);
    AMQP_INCREMENT(offset, 2, length);
    switch (method) {
    case AMQP_0_10_METHOD_DTX_START:
        if ((flag1 & ~0x07) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            xid_length = tvb_get_ntohs(tvb, offset);
            AMQP_INCREMENT(offset, 2, length);
            THROW_ON(((offset + xid_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_dtx_xid,
                                     tvb,
                                     offset - 2,
                                     xid_length + 2, ENC_NA);
            dissect_amqp_0_10_xid (tvb,
                                   offset,
                                   xid_length,
                                   ti);
            AMQP_INCREMENT(offset, xid_length, length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_dtx_start_join,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_dtx_start_resume,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);

        break;

    case AMQP_0_10_METHOD_DTX_END:
        if ((flag1 & ~0x07) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            xid_length = tvb_get_ntohs(tvb, offset);
            AMQP_INCREMENT(offset, 2, length);
            THROW_ON(((offset + xid_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_dtx_xid,
                                     tvb,
                                     offset - 2,
                                     xid_length + 2, ENC_NA);
            dissect_amqp_0_10_xid (tvb,
                                   offset,
                                   xid_length,
                                   ti);
            AMQP_INCREMENT(offset, xid_length, length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_dtx_end_fail,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_dtx_end_suspend,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        break;

    case AMQP_0_10_METHOD_DTX_COMMIT:
        if ((flag1 & ~0x03) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            xid_length = tvb_get_ntohs(tvb, offset);
            AMQP_INCREMENT(offset, 2, length);
            THROW_ON(((offset + xid_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_dtx_xid,
                                     tvb,
                                     offset - 2,
                                     xid_length + 2, ENC_NA);
            dissect_amqp_0_10_xid (tvb,
                                   offset,
                                   xid_length,
                                   ti);
            AMQP_INCREMENT(offset, xid_length, length);
        }
        /
        proto_tree_add_item(args_tree,
                            hf_amqp_0_10_method_dtx_commit_one_phase,
                            tvb, flags_offset, 1, ENC_BIG_ENDIAN);
        break;

    case AMQP_0_10_METHOD_DTX_FORGET:
    case AMQP_0_10_METHOD_DTX_GET_TIMEOUT:
    case AMQP_0_10_METHOD_DTX_PREPARE:
    case AMQP_0_10_METHOD_DTX_ROLLBACK:
        if ((flag1 & ~0x01) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            xid_length = tvb_get_ntohs(tvb, offset);
            AMQP_INCREMENT(offset, 2, length);
            THROW_ON(((offset + xid_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_dtx_xid,
                                     tvb,
                                     offset - 2,
                                     xid_length + 2, ENC_NA);
            dissect_amqp_0_10_xid (tvb,
                                   offset,
                                   xid_length,
                                   ti);
            AMQP_INCREMENT(offset, xid_length, length);
        }
        break;

    case AMQP_0_10_METHOD_DTX_SET_TIMEOUT:
        if ((flag1 & ~0x03) || (flag2 != 0))
            expert_add_info(pinfo, flags_item, &ei_amqp_bad_flag_value);
        if (flag1 & 0x01) {     /
            xid_length = tvb_get_ntohs(tvb, offset);
            AMQP_INCREMENT(offset, 2, length);
            THROW_ON(((offset + xid_length) > length), ReportedBoundsError);
            ti = proto_tree_add_item(args_tree,
                                     hf_amqp_0_10_dtx_xid,
                                     tvb,
                                     offset - 2,
                                     xid_length + 2, ENC_NA);
            dissect_amqp_0_10_xid (tvb,
                                   offset,
                                   xid_length,
                                   ti);
            AMQP_INCREMENT(offset, xid_length, length);
        }
        if (flag1 & 0x02) {    /
            proto_tree_add_item(args_tree,
                                hf_amqp_0_10_method_dtx_set_timeout_timeout,
                                tvb, offset, 4, ENC_BIG_ENDIAN);
            AMQP_INCREMENT(offset, 4, length);
        }
        break;

    }
}
