static gint
dissect_bthci_acl(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    proto_item               *bthci_acl_itam;
    proto_tree               *bthci_acl_tree;
    proto_item               *sub_item;
    proto_item               *length_item;
    guint16                   flags;
    guint16                   length;
    gboolean                  fragmented;
    gint                      offset                = 0;
    guint16                   pb_flag, l2cap_length = 0;
    tvbuff_t                 *next_tvb;
    bthci_acl_data_t         *acl_data;
    chandle_data_t           *chandle_data;
    bluetooth_data_t         *bluetooth_data;
    wmem_tree_t              *subtree;
    wmem_tree_key_t           key[6];
    guint32                   interface_id;
    guint32                   adapter_id;
    guint32                   connection_handle;
    guint32                   direction;
    guint32                   frame_number;
    remote_bdaddr_t          *remote_bdaddr;
    const gchar              *localhost_name;
    guint8                    localhost_bdaddr[6];
    const gchar              *localhost_ether_addr;
    gchar                    *localhost_addr_name;
    gint                      localhost_length;
    localhost_bdaddr_entry_t *localhost_bdaddr_entry;
    localhost_name_entry_t   *localhost_name_entry;
    const guint8              unknown_bd_addr[6] = {0, 0, 0, 0, 0, 0};
    const guint8             *src_bd_addr = &unknown_bd_addr[0];
    const gchar              *src_name = "";
    const gchar              *src_addr_name = "";
    const guint8             *dst_bd_addr = &unknown_bd_addr[0];
    const gchar              *dst_name = "";
    const gchar              *dst_addr_name = "";
    chandle_session_t        *chandle_session;
    guint32                   src_role = ROLE_UNKNOWN;
    guint32                   dst_role = ROLE_UNKNOWN;
    guint32                   role_last_change_in_frame = 0;
    connection_mode_t        *connection_mode;
    gint32                    mode = -1;
    guint32                   mode_last_change_in_frame = 0;

    /
    if (data == NULL)
        return 0;
    bluetooth_data = (bluetooth_data_t *) data;

    bthci_acl_itam = proto_tree_add_item(tree, proto_bthci_acl, tvb, offset, -1, ENC_NA);
    bthci_acl_tree = proto_item_add_subtree(bthci_acl_itam, ett_bthci_acl);

    switch (pinfo->p2p_dir) {
        case P2P_DIR_SENT:
            col_set_str(pinfo->cinfo, COL_INFO, "Sent ");
            break;
        case P2P_DIR_RECV:
            col_set_str(pinfo->cinfo, COL_INFO, "Rcvd ");
            break;
        default:
        col_set_str(pinfo->cinfo, COL_INFO, "UnknownDirection ");
            break;
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "HCI_ACL");

    flags   = tvb_get_letohs(tvb, offset);
    pb_flag = (flags & 0x3000) >> 12;
    proto_tree_add_item(bthci_acl_tree, hf_bthci_acl_chandle, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(bthci_acl_tree, hf_bthci_acl_pb_flag, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(bthci_acl_tree, hf_bthci_acl_bc_flag, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    offset += 2;

    interface_id      = bluetooth_data->interface_id;
    adapter_id        = bluetooth_data->adapter_id;
    connection_handle = flags & 0x0fff;
    direction         = pinfo->p2p_dir;
    frame_number      = pinfo->num;

    acl_data = wmem_new(wmem_packet_scope(), bthci_acl_data_t);
    acl_data->interface_id                = interface_id;
    acl_data->adapter_id                  = adapter_id;
    acl_data->adapter_disconnect_in_frame = bluetooth_data->adapter_disconnect_in_frame;
    acl_data->chandle                     = connection_handle;
    acl_data->is_btle                     = FALSE;

    key[0].length = 1;
    key[0].key    = &interface_id;
    key[1].length = 1;
    key[1].key    = &adapter_id;
    key[2].length = 1;
    key[2].key    = &connection_handle;
    key[3].length = 0;
    key[3].key    = NULL;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->chandle_sessions, key);
    chandle_session = (subtree) ? (chandle_session_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (chandle_session &&
            chandle_session->connect_in_frame < pinfo->num &&
            chandle_session->disconnect_in_frame > pinfo->num) {
        acl_data->disconnect_in_frame = &chandle_session->disconnect_in_frame;
    } else {
        acl_data->disconnect_in_frame = &invalid_session;
        chandle_session = NULL;
    }

    acl_data->remote_bd_addr_oui         = 0;
    acl_data->remote_bd_addr_id          = 0;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->chandle_to_mode, key);
    connection_mode = (subtree) ? (connection_mode_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (connection_mode) {
        mode = connection_mode->mode;
        mode_last_change_in_frame = connection_mode->change_in_frame;
    }

    /
    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->chandle_to_bdaddr, key);
    remote_bdaddr = (subtree) ? (remote_bdaddr_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (remote_bdaddr) {
        guint32         k_bd_addr_oui;
        guint32         k_bd_addr_id;
        guint32         bd_addr_oui;
        guint32         bd_addr_id;
        device_name_t  *device_name;
        device_role_t  *device_role;
        const gchar    *remote_name;
        const gchar    *remote_ether_addr;
        gchar          *remote_addr_name;
        gint            remote_length;

        bd_addr_oui = remote_bdaddr->bd_addr[0] << 16 | remote_bdaddr->bd_addr[1] << 8 | remote_bdaddr->bd_addr[2];
        bd_addr_id  = remote_bdaddr->bd_addr[3] << 16 | remote_bdaddr->bd_addr[4] << 8 | remote_bdaddr->bd_addr[5];

        acl_data->remote_bd_addr_oui = bd_addr_oui;
        acl_data->remote_bd_addr_id  = bd_addr_id;

        k_bd_addr_oui  = bd_addr_oui;
        k_bd_addr_id   = bd_addr_id;

        key[0].length = 1;
        key[0].key    = &interface_id;
        key[1].length = 1;
        key[1].key    = &adapter_id;
        key[2].length = 1;
        key[2].key    = &k_bd_addr_id;
        key[3].length = 1;
        key[3].key    = &k_bd_addr_oui;
        key[4].length = 0;
        key[4].key    = NULL;

        subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->bdaddr_to_role, key);
        device_role = (subtree) ? (device_role_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
        if (device_role) {
            if ((pinfo->p2p_dir == P2P_DIR_SENT && device_role->role == ROLE_MASTER) ||
                    (pinfo->p2p_dir == P2P_DIR_RECV && device_role->role == ROLE_SLAVE)) {
                src_role = ROLE_SLAVE;
                dst_role = ROLE_MASTER;
            } else if ((pinfo->p2p_dir == P2P_DIR_SENT && device_role->role == ROLE_SLAVE) ||
                    (pinfo->p2p_dir == P2P_DIR_RECV && device_role->role == ROLE_MASTER)) {
                src_role = ROLE_MASTER;
                dst_role = ROLE_SLAVE;
            }
            role_last_change_in_frame = device_role->change_in_frame;
        }

        subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->bdaddr_to_name, key);
        device_name = (subtree) ? (device_name_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
        if (device_name)
            remote_name = device_name->name;
        else
            remote_name = "";

        remote_ether_addr = get_ether_name(remote_bdaddr->bd_addr);
        remote_length = (gint)(strlen(remote_ether_addr) + 3 + strlen(remote_name) + 1);
        remote_addr_name = (gchar *)wmem_alloc(pinfo->pool, remote_length);

        g_snprintf(remote_addr_name, remote_length, "%s (%s)", remote_ether_addr, remote_name);

        if (pinfo->p2p_dir == P2P_DIR_RECV) {
            src_bd_addr   = remote_bdaddr->bd_addr;
            src_name      = remote_name;
            src_addr_name = remote_addr_name;
        } else if (pinfo->p2p_dir == P2P_DIR_SENT) {
            dst_bd_addr   = remote_bdaddr->bd_addr;
            dst_name      = remote_name;
            dst_addr_name = remote_addr_name;
        }
    } else {
        if (pinfo->p2p_dir == P2P_DIR_RECV)
            src_addr_name = "remote ()";
        else if (pinfo->p2p_dir == P2P_DIR_SENT)
            dst_addr_name = "remote ()";
    }

    /
    key[0].length = 1;
    key[0].key    = &interface_id;
    key[1].length = 1;
    key[1].key    = &adapter_id;
    key[2].length = 0;
    key[2].key    = NULL;


    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->localhost_bdaddr, key);
    localhost_bdaddr_entry = (subtree) ? (localhost_bdaddr_entry_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (localhost_bdaddr_entry) {
        localhost_ether_addr = get_ether_name(localhost_bdaddr_entry->bd_addr);
        memcpy(localhost_bdaddr, localhost_bdaddr_entry->bd_addr, 6);
    } else {
        localhost_ether_addr = "localhost";
        memcpy(localhost_bdaddr, unknown_bd_addr, 6);
    }

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(bluetooth_data->localhost_name, key);
    localhost_name_entry = (subtree) ? (localhost_name_entry_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (localhost_name_entry)
        localhost_name = localhost_name_entry->name;
    else
        localhost_name = "";

    localhost_length = (gint)(strlen(localhost_ether_addr) + 3 + strlen(localhost_name) + 1);
    localhost_addr_name = (gchar *)wmem_alloc(pinfo->pool, localhost_length);

    g_snprintf(localhost_addr_name, localhost_length, "%s (%s)", localhost_ether_addr, localhost_name);

    if (pinfo->p2p_dir == P2P_DIR_RECV) {
        dst_bd_addr   = localhost_bdaddr;
        dst_name      = localhost_name;
        dst_addr_name = localhost_addr_name;
    } else if (pinfo->p2p_dir == P2P_DIR_SENT) {
        src_bd_addr   = localhost_bdaddr;
        src_name      = localhost_name;
        src_addr_name = localhost_addr_name;
    }

    /
    key[0].length = 1;
    key[0].key = &interface_id;
    key[1].length = 1;
    key[1].key = &adapter_id;
    key[2].length = 1;
    key[2].key = &connection_handle;
    key[3].length = 1;
    key[3].key = &direction;
    key[4].length = 0;
    key[4].key = NULL;

    subtree = (wmem_tree_t *) wmem_tree_lookup32_array(chandle_tree, key);
    chandle_data = (subtree) ? (chandle_data_t *) wmem_tree_lookup32_le(subtree, pinfo->num) : NULL;
    if (!pinfo->fd->flags.visited && !chandle_data) {
        key[0].length = 1;
        key[0].key = &interface_id;
        key[1].length = 1;
        key[1].key = &adapter_id;
        key[2].length = 1;
        key[2].key = &connection_handle;
        key[3].length = 1;
        key[3].key = &direction;
        key[4].length = 1;
        key[4].key = &frame_number;
        key[5].length = 0;
        key[5].key = NULL;

        chandle_data = (chandle_data_t *)wmem_alloc(wmem_file_scope(), sizeof(chandle_data_t));
        chandle_data->start_fragments = wmem_tree_new(wmem_file_scope());

        wmem_tree_insert32_array(chandle_tree, key, chandle_data);
    } else if (pinfo->fd->flags.visited && !chandle_data) {
        DISSECTOR_ASSERT_HINT(0, "Impossible: no previously session saved");
    }

    length = tvb_get_letohs(tvb, offset);
    length_item = proto_tree_add_item(bthci_acl_tree, hf_bthci_acl_length, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    offset += 2;

    /
    switch(pb_flag) {
    case 0x01:  /
        fragmented = TRUE;
        break;
    case 0x00:  /
    case 0x02:  /
        l2cap_length = tvb_get_letohs(tvb, offset);
        fragmented   = (l2cap_length + 4 != length);
        break;
    default:
        /
        fragmented = FALSE;
    }


    if (!fragmented || (!acl_reassembly && !(pb_flag & 0x01))) {
        /
        if (length < tvb_captured_length_remaining(tvb, offset)) {
            expert_add_info(pinfo, length_item, &ei_length_bad);
            /
            length = tvb_captured_length_remaining(tvb, offset);
        }

        next_tvb = tvb_new_subset(tvb, offset, tvb_captured_length_remaining(tvb, offset), length);
        call_dissector_with_data(btl2cap_handle, next_tvb, pinfo, tree, acl_data);
    } else if (fragmented && acl_reassembly) {
        multi_fragment_pdu_t *mfp = NULL;
        gint                  len;

        if (!(pb_flag & 0x01)) { /
            if (!pinfo->fd->flags.visited) {
                mfp = (multi_fragment_pdu_t *) wmem_new(wmem_file_scope(), multi_fragment_pdu_t);
                mfp->first_frame = pinfo->num;
                mfp->last_frame  = 0;
                mfp->tot_len     = l2cap_length + 4;
                mfp->reassembled = (char *) wmem_alloc(wmem_file_scope(), mfp->tot_len);
                len = tvb_captured_length_remaining(tvb, offset);
                if (len <= mfp->tot_len) {
                    tvb_memcpy(tvb, (guint8 *) mfp->reassembled, offset, len);
                    mfp->cur_off = len;
                    wmem_tree_insert32(chandle_data->start_fragments, pinfo->num, mfp);
                }
            } else {
                mfp = (multi_fragment_pdu_t *)wmem_tree_lookup32(chandle_data->start_fragments, pinfo->num);
            }
            if (mfp != NULL && mfp->last_frame) {
                proto_item *item;

                item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_reassembled_in, tvb, 0, 0, mfp->last_frame);
                PROTO_ITEM_SET_GENERATED(item);
                col_append_fstr(pinfo->cinfo, COL_INFO, " [Reassembled in #%u]", mfp->last_frame);
            }
        }
        if (pb_flag == 0x01) { /
            mfp = (multi_fragment_pdu_t *)wmem_tree_lookup32_le(chandle_data->start_fragments, pinfo->num);
            if (!pinfo->fd->flags.visited) {
                len = tvb_captured_length_remaining(tvb, offset);
                if (mfp != NULL && !mfp->last_frame && (mfp->tot_len >= mfp->cur_off + len)) {
                    tvb_memcpy(tvb, (guint8 *) mfp->reassembled + mfp->cur_off, offset, len);
                    mfp->cur_off += len;
                    if (mfp->cur_off == mfp->tot_len) {
                        mfp->last_frame = pinfo->num;
                    }
                }
            }
            if (mfp) {
                proto_item *item;

                item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_continuation_to, tvb, 0, 0, mfp->first_frame);
                PROTO_ITEM_SET_GENERATED(item);
                col_append_fstr(pinfo->cinfo, COL_INFO, " [Continuation to #%u]", mfp->first_frame);
                if (mfp->last_frame && mfp->last_frame != pinfo->num) {
                    item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_reassembled_in, tvb, 0, 0, mfp->last_frame);
                    PROTO_ITEM_SET_GENERATED(item);
                    col_append_fstr(pinfo->cinfo, COL_INFO, " [Reassembled in #%u]", mfp->last_frame);
                }
            }
            if (mfp != NULL && mfp->last_frame == pinfo->num) {
                next_tvb = tvb_new_child_real_data(tvb, (guint8 *) mfp->reassembled, mfp->tot_len, mfp->tot_len);
                add_new_data_source(pinfo, next_tvb, "Reassembled BTHCI ACL");

                call_dissector_with_data(btl2cap_handle, next_tvb, pinfo, tree, acl_data);
            }
        }
    }

    if (tvb_captured_length_remaining(tvb, offset) > 0) {
        sub_item = proto_tree_add_item(bthci_acl_tree, hf_bthci_acl_data, tvb, offset, -1, ENC_NA);
        if (fragmented) {
            proto_item_append_text(sub_item, " Fragment");
        }
    }

    if (chandle_session) {
        sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_connect_in, tvb, 0, 0, chandle_session->connect_in_frame);
        PROTO_ITEM_SET_GENERATED(sub_item);

        if (chandle_session->disconnect_in_frame < G_MAXUINT32) {
            sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_disconnect_in, tvb, 0, 0, chandle_session->disconnect_in_frame);
            PROTO_ITEM_SET_GENERATED(sub_item);
        }
    }

    if (acl_data->disconnect_in_frame == &invalid_session) {
        expert_add_info(pinfo, bthci_acl_itam, &ei_invalid_session);
    }

    set_address(&pinfo->net_src, AT_STRINGZ, (int)strlen(src_name) + 1, src_name);
    set_address(&pinfo->dl_src, AT_ETHER, 6, src_bd_addr);
    set_address(&pinfo->src, AT_STRINGZ, (int)strlen(src_addr_name) + 1, src_addr_name);

    set_address(&pinfo->net_dst, AT_STRINGZ, (int)strlen(dst_name) + 1, dst_name);
    set_address(&pinfo->dl_dst, AT_ETHER, 6, dst_bd_addr);
    set_address(&pinfo->dst, AT_STRINGZ, (int)strlen(dst_addr_name) + 1, dst_addr_name);

    if (!pinfo->fd->flags.visited) {
        address *addr;

        addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_src, sizeof(address));
        addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_src.data, pinfo->dl_src.len);
        p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_SRC, addr);

        addr = (address *) wmem_memdup(wmem_file_scope(), &pinfo->dl_dst, sizeof(address));
        addr->data =  wmem_memdup(wmem_file_scope(), pinfo->dl_dst.data, pinfo->dl_dst.len);
        p_add_proto_data(wmem_file_scope(), pinfo, proto_bluetooth, BLUETOOTH_DATA_DST, addr);
    }

    sub_item = proto_tree_add_ether(bthci_acl_tree, hf_bthci_acl_src_bd_addr, tvb, 0, 0, src_bd_addr);
    PROTO_ITEM_SET_GENERATED(sub_item);

    sub_item = proto_tree_add_string(bthci_acl_tree, hf_bthci_acl_src_name, tvb, 0, 0, src_name);
    PROTO_ITEM_SET_GENERATED(sub_item);

    sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_src_role, tvb, 0, 0, src_role);
    PROTO_ITEM_SET_GENERATED(sub_item);

    sub_item = proto_tree_add_ether(bthci_acl_tree, hf_bthci_acl_dst_bd_addr, tvb, 0, 0, dst_bd_addr);
    PROTO_ITEM_SET_GENERATED(sub_item);

    sub_item = proto_tree_add_string(bthci_acl_tree, hf_bthci_acl_dst_name, tvb, 0, 0, dst_name);
    PROTO_ITEM_SET_GENERATED(sub_item);

    sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_dst_role, tvb, 0, 0, dst_role);
    PROTO_ITEM_SET_GENERATED(sub_item);

    if (role_last_change_in_frame > 0) {
        sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_role_last_change_in_frame, tvb, 0, 0, role_last_change_in_frame);
        PROTO_ITEM_SET_GENERATED(sub_item);
    }

    sub_item = proto_tree_add_int(bthci_acl_tree, hf_bthci_acl_mode, tvb, 0, 0, mode);
    PROTO_ITEM_SET_GENERATED(sub_item);

    if (mode_last_change_in_frame > 0) {
        sub_item = proto_tree_add_uint(bthci_acl_tree, hf_bthci_acl_mode_last_change_in_frame, tvb, 0, 0, mode_last_change_in_frame);
        PROTO_ITEM_SET_GENERATED(sub_item);
    }

    return tvb_captured_length(tvb);
}
