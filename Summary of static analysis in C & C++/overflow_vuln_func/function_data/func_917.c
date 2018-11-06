static gboolean
dissect_opensafety_message(guint16 frameStart1, guint16 frameStart2, guint8 type,
                           tvbuff_t *message_tvb, packet_info *pinfo,
                           proto_item *opensafety_item, proto_tree *opensafety_tree, guint8 u_nrInPackage)
{
    guint8      b_ID, ctr;
    guint8      scm_udid[6];
    GByteArray *scmUDID = NULL;
    gboolean    validSCMUDID;
    proto_item *item;
    gboolean    messageTypeUnknown, crcValid;

    messageTypeUnknown = FALSE;

    for ( ctr = 0; ctr < 6; ctr++ )
        scm_udid[ctr] = 0;

    b_ID = OSS_FRAME_ID_T(message_tvb, frameStart1);
    /
    if ( type == OPENSAFETY_SPDO_MESSAGE_TYPE )
        b_ID = b_ID & 0xF8;

    col_append_fstr(pinfo->cinfo, COL_INFO, (u_nrInPackage > 1 ? " | %s" : "%s" ),
            val_to_str(b_ID, message_type_values, "Unknown Message (0x%02X) "));

    {
        if ( type == OPENSAFETY_SNMT_MESSAGE_TYPE )
        {
            dissect_opensafety_snmt_message ( message_tvb, pinfo, opensafety_tree, frameStart1, frameStart2 );
        }
        else
        {
            validSCMUDID = FALSE;
            scmUDID = g_byte_array_new();

            if ( hex_str_to_bytes((local_scm_udid != NULL ? local_scm_udid : global_scm_udid), scmUDID, TRUE) && scmUDID->len == 6 )
            {
                validSCMUDID = TRUE;

                /
                b_ID = OSS_FRAME_ID_T(message_tvb, frameStart2) ^ (guint8)(scmUDID->data[OSS_FRAME_POS_ID]);

                if ( ( OSS_FRAME_ID_T(message_tvb, frameStart1) ^ b_ID ) != 0 )
                    validSCMUDID = FALSE;
                else
                    for ( ctr = 0; ctr < 6; ctr++ )
                        scm_udid[ctr] = scmUDID->data[ctr];
            }

            if ( strlen ( (local_scm_udid != NULL ? local_scm_udid : global_scm_udid) ) > 0  && scmUDID->len == 6 )
            {
                if ( local_scm_udid != NULL )
                {
                    item = proto_tree_add_string(opensafety_tree, hf_oss_scm_udid_auto, message_tvb, 0, 0, local_scm_udid);
                    if ( ! validSCMUDID )
                        expert_add_info(pinfo, item, &ei_message_id_field_mismatch );
                }
                else
                    item = proto_tree_add_string(opensafety_tree, hf_oss_scm_udid, message_tvb, 0, 0, global_scm_udid);
                PROTO_ITEM_SET_GENERATED(item);
            }

            item = proto_tree_add_boolean(opensafety_tree, hf_oss_scm_udid_valid, message_tvb, 0, 0, validSCMUDID);
            if ( scmUDID->len != 6 )
                expert_add_info(pinfo, item, &ei_scmudid_invalid_preference );
            PROTO_ITEM_SET_GENERATED(item);

            g_byte_array_free( scmUDID, TRUE);

            if ( type == OPENSAFETY_SSDO_MESSAGE_TYPE || type == OPENSAFETY_SLIM_SSDO_MESSAGE_TYPE )
            {
                dissect_opensafety_ssdo_message ( message_tvb, pinfo, opensafety_tree, frameStart1, frameStart2, validSCMUDID, scm_udid );
            }
            else if ( type == OPENSAFETY_SPDO_MESSAGE_TYPE )
            {
                dissect_opensafety_spdo_message ( message_tvb, pinfo, opensafety_tree, frameStart1, frameStart2, validSCMUDID, scm_udid );
            }
            else
            {
                messageTypeUnknown = TRUE;
            }
        }

        crcValid = FALSE;
        item = proto_tree_add_uint(opensafety_tree, hf_oss_length,
                                   message_tvb, OSS_FRAME_POS_LEN + frameStart1, 1, OSS_FRAME_LENGTH_T(message_tvb, frameStart1));
        if ( messageTypeUnknown )
        {
            expert_add_info(pinfo, item, &ei_message_unknown_type );
        }
        else
        {
            crcValid = dissect_opensafety_checksum ( message_tvb, pinfo, opensafety_tree, type, frameStart1, frameStart2 );
        }

        if ( ! crcValid )
        {
            expert_add_info(pinfo, opensafety_item, &ei_crc_frame_1_invalid );
        }

        /
        if ( crcValid && type == OPENSAFETY_SNMT_MESSAGE_TYPE )
        {
            if ( OSS_FRAME_ID_T(message_tvb, frameStart1) != OSS_FRAME_ID_T(message_tvb, frameStart2) )
                expert_add_info(pinfo, opensafety_item, &ei_crc_frame_1_valid_frame2_invalid );
        }
    }

    return TRUE;
}
