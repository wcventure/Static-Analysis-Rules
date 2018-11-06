static gboolean
dissect_opensafety_checksum(tvbuff_t *message_tvb, packet_info *pinfo, proto_tree *opensafety_tree,
                            guint8 type, guint16 frameStart1, guint16 frameStart2 )
{
    guint16     frame1_crc, frame2_crc;
    guint16     calc1_crc, calc2_crc;
    guint       dataLength, frame2Length;
    guint8     *bytes, ctr = 0, crcType = OPENSAFETY_CHECKSUM_CRC8;
    proto_item *item;
    proto_tree *checksum_tree;
    gint        start;
    gint        length;
    gboolean    isSlim = FALSE;
    GByteArray *scmUDID = NULL;

    dataLength = OSS_FRAME_LENGTH_T(message_tvb, frameStart1);
    start = OSS_FRAME_POS_DATA + dataLength + frameStart1;

    if (OSS_FRAME_LENGTH_T(message_tvb, frameStart1) > OSS_PAYLOAD_MAXSIZE_FOR_CRC8)
        frame1_crc = tvb_get_letohs(message_tvb, start);
    else
        frame1_crc = tvb_get_guint8(message_tvb, start);

    if ( type == OPENSAFETY_SLIM_SSDO_MESSAGE_TYPE )
        isSlim = TRUE;

    frame2Length = (isSlim ? 0 : dataLength) + 5;

    length = (dataLength > OSS_PAYLOAD_MAXSIZE_FOR_CRC8 ? OPENSAFETY_CHECKSUM_CRC16 : OPENSAFETY_CHECKSUM_CRC8);
    item = proto_tree_add_uint_format(opensafety_tree, hf_oss_crc, message_tvb, start, length, frame1_crc,
                                      "CRC for subframe #1: 0x%04X", frame1_crc);

    checksum_tree = proto_item_add_subtree(item, ett_opensafety_checksum);

    bytes = (guint8*)tvb_memdup(wmem_packet_scope(), message_tvb, frameStart1, dataLength + 4);
    if ( dataLength > OSS_PAYLOAD_MAXSIZE_FOR_CRC8 )
    {
        calc1_crc = crc16_0x755B(bytes, dataLength + 4, 0);
        if ( frame1_crc == calc1_crc )
            crcType = OPENSAFETY_CHECKSUM_CRC16;
        if ( frame1_crc != calc1_crc )
        {
            calc1_crc = crc16_0x5935(bytes, dataLength + 4, 0);
            if ( frame1_crc == calc1_crc )
            {
                crcType = OPENSAFETY_CHECKSUM_CRC16SLIM;
                if ( ! isSlim )
                    expert_add_info(pinfo, item, &ei_crc_slimssdo_instead_of_spdo );
            }
        }
    }
    else
        calc1_crc = crc8_0x2F(bytes, dataLength + 4, 0);

    item = proto_tree_add_boolean(checksum_tree, hf_oss_crc_valid, message_tvb,
            frameStart1, dataLength + 4, (frame1_crc == calc1_crc));
    PROTO_ITEM_SET_GENERATED(item);
    /
    proto_tree_add_uint(checksum_tree, hf_oss_crc_type, message_tvb, start, length, crcType );

    start = frameStart2 + (isSlim ? 5 : dataLength + OSS_FRAME_POS_DATA + 1 );
    if (OSS_FRAME_LENGTH_T(message_tvb, frameStart1) > OSS_PAYLOAD_MAXSIZE_FOR_CRC8)
        frame2_crc = tvb_get_letohs(message_tvb, start);
    else
        frame2_crc = tvb_get_guint8(message_tvb, start);

    /
    calc2_crc = 0xFFFF;

    if ( global_calculate_crc2 )
    {
        bytes = (guint8*)tvb_memdup(wmem_packet_scope(), message_tvb, frameStart2, frame2Length + length);

        /
        if ( isSlim == TRUE )
            dataLength = 0;

        scmUDID = g_byte_array_new();
        if ( hex_str_to_bytes((local_scm_udid != NULL ? local_scm_udid : global_scm_udid), scmUDID, TRUE) && scmUDID->len == 6 )
        {
            if ( type != OPENSAFETY_SNMT_MESSAGE_TYPE )
            {
                for ( ctr = 0; ctr < 6; ctr++ )
                    bytes[ctr] = bytes[ctr] ^ (guint8)(scmUDID->data[ctr]);

                /
                if ( dataLength == 0 )
                    frame2_crc = ( ( isSlim && length == 2 ) ? ( ( bytes[6] << 8 ) + bytes[5] ) : bytes[5] );
            }

            item = proto_tree_add_uint_format(opensafety_tree, hf_oss_crc, message_tvb, start, length, frame2_crc,
                    "CRC for subframe #2: 0x%04X", frame2_crc);

            checksum_tree = proto_item_add_subtree(item, ett_opensafety_checksum);

            if ( OSS_FRAME_LENGTH_T(message_tvb, frameStart1) > OSS_PAYLOAD_MAXSIZE_FOR_CRC8 )
            {
                calc2_crc = crc16_0x755B(bytes, frame2Length, 0);
                if ( frame2_crc != calc2_crc )
                    calc2_crc = crc16_0x5935(bytes, frame2Length, 0);
            }
            else
                calc2_crc = crc8_0x2F(bytes, frame2Length, 0);

            item = proto_tree_add_boolean(checksum_tree, hf_oss_crc2_valid, message_tvb,
                    frameStart2, frame2Length, (frame2_crc == calc2_crc));
            PROTO_ITEM_SET_GENERATED(item);

            if ( frame2_crc != calc2_crc )
            {
                item = proto_tree_add_uint_format(checksum_tree, hf_oss_crc, message_tvb,
                        frameStart2, frame2Length, calc2_crc, "Calculated CRC: 0x%04X", calc2_crc);
                PROTO_ITEM_SET_GENERATED(item);
                expert_add_info(pinfo, item, &ei_crc_frame_2_invalid );
            }
        }
        else
            expert_add_info(pinfo, item, &ei_crc_frame_2_unknown_scm_udid );
    }

    /
    return (gboolean) (frame1_crc == calc1_crc) && ( global_calculate_crc2 == TRUE ? (frame2_crc == calc2_crc) : TRUE);
}
