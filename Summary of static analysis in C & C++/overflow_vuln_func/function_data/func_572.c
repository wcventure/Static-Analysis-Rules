static int
dissect_sigcomp_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *sigcomp_tree)
{

/
    tvbuff_t   *udvm_tvb, *msg_tvb, *udvm2_tvb;
    tvbuff_t   *decomp_tvb   = NULL;
    proto_item *udvm_bytecode_item, *udvm_exe_item;
    proto_tree *sigcomp_udvm_tree, *sigcomp_udvm_exe_tree;
    gint        offset       = 0;
    gint        bytecode_offset;
    guint16     partial_state_len;
    guint       octet;
    guint8      returned_feedback_field[128];
    guint8      partial_state[12];
    guint       tbit;
    guint16     len          = 0;
    guint16     bytecode_len = 0;
    guint       destination;
    gint        msg_len      = 0;
    guint8     *buff;
    guint16     p_id_start;
    guint8      i;
    guint16     state_begin;
    guint16     state_length;
    guint16     state_address;
    guint16     state_instruction;
    guint16     result_code;
    gchar      *partial_state_str;
    guint8      nack_version;



/
    octet = tvb_get_guint8(tvb, offset);

/

    proto_tree_add_item(sigcomp_tree,hf_sigcomp_t_bit, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(sigcomp_tree,hf_sigcomp_len, tvb, offset, 1, ENC_BIG_ENDIAN);
    tbit = ( octet & 0x04)>>2;
    partial_state_len = octet & 0x03;
    offset ++;
    if ( partial_state_len != 0 ) {
        /
        partial_state_len = partial_state_len * 3 + 3;

        /
        col_set_str(pinfo->cinfo, COL_INFO, "Msg format 1");

        if ( tbit == 1 ) {
            /
            len = 1;
            octet = tvb_get_guint8(tvb, offset);
            /

            if ( (octet & 0x80) != 0 ) {
                len = octet & 0x7f;
                proto_tree_add_item(sigcomp_tree,hf_sigcomp_returned_feedback_item_len,
                                    tvb, offset, 1, ENC_BIG_ENDIAN);
                offset ++;
                tvb_memcpy(tvb,returned_feedback_field,offset, len);
            } else {
                returned_feedback_field[0] = tvb_get_guint8(tvb, offset) & 0x7f;
            }
            proto_tree_add_bytes(sigcomp_tree,hf_sigcomp_returned_feedback_item,
                                 tvb, offset, len, returned_feedback_field);
            offset = offset + len;
        }
        tvb_memcpy(tvb, partial_state, offset, partial_state_len);
        partial_state_str = bytes_to_str(wmem_packet_scope(), partial_state, partial_state_len);
        proto_tree_add_string(sigcomp_tree,hf_sigcomp_partial_state,
            tvb, offset, partial_state_len, partial_state_str);
        offset = offset + partial_state_len;
        msg_len = tvb_reported_length_remaining(tvb, offset);

        if (msg_len>0) {
            proto_item *ti;
            ti = proto_tree_add_uint(sigcomp_tree, hf_sigcomp_remaining_message_bytes, tvb,
                                     offset, 0, msg_len);
            PROTO_ITEM_SET_GENERATED(ti);
        }

        if ( decompress ) {
            msg_tvb = tvb_new_subset_length(tvb, offset, msg_len);
            /
            /
            buff = (guint8 *)wmem_alloc0(pinfo->pool, UDVM_MEMORY_SIZE);


            p_id_start = 0;
            state_begin = 0;
            /
            state_length = 0;
            state_address = 0;
            state_instruction =0;

            i = 0;
            while ( i < partial_state_len ) {
                buff[i] = partial_state[i];
                i++;
            }

/
#if 0
            result_code = udvm_state_access(tvb, sigcomp_tree, buff, p_id_start, partial_state_len, state_begin, &state_length,
                &state_address, &state_instruction, hf_sigcomp_partial_state);
#endif
            result_code = udvm_state_access(tvb, sigcomp_tree, buff, p_id_start, STATE_MIN_ACCESS_LEN, state_begin, &state_length,
                &state_address, &state_instruction, hf_sigcomp_partial_state);

/
            if ( result_code != 0 ) {
                proto_item *ti;
                ti = proto_tree_add_text(sigcomp_tree, tvb, 0, -1,"Failed to Access state Wireshark UDVM diagnostic: %s.",
                                         val_to_str(result_code, result_code_vals,"Unknown (%u)"));
                PROTO_ITEM_SET_GENERATED(ti);
                return tvb_length(tvb);
            }

            udvm_tvb = tvb_new_child_real_data(tvb, buff,state_length+state_address,state_length+state_address);
            add_new_data_source(pinfo, udvm_tvb, "State/ExecutionTrace");

            udvm2_tvb = tvb_new_subset_length(udvm_tvb, state_address, state_length);
            udvm_exe_item = proto_tree_add_item(sigcomp_tree, hf_udvm_execution_trace,
                                                udvm2_tvb, 0, state_length,
                                                ENC_NA);
            sigcomp_udvm_exe_tree = proto_item_add_subtree( udvm_exe_item, ett_sigcomp_udvm_exe);

            decomp_tvb = decompress_sigcomp_message(udvm2_tvb, msg_tvb, pinfo,
                           sigcomp_udvm_exe_tree, state_address,
                           udvm_print_detail_level, hf_sigcomp_partial_state,
                           offset, state_length, partial_state_len, state_instruction);


            if ( decomp_tvb ) {
                proto_item *ti;
                guint32 compression_ratio =
                    (guint32)(((float)tvb_length(decomp_tvb) / (float)tvb_length(tvb)) * 100);

                /
                ti = proto_tree_add_uint(sigcomp_tree, hf_sigcomp_compression_ratio, decomp_tvb,
                                         0, 0, compression_ratio);
                PROTO_ITEM_SET_GENERATED(ti);

                if ( display_raw_txt )
                    tvb_raw_text_add(decomp_tvb, top_tree);

                col_append_str(pinfo->cinfo, COL_PROTOCOL, "/");
                col_set_fence(pinfo->cinfo,COL_PROTOCOL);
                call_dissector(sip_handle, decomp_tvb, pinfo, top_tree);
            }
        }/

    }
    else{
        /
    col_set_str(pinfo->cinfo, COL_INFO, "Msg format 2");
        if ( tbit == 1 ) {
            /
            len = 1;
            octet = tvb_get_guint8(tvb, offset);
            if ( (octet & 0x80) != 0 ) {
                len = octet & 0x7f;
                proto_tree_add_item(sigcomp_tree,hf_sigcomp_returned_feedback_item_len,
                                    tvb, offset, 1, ENC_BIG_ENDIAN);
                offset ++;
            }
            tvb_memcpy(tvb,returned_feedback_field,offset, len);
            proto_tree_add_bytes(sigcomp_tree,hf_sigcomp_returned_feedback_item,
                                 tvb, offset, len, returned_feedback_field);
            offset = offset + len;
        }
        len = tvb_get_ntohs(tvb, offset) >> 4;
        nack_version = tvb_get_guint8(tvb, offset+1) & 0x0f;
        if ((len == 0) && (nack_version == 1)) {
            /
            proto_item *reason_ti;
            guint8 opcode;
            offset++;
            proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_ver, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
            octet = tvb_get_guint8(tvb, offset);
            reason_ti = proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_reason_code, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
            opcode = tvb_get_guint8(tvb, offset);
            proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_failed_op_code, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;

            /
            expert_add_info_format(pinfo, reason_ti, &ei_sigcomp_nack_failed_op_code,
                                   "SigComp NACK (reason=%s, opcode=%s)",
                                   val_to_str_ext_const(octet, &sigcomp_nack_reason_code_vals_ext, "Unknown"),
                                   val_to_str_ext_const(opcode, &udvm_instruction_code_vals_ext, "Unknown"));

            proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_pc, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset = offset +2;
            proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_sha1, tvb, offset, 20, ENC_NA);
            offset = offset +20;

            /
            col_append_fstr(pinfo->cinfo, COL_INFO, "  NACK reason=%s, opcode=%s",
                            val_to_str_ext_const(octet, &sigcomp_nack_reason_code_vals_ext, "Unknown"),
                            val_to_str_ext_const(opcode, &udvm_instruction_code_vals_ext, "Unknown"));

            switch ( octet) {
            case SIGCOMP_NACK_STATE_NOT_FOUND:
            case SIGCOMP_NACK_ID_NOT_UNIQUE:
            case SIGCOMP_NACK_STATE_TOO_SHORT:
                /
                proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_state_id, tvb, offset, -1, ENC_NA);
                break;
            case SIGCOMP_NACK_CYCLES_EXHAUSTED:
                /
                proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_cycles_per_bit, tvb, offset, 1, ENC_BIG_ENDIAN);
                break;
            case SIGCOMP_NACK_BYTECODES_TOO_LARGE:
                /
                proto_tree_add_item(sigcomp_tree,hf_sigcomp_nack_memory_size, tvb, offset, 2, ENC_BIG_ENDIAN);
                break;
            default:
                break;
            }
        } else {
            octet = tvb_get_guint8(tvb, (offset + 1));
            destination = (octet & 0x0f);
            if ( destination != 0 )
                destination = 64 + ( destination * 64 );
            proto_tree_add_item(sigcomp_tree,hf_sigcomp_code_len, tvb, offset, 2, ENC_BIG_ENDIAN);
            proto_tree_add_item(sigcomp_tree,hf_sigcomp_destination, tvb, (offset+ 1), 1, ENC_BIG_ENDIAN);
            offset = offset +2;

            bytecode_len = len;
            bytecode_offset = offset;
            udvm_bytecode_item = proto_tree_add_item(sigcomp_tree, hf_sigcomp_udvm_bytecode, tvb,
                                                     bytecode_offset, bytecode_len, ENC_NA);
            proto_item_append_text(udvm_bytecode_item,
                                   " %u (0x%x) bytes", bytecode_len, bytecode_len);
            sigcomp_udvm_tree = proto_item_add_subtree( udvm_bytecode_item, ett_sigcomp_udvm);

            udvm_tvb = tvb_new_subset_length(tvb, offset, len);
            if ( dissect_udvm_code )
                dissect_udvm_bytecode(udvm_tvb, sigcomp_udvm_tree, destination);

            offset = offset + len;
            msg_len = tvb_reported_length_remaining(tvb, offset);
            if (msg_len>0) {
                proto_item *ti = proto_tree_add_text(sigcomp_tree, tvb, offset, -1,
                                                     "Remaining SigComp message %u bytes",
                                                     tvb_reported_length_remaining(tvb, offset));
                PROTO_ITEM_SET_GENERATED(ti);
            }
            if ( decompress ) {

                msg_tvb = tvb_new_subset_length(tvb, offset, msg_len);

                udvm_exe_item = proto_tree_add_item(sigcomp_tree, hf_udvm_execution_trace,
                                                    tvb, bytecode_offset, bytecode_len,
                                                    ENC_NA);
                sigcomp_udvm_exe_tree = proto_item_add_subtree( udvm_exe_item, ett_sigcomp_udvm_exe);
                decomp_tvb = decompress_sigcomp_message(udvm_tvb, msg_tvb, pinfo,
                           sigcomp_udvm_exe_tree, destination,
                           udvm_print_detail_level, hf_sigcomp_partial_state,
                           offset, 0, 0, destination);
                if ( decomp_tvb ) {
                    proto_item *ti;
                    guint32 compression_ratio =
                        (guint32)(((float)tvb_length(decomp_tvb) / (float)tvb_length(tvb)) * 100);

                    /
                    ti = proto_tree_add_uint(sigcomp_tree, hf_sigcomp_compression_ratio, decomp_tvb,
                                             0, 0, compression_ratio);
                    PROTO_ITEM_SET_GENERATED(ti);

                    if ( display_raw_txt )
                        tvb_raw_text_add(decomp_tvb, top_tree);

                    col_append_str(pinfo->cinfo, COL_PROTOCOL, "/");
                    col_set_fence(pinfo->cinfo,COL_PROTOCOL);
                    call_dissector(sip_handle, decomp_tvb, pinfo, top_tree);
                }
            } /
        }/

    }
    return tvb_length(tvb);
}
