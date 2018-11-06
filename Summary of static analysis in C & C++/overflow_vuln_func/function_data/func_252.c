static guint
AddAttribute(packet_info *pinfo, tvbuff_t *tvb, proto_tree *tree, guint offset,
             guint16 function_id)
{
    proto_tree *attr_tree;
    proto_item *attr_item;
    guint32 tag,len;
    proto_item *len_item;

    attr_tree = proto_tree_add_subtree(tree, tvb, offset, -1,
            ett_isns_attribute, &attr_item, "Attribute");

    tag = tvb_get_ntohl(tvb, offset);
    proto_tree_add_item(attr_tree, hf_isns_attr_tag,
            tvb, offset, 4, ENC_BIG_ENDIAN);
    offset +=4;

    len = tvb_get_ntohl(tvb, offset);
    len_item = proto_tree_add_item(attr_tree, hf_isns_attr_len,
            tvb, offset, 4, ENC_BIG_ENDIAN);
    offset +=4;

    proto_item_set_len(attr_item, 8+len);
    proto_item_append_text(attr_item, ": %s", val_to_str_ext_const(tag, &isns_attribute_tags_ext, "Unknown"));

    /
    if (len==0) {
        if ((tag==ISNS_ATTR_TAG_PORTAL_GROUP_TAG) &&
                ((function_id==ISNS_FUNC_DEVATTRREG) || (function_id==ISNS_FUNC_RSP_DEVATTRREG))) {
            /
            proto_tree_add_uint_format_value(tree, hf_isns_portal_group_tag, tvb, offset, 8, 0, "<NULL>");
        }
        return offset;
    }

    switch( tag )
    {
        case ISNS_ATTR_TAG_DELIMITER:
            /
            break;
        case ISNS_ATTR_TAG_ENTITY_IDENTIFIER:
            proto_tree_add_item(attr_tree, hf_isns_entity_identifier, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_ENTITY_PROTOCOL:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_entity_protocol, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_MGMT_IP_ADDRESS:
            ISNS_REQUIRE_ATTR_LEN(16);
            proto_tree_add_item(attr_tree, hf_isns_mgmt_ip_addr, tvb, offset, len, ENC_NA);
            break;
        case ISNS_ATTR_TAG_TIMESTAMP:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_timestamp, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_REGISTRATION_PERIOD:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_registration_period, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ENTITY_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_entity_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ENTITY_NEXT_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_entity_next_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORTAL_IP_ADDRESS:
            proto_tree_add_item(attr_tree, hf_isns_portal_ip_addr, tvb, offset, len, ENC_NA);
            break;
        case ISNS_ATTR_TAG_PORTAL_PORT:
            dissect_isns_attr_port(tvb, offset, attr_tree, hf_isns_portal_port, ISNS_OTHER_PORT, pinfo);
            break;
        case ISNS_ATTR_TAG_PORTAL_SYMBOLIC_NAME:
            proto_tree_add_item(attr_tree, hf_isns_portal_symbolic_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_ESI_INTERVAL:
            proto_tree_add_item(attr_tree, hf_isns_esi_interval, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ESI_PORT:
            dissect_isns_attr_port(tvb, offset, attr_tree, hf_isns_esi_port, ISNS_ESI_PORT, pinfo);
            break;
        case ISNS_ATTR_TAG_PORTAL_INDEX:
            proto_tree_add_item(attr_tree, hf_isns_portal_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_SCN_PORT:
            dissect_isns_attr_port(tvb, offset, attr_tree, hf_isns_scn_port, ISNS_SCN_PORT, pinfo);
            break;
        case ISNS_ATTR_TAG_PORTAL_NEXT_INDEX:
            proto_tree_add_item(attr_tree, hf_isns_portal_next_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORTAL_SECURITY_BITMAP:
            dissect_isns_attr_portal_security_bitmap(tvb, offset, attr_tree);
            break;
        case ISNS_ATTR_TAG_ISCSI_NAME:
            proto_tree_add_item(attr_tree, hf_isns_iscsi_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_ISCSI_NODE_TYPE:
            ISNS_REQUIRE_ATTR_LEN(4);
            dissect_isns_attr_iscsi_node_type(tvb, offset, attr_tree);
            break;
        case ISNS_ATTR_TAG_ISCSI_ALIAS:
            proto_tree_add_item(attr_tree, hf_isns_iscsi_alias, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_ISCSI_SCN_BITMAP:
            ISNS_REQUIRE_ATTR_LEN(4);
            dissect_isns_attr_scn_bitmap(tvb, offset, attr_tree);
            break;
        case ISNS_ATTR_TAG_ISCSI_NODE_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_node_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_WWNN_TOKEN:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_wwnn_token, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ISCSI_NODE_NEXT_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_node_next_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ISCSI_AUTH_METHOD:
            proto_tree_add_item(attr_tree, hf_isns_iscsi_auth_method, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_PG_ISCSI_NAME:
            proto_tree_add_item(attr_tree, hf_isns_pg_iscsi_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_PG_PORTAL_IP_ADDR:
            proto_tree_add_item(attr_tree, hf_isns_pg_portal_ip_addr, tvb, offset, len, ENC_NA);
            break;
        case ISNS_ATTR_TAG_PG_PORTAL_PORT:
            ISNS_REQUIRE_ATTR_LEN(4);
            dissect_isns_attr_port(tvb, offset, attr_tree, hf_isns_pg_portal_port, ISNS_OTHER_PORT, pinfo);
            break;
        case ISNS_ATTR_TAG_PORTAL_GROUP_TAG:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_portal_group_tag, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORTAL_GROUP_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_pg_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORTAL_GROUP_NEXT_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_pg_next_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_FC_PORT_NAME_WWPN:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_fc_port_name_wwpn, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORT_ID:
            ISNS_REQUIRE_ATTR_LEN(3);
            proto_tree_add_item(attr_tree, hf_isns_port_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
            /
        case ISNS_ATTR_TAG_SYMBOLIC_PORT_NAME:
            proto_tree_add_item(attr_tree, hf_isns_symbolic_port_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_FABRIC_PORT_NAME:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_fabric_port_name, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_HARD_ADDRESS:
            ISNS_REQUIRE_ATTR_LEN(3);
            proto_tree_add_item(attr_tree, hf_isns_hard_address, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PORT_IP_ADDRESS:
            ISNS_REQUIRE_ATTR_LEN(16);
            proto_tree_add_item(attr_tree, hf_isns_port_ip_addr, tvb, offset, len, ENC_NA);
            break;
            /
        case ISNS_ATTR_TAG_FC4_DESCRIPTOR:
            proto_tree_add_item(attr_tree, hf_isns_fc4_descriptor, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
            /
        case ISNS_ATTR_TAG_PERMANENT_PORT_NAME:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_permanent_port_name, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
            /
        case ISNS_ATTR_TAG_FC_NODE_NAME_WWNN:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_fc_node_name_wwnn, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_SYMBOLIC_NODE_NAME:
            proto_tree_add_item(attr_tree, hf_isns_symbolic_node_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_NODE_IP_ADDRESS:
            ISNS_REQUIRE_ATTR_LEN(16);
            proto_tree_add_item(attr_tree, hf_isns_node_ip_addr, tvb, offset, len, ENC_NA);
            break;
        case ISNS_ATTR_TAG_NODE_IPA:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_node_ipa, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PROXY_ISCSI_NAME:
            proto_tree_add_item(attr_tree, hf_isns_proxy_iscsi_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_SWITCH_NAME:
            ISNS_REQUIRE_ATTR_LEN(8);
            proto_tree_add_item(attr_tree, hf_isns_switch_name, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_PREFERRED_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_preferred_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_ASSIGNED_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_assigned_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_VIRTUAL_FABRIC_ID:
            proto_tree_add_item(attr_tree, hf_isns_virtual_fabric_id, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_VENDOR_OUI:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_vendor_oui, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_SET_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_dd_set_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_SET_SYMBOLIC_NAME:
            proto_tree_add_item(attr_tree, hf_isns_dd_set_symbolic_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_DD_SET_NEXT_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_dd_set_next_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_dd_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_SYMBOLIC_NAME:
            proto_tree_add_item(attr_tree, hf_isns_dd_symbolic_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_ISCSI_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_member_iscsi_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_ISCSI_NAME:
            proto_tree_add_item(attr_tree, hf_isns_dd_member_iscsi_name, tvb, offset, len, ENC_ASCII|ENC_NA);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_FC_PORT_NAME:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_member_fc_port_name, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_PORTAL_INDEX:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_member_portal_index, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_PORTAL_IP_ADDRESS:
            ISNS_REQUIRE_ATTR_LEN(16);
            proto_tree_add_item(attr_tree, hf_isns_dd_member_portal_ip_addr, tvb, offset, len, ENC_NA);
            break;
        case ISNS_ATTR_TAG_DD_MEMBER_PORTAL_PORT:
            ISNS_REQUIRE_ATTR_LEN(4);
            dissect_isns_attr_port(tvb, offset, attr_tree, hf_isns_dd_member_portal_port, ISNS_OTHER_PORT, pinfo);
            break;
        case ISNS_ATTR_TAG_DD_ID_NEXT_ID:
            ISNS_REQUIRE_ATTR_LEN(4);
            proto_tree_add_item(attr_tree, hf_isns_dd_id_next_id, tvb, offset, len, ENC_BIG_ENDIAN);
            break;
        default:
            proto_tree_add_item(attr_tree, hf_isns_not_decoded_yet, tvb, offset, len, ENC_NA);
    }

    offset += len;
    return offset;
}
