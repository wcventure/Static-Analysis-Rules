static int
dissect_openflow_table_feature_prop_v4(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, int offset, guint16 length)
{
    proto_item *ti;
    proto_tree *prop_tree, *elem_tree;
    guint16 prop_type;
    guint16 prop_length;
    guint16 elem_begin;
    guint16 body_end;
    guint16 pad_length;

    ti = proto_tree_add_text(tree, tvb, offset, -1, "Table feature property");
    prop_tree = proto_item_add_subtree(ti, ett_openflow_v4_table_feature_prop);

    /
    prop_type = tvb_get_ntohs(tvb, offset);
    proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_type, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset+=2;

    /
    prop_length = tvb_get_ntohs(tvb, offset);
    proto_item_set_len(ti, prop_length);
    proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_length, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset+=2;

    if (prop_length < 4)
        return offset;

    body_end = offset + prop_length - 4;

    /
    switch (prop_type) {
    case OFPTFPT_INSTRUCTIONS:
    case OFPTFPT_INSTRUCTIONS_MISS:
        while (offset < body_end) {
            elem_begin = offset;
            ti = proto_tree_add_text(prop_tree, tvb, offset, -1, "Instruction ID");
            elem_tree = proto_item_add_subtree(ti, ett_openflow_v4_table_feature_prop_instruction_id);

            offset = dissect_openflow_instruction_header_v4(tvb, pinfo, elem_tree, offset, length);
            proto_item_set_len(ti, offset - elem_begin);
        }
        break;

    case OFPTFPT_NEXT_TABLES:
    case OFPTFPT_NEXT_TABLES_MISS:
        while (offset < body_end) {
            proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_next_tables_next_table_id, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset+=1;
        }
        break;

    case OFPTFPT_WRITE_ACTIONS:
    case OFPTFPT_WRITE_ACTIONS_MISS:
    case OFPTFPT_APPLY_ACTIONS:
    case OFPTFPT_APPLY_ACTIONS_MISS:
        while (offset < body_end) {
            elem_begin = offset;
            ti = proto_tree_add_text(prop_tree, tvb, offset, -1, "Action ID");
            elem_tree = proto_item_add_subtree(ti, ett_openflow_v4_table_feature_prop_action_id);

            offset = dissect_openflow_action_header_v4(tvb, pinfo, elem_tree, offset, length);
            proto_item_set_len(ti, offset - elem_begin);
        }
        break;

    case OFPTFPT_MATCH:
    case OFPTFPT_WILDCARDS:
    case OFPTFPT_WRITE_SETFIELD:
    case OFPTFPT_WRITE_SETFIELD_MISS:
    case OFPTFPT_APPLY_SETFIELD:
    case OFPTFPT_APPLY_SETFIELD_MISS:
        while (offset < body_end) {
            elem_begin = offset;
            ti = proto_tree_add_text(prop_tree, tvb, offset, -1, "OXM ID");
            elem_tree = proto_item_add_subtree(ti, ett_openflow_v4_table_feature_prop_oxm_id);

            offset = dissect_openflow_oxm_header_v4(tvb, pinfo, elem_tree, offset, length);
            proto_item_set_len(ti, offset - elem_begin);
        }
        break;

    case OFPTFPT_EXPERIMENTER:
    case OFPTFPT_EXPERIMENTER_MISS:
        /
        proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_experimenter_experimenter, tvb, offset, 4, ENC_BIG_ENDIAN);
        offset+=4;

        /
        proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_experimenter_exp_type, tvb, offset, 4, ENC_BIG_ENDIAN);
        offset+=4;

        /
        proto_tree_add_expert_format(prop_tree, pinfo, &ei_openflow_v4_table_feature_prop_undecoded,
                                     tvb, offset, body_end - offset, "Experimenter table property body.");
        offset = body_end;
        break;

    default:
        proto_tree_add_expert_format(prop_tree, pinfo, &ei_openflow_v4_table_feature_prop_undecoded,
                                     tvb, offset, body_end - offset, "Unknown table property body.");
        offset = body_end;
        break;
    };

    pad_length = (prop_length + 7)/8*8 - prop_length;
    if (pad_length > 0) {
        proto_tree_add_item(prop_tree, hf_openflow_v4_table_feature_prop_pad, tvb, offset, pad_length, ENC_NA);
        offset+=pad_length;
    }

    return offset;
}
