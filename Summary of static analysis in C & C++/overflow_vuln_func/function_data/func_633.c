static gint
reassemble_continuation_state(tvbuff_t *tvb, packet_info *pinfo,
        gint offset, guint tid, gboolean is_request,
        gint attribute_list_byte_offset, gint attribute_list_byte_count,
        guint32 pdu_type, tvbuff_t **new_tvb, gboolean *is_first,
        gboolean *is_continued, wmem_array_t **uuid_array,
        guint32 *record_handle, btl2cap_data_t *l2cap_data)
{
    guint              length;
    tid_request_t     *tid_request;
    continuation_state_data_t *continuation_state_data;
    wmem_tree_key_t    key[12];
    guint32            k_interface_id;
    guint32            k_adapter_id;
    guint32            k_chandle;
    guint32            k_psm;
    guint32            k_tid;
    guint32            k_pdu_type;
    guint32            k_frame_number;
    guint8             *k_continuation_state;
    guint32            interface_id;
    guint32            adapter_id;
    guint32            chandle;
    guint32            psm;
    guint32            frame_number;
    guint32           *continuation_state_array;

    if (new_tvb) *new_tvb = NULL;

    interface_id = l2cap_data->interface_id;
    adapter_id   = l2cap_data->adapter_id;
    chandle      = l2cap_data->chandle;
    psm          = l2cap_data->psm;
    frame_number = pinfo->fd->num;

    k_interface_id = interface_id;
    k_adapter_id   = adapter_id;
    k_chandle      = chandle;
    k_psm          = psm;
    k_tid          = tid;
    k_frame_number = frame_number;

    key[0].length = 1;
    key[0].key    = &k_interface_id;
    key[1].length = 1;
    key[1].key    = &k_adapter_id;
    key[2].length = 1;
    key[2].key    = &k_chandle;
    key[3].length = 1;
    key[3].key    = &k_psm;
    key[4].length = 1;
    key[4].key    = &k_tid;
    key[5].length = 1;
    key[5].key    = &k_frame_number;
    key[6].length = 0;
    key[6].key    = NULL;

    if (is_first) *is_first = TRUE;
    if (is_continued) *is_continued = TRUE;

    length = tvb_length_remaining(tvb, offset);
    if (length == 0)  {
        return offset;
    } else if (length > 17) {
        return offset;
    } else if (length == 1 && tvb_get_guint8(tvb, offset) == 0x00) {
        if (is_continued) *is_continued = FALSE;

        if (!pinfo->fd->flags.visited) {
            if (is_request) {
                tid_request = (tid_request_t *) wmem_new(wmem_file_scope(), tid_request_t);
                tid_request->interface_id = interface_id;
                tid_request->adapter_id   = adapter_id;
                tid_request->chandle      = chandle;
                tid_request->psm          = psm;
                tid_request->tid          = tid;

                if (uuid_array)
                    tid_request->uuid_array = *uuid_array;
                else
                    tid_request->uuid_array = NULL;
                if (record_handle)
                    tid_request->record_handle   = *record_handle;
                else
                    tid_request->record_handle = 0;

                tid_request->data         = NULL;
                tid_request->data_length  = 0;

                tid_request->pdu_type = pdu_type;

                tid_request->continuation_state        = NULL;
                tid_request->continuation_state_length = 0;

                wmem_tree_insert32_array(tid_requests, key, tid_request);
            } else {
                tid_request = (tid_request_t *) wmem_tree_lookup32_array_le(tid_requests, key);
                if (tid_request && tid_request->interface_id == interface_id &&
                        tid_request->adapter_id == adapter_id &&
                        tid_request->chandle == chandle &&
                        tid_request->psm == psm &&
                        tid_request->tid == tid) {
                    if (tid_request->continuation_state_length > 0) {
                        /

                        k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                        k_continuation_state[0] = tid_request->continuation_state_length;
                        memcpy(&k_continuation_state[1], tid_request->continuation_state, tid_request->continuation_state_length);
                        continuation_state_array = (guint32 *) k_continuation_state;

                        k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                        k_continuation_state[0] = tid_request->continuation_state_length;
                        memcpy(&k_continuation_state[1], tid_request->continuation_state, tid_request->continuation_state_length);

                        k_interface_id       = interface_id;
                        k_adapter_id         = adapter_id;
                        k_chandle            = chandle;
                        k_psm                = psm;
                        k_pdu_type           = tid_request->pdu_type;
                        k_frame_number       = frame_number;

                        key[0].length = 1;
                        key[0].key    = &k_interface_id;
                        key[1].length = 1;
                        key[1].key    = &k_adapter_id;
                        key[2].length = 1;
                        key[2].key    = &k_chandle;
                        key[3].length = 1;
                        key[3].key    = &k_psm;
                        key[4].length = 1;
                        key[4].key    = &k_pdu_type;
                        key[5].length = 1;
                        key[5].key    = (guint32 *) &k_continuation_state[0];
                        key[6].length = 1;
                        key[6].key    = (guint32 *) &k_continuation_state[4];
                        key[7].length = 1;
                        key[7].key    = (guint32 *) &k_continuation_state[8];
                        key[8].length = 1;
                        key[8].key    = (guint32 *) &k_continuation_state[12];
                        key[9].length = 1;
                        key[9].key    = (guint32 *) &k_continuation_state[16];
                        key[10].length = 1;
                        key[10].key    = &k_frame_number;
                        key[11].length = 0;
                        key[11].key    = NULL;

                        continuation_state_data = (continuation_state_data_t *) wmem_tree_lookup32_array_le(continuation_states, key);
                        if (continuation_state_data && continuation_state_data->interface_id == interface_id &&
                                continuation_state_data->adapter_id == adapter_id &&
                                continuation_state_data->chandle == chandle &&
                                continuation_state_data->psm == psm &&
                                continuation_state_data->pdu_type == tid_request->pdu_type &&
                                continuation_state_data->continuation_state[0] == continuation_state_array[0] &&
                                continuation_state_data->continuation_state[1] == continuation_state_array[1] &&
                                continuation_state_data->continuation_state[2] == continuation_state_array[2] &&
                                continuation_state_data->continuation_state[3] == continuation_state_array[3] &&
                                continuation_state_data->continuation_state[4] == continuation_state_array[4]) {
                            tid_request->data = (guint8 *) wmem_alloc(wmem_file_scope(), continuation_state_data->data_length + attribute_list_byte_count);
                            tid_request->data_length = continuation_state_data->data_length + attribute_list_byte_count;
                            memcpy(tid_request->data, continuation_state_data->data, continuation_state_data->data_length);
                            tvb_memcpy(tvb, tid_request->data + continuation_state_data->data_length, attribute_list_byte_offset, attribute_list_byte_count);
                        }
                    } else {
                        tid_request->data        = (guint8 *) wmem_alloc(wmem_file_scope(), attribute_list_byte_count);
                        tid_request->data_length = attribute_list_byte_count;

                        tvb_memcpy(tvb, tid_request->data, attribute_list_byte_offset, attribute_list_byte_count);
                    }

                    if (uuid_array) *uuid_array = tid_request->uuid_array;
                    if (record_handle) *record_handle = tid_request->record_handle;
                }
            }

            k_interface_id = interface_id;
            k_adapter_id   = adapter_id;
            k_chandle      = chandle;
            k_psm          = psm;
            k_tid          = tid;
            k_frame_number = frame_number;

            key[0].length = 1;
            key[0].key    = &k_interface_id;
            key[1].length = 1;
            key[1].key    = &k_adapter_id;
            key[2].length = 1;
            key[2].key    = &k_chandle;
            key[3].length = 1;
            key[3].key    = &k_psm;
            key[4].length = 1;
            key[4].key    = &k_tid;
            key[5].length = 1;
            key[5].key    = &k_frame_number;
            key[6].length = 0;
            key[6].key    = NULL;
        }

        /
        if (!is_request) {
            tid_request = (tid_request_t *) wmem_tree_lookup32_array_le(tid_requests, key);
            if (tid_request && tid_request->interface_id == interface_id &&
                    tid_request->adapter_id == adapter_id &&
                    tid_request->chandle == chandle &&
                    tid_request->psm == psm &&
                    tid_request->tid == tid) {
                tvbuff_t *next_tvb;

                next_tvb = tvb_new_child_real_data(tvb, tid_request->data,
                        tid_request->data_length, tid_request->data_length);

                if (new_tvb) *new_tvb = next_tvb;
                if (tid_request->continuation_state_length && is_first) *is_first = FALSE;

                if (uuid_array) *uuid_array = tid_request->uuid_array;
                if (record_handle) *record_handle = tid_request->record_handle;
            }
        }
    } else {
        guint8      *continuation_state;
        guint8       continuation_state_length;
        guint8      *packet_scope_string;

        continuation_state_length = tvb_get_guint8(tvb, offset);
        offset++;

        continuation_state = (guint8 *) wmem_alloc(wmem_file_scope(), continuation_state_length);
        packet_scope_string = tvb_bytes_to_ep_str(tvb, offset, continuation_state_length);
        memcpy(continuation_state, packet_scope_string, continuation_state_length);

        if (!pinfo->fd->flags.visited) {
            if (is_request) {
                tid_request = (tid_request_t *) wmem_new(wmem_file_scope(), tid_request_t);
                tid_request->interface_id              = interface_id;
                tid_request->adapter_id                = adapter_id;
                tid_request->chandle                   = chandle;
                tid_request->psm                       = psm;
                tid_request->tid                       = tid;

                if (uuid_array)
                    tid_request->uuid_array = *uuid_array;
                else
                    tid_request->uuid_array = NULL;

                if (record_handle)
                    tid_request->record_handle = *record_handle;
                else
                    tid_request->record_handle = 0;

                /
                tid_request->data        = NULL;
                tid_request->data_length = 0;

                tid_request->pdu_type = pdu_type;

                tid_request->continuation_state        = continuation_state;
                tid_request->continuation_state_length = continuation_state_length;

                wmem_tree_insert32_array(tid_requests, key, tid_request);
            } else {
                tid_request = (tid_request_t *) wmem_tree_lookup32_array_le(tid_requests, key);
                if (tid_request && tid_request->interface_id == interface_id &&
                        tid_request->adapter_id == adapter_id &&
                        tid_request->chandle == chandle &&
                        tid_request->psm == psm &&
                        tid_request->tid == tid) {
                    /

                    if (tid_request->continuation_state_length > 0) {
                        /

                        k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                        k_continuation_state[0] = tid_request->continuation_state_length;
                        memcpy(&k_continuation_state[1], tid_request->continuation_state, tid_request->continuation_state_length);
                        continuation_state_array = (guint32 *) k_continuation_state;

                        k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                        k_continuation_state[0] = tid_request->continuation_state_length;
                        memcpy(&k_continuation_state[1], tid_request->continuation_state, tid_request->continuation_state_length);

                        k_interface_id       = interface_id;
                        k_adapter_id         = adapter_id;
                        k_chandle            = chandle;
                        k_psm                = psm;
                        k_pdu_type           = tid_request->pdu_type;
                        k_frame_number       = frame_number;

                        key[0].length = 1;
                        key[0].key    = &k_interface_id;
                        key[1].length = 1;
                        key[1].key    = &k_adapter_id;
                        key[2].length = 1;
                        key[2].key    = &k_chandle;
                        key[3].length = 1;
                        key[3].key    = &k_psm;
                        key[4].length = 1;
                        key[4].key    = &k_pdu_type;
                        key[5].length = 1;
                        key[5].key    = (guint32 *) &k_continuation_state[0];
                        key[6].length = 1;
                        key[6].key    = (guint32 *) &k_continuation_state[4];
                        key[7].length = 1;
                        key[7].key    = (guint32 *) &k_continuation_state[8];
                        key[8].length = 1;
                        key[8].key    = (guint32 *) &k_continuation_state[12];
                        key[9].length = 1;
                        key[9].key    = (guint32 *) &k_continuation_state[16];
                        key[10].length = 1;
                        key[10].key    = &k_frame_number;
                        key[11].length = 0;
                        key[11].key    = NULL;

                        continuation_state_data = (continuation_state_data_t *) wmem_tree_lookup32_array_le(continuation_states, key);
                        if (continuation_state_data && continuation_state_data->interface_id == interface_id &&
                                continuation_state_data->adapter_id == adapter_id &&
                                continuation_state_data->chandle == chandle &&
                                continuation_state_data->psm == psm &&
                                continuation_state_data->pdu_type == tid_request->pdu_type &&
                                continuation_state_data->continuation_state[0] == continuation_state_array[0] &&
                                continuation_state_data->continuation_state[1] == continuation_state_array[1] &&
                                continuation_state_data->continuation_state[2] == continuation_state_array[2] &&
                                continuation_state_data->continuation_state[3] == continuation_state_array[3] &&
                                continuation_state_data->continuation_state[4] == continuation_state_array[4]) {
                            tid_request->data = (guint8 *) wmem_alloc(wmem_file_scope(), continuation_state_data->data_length + attribute_list_byte_count);
                            tid_request->data_length = continuation_state_data->data_length + attribute_list_byte_count;
                            memcpy(tid_request->data, continuation_state_data->data, continuation_state_data->data_length);
                            tvb_memcpy(tvb, tid_request->data + continuation_state_data->data_length, attribute_list_byte_offset, attribute_list_byte_count);
                        }
                    } else {
                        tid_request->data        = (guint8 *) wmem_alloc(wmem_file_scope(), attribute_list_byte_count);
                        tid_request->data_length = attribute_list_byte_count;

                        tvb_memcpy(tvb, tid_request->data, attribute_list_byte_offset, attribute_list_byte_count);
                    }

                    if (uuid_array) *uuid_array = tid_request->uuid_array;
                    if (record_handle) *record_handle = tid_request->record_handle;

                    /
                    k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                    k_continuation_state[0] = continuation_state_length;
                    memcpy(&k_continuation_state[1], continuation_state, continuation_state_length);
                    continuation_state_array = (guint32 *) k_continuation_state;

                    k_continuation_state = (guint8 *) wmem_alloc0(wmem_packet_scope(), 20);
                    k_continuation_state[0] = continuation_state_length;
                    memcpy(&k_continuation_state[1], continuation_state, continuation_state_length);

                    k_interface_id       = interface_id;
                    k_adapter_id         = adapter_id;
                    k_chandle            = chandle;
                    k_psm                = psm;
                    k_pdu_type           = pdu_type;
                    k_frame_number       = frame_number;

                    key[0].length = 1;
                    key[0].key    = &k_interface_id;
                    key[1].length = 1;
                    key[1].key    = &k_adapter_id;
                    key[2].length = 1;
                    key[2].key    = &k_chandle;
                    key[3].length = 1;
                    key[3].key    = &k_psm;
                    key[4].length = 1;
                    key[4].key    = &k_pdu_type;
                    key[5].length = 1;
                    key[5].key    = (guint32 *) &k_continuation_state[0];
                    key[6].length = 1;
                    key[6].key    = (guint32 *) &k_continuation_state[4];
                    key[7].length = 1;
                    key[7].key    = (guint32 *) &k_continuation_state[8];
                    key[8].length = 1;
                    key[8].key    = (guint32 *) &k_continuation_state[12];
                    key[9].length = 1;
                    key[9].key    = (guint32 *) &k_continuation_state[16];
                    key[10].length = 1;
                    key[10].key    = &k_frame_number;
                    key[11].length = 0;
                    key[11].key    = NULL;

                    continuation_state_data = (continuation_state_data_t *) wmem_new(wmem_file_scope(), continuation_state_data_t);
                    continuation_state_data->interface_id = interface_id;
                    continuation_state_data->adapter_id = adapter_id;
                    continuation_state_data->chandle = chandle;
                    continuation_state_data->psm = psm;
                    continuation_state_data->pdu_type = pdu_type;
                    continuation_state_data->continuation_state[0] = continuation_state_array[0];
                    continuation_state_data->continuation_state[1] = continuation_state_array[1];
                    continuation_state_data->continuation_state[2] = continuation_state_array[2];
                    continuation_state_data->continuation_state[3] = continuation_state_array[3];
                    continuation_state_data->continuation_state[4] = continuation_state_array[4];
                    continuation_state_data->data = tid_request->data;
                    continuation_state_data->data_length = tid_request->data_length;

                    wmem_tree_insert32_array(continuation_states, key, continuation_state_data);
                }
            }

            k_interface_id = interface_id;
            k_adapter_id   = adapter_id;
            k_chandle      = chandle;
            k_psm          = psm;
            k_tid          = tid;
            k_frame_number = frame_number;

            key[0].length = 1;
            key[0].key    = &k_interface_id;
            key[1].length = 1;
            key[1].key    = &k_adapter_id;
            key[2].length = 1;
            key[2].key    = &k_chandle;
            key[3].length = 1;
            key[3].key    = &k_psm;
            key[4].length = 1;
            key[4].key    = &k_tid;
            key[5].length = 1;
            key[5].key    = &k_frame_number;
            key[6].length = 0;
            key[6].key    = NULL;
        }

        /
        if (!is_request) {
            tid_request = (tid_request_t *) wmem_tree_lookup32_array_le(tid_requests, key);
            if (tid_request && tid_request->interface_id == interface_id &&
                    tid_request->adapter_id == adapter_id &&
                    tid_request->chandle == chandle &&
                    tid_request->psm == psm &&
                    tid_request->tid == tid) {
                tvbuff_t *next_tvb;

                next_tvb = tvb_new_child_real_data(tvb, tid_request->data,
                        tid_request->data_length, tid_request->data_length);

                if (new_tvb) *new_tvb = next_tvb;
                if (tid_request->continuation_state_length && is_first) *is_first = FALSE;

                if (uuid_array) *uuid_array = tid_request->uuid_array;
                if (record_handle) *record_handle = tid_request->record_handle;
            }

        }
    }

    return offset;
}
