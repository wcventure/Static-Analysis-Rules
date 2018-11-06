static guint8 findSafetyFrame ( tvbuff_t *message_tvb, guint u_Offset, gboolean b_frame2first, guint *u_frameOffset, guint *u_frameLength )
{
    guint     ctr, rem_length;
    guint16   crc, f2crc, calcCrc;
    guint8    b_Length, crcOffset;
    guint8   *bytes;
    guint     b_ID;
    gboolean  found;

    found = 0;
    ctr = u_Offset;
    rem_length = tvb_reported_length_remaining (message_tvb, ctr);

    while ( rem_length >= OSS_MINIMUM_LENGTH)
    {
        /
        if ( ctr != 0 )
        {
            *u_frameLength = 0;
            *u_frameOffset = 0;

            crcOffset = 0;
            b_ID = tvb_get_guint8(message_tvb, ctr );

            if ( b_ID != 0x0 )
            {
                b_Length = tvb_get_guint8(message_tvb, ctr + 1 );

                /
                if ( ( b_ID != 0xFF ) && ( b_ID & 0x80 ) )
                {
                    /
                    rem_length = tvb_reported_length_remaining(message_tvb, ctr);

                    /
                    if ( (guint)( b_Length * 2 ) < ( rem_length + OSS_MINIMUM_LENGTH ) )
                    {

                        /
                        if ( ( b_Length <= (guint) 8 && ( b_Length <= rem_length ) ) ||
                            ( b_Length > (guint) 8 && ( ( b_Length + (guint) 5 ) <= rem_length ) ) )
                        {
                            /
                            if ( tvb_bytes_exist(message_tvb, ctr - 1, b_Length + 5) )
                            {
                                /
                                if ( ( ( b_ID >> 4 ) != 0x09 ) && ( ( b_ID >> 4 ) != 0x0F ) )
                                {
                                    /
                                    crc = tvb_get_guint8(message_tvb, ctr + 3 + b_Length );

                                    bytes = (guint8 *)tvb_memdup(wmem_packet_scope(), message_tvb, ctr - 1, b_Length + 5 );
                                    if ( b_Length > 8 ) {
                                        crc = tvb_get_letohs ( message_tvb, ctr + 3 + b_Length );
                                        crcOffset = 1;

                                        calcCrc = crc16_0x755B( bytes, b_Length + 4, 0 );
                                        if ( ( crc ^ calcCrc ) != 0 )
                                            calcCrc = crc16_0x5935( bytes, b_Length + 4, 0 );
                                    } else {
                                        calcCrc = crc8_0x2F ( bytes, b_Length + 4, 0 );
                                    }

                                    if ( ( crc ^ calcCrc ) == 0 )
                                    {
                                        /
                                        if ( ( b_ID >> 3 ) == ( OPENSAFETY_SLIM_SSDO_MESSAGE_TYPE >> 3 ) )
                                        {
                                            /
                                            if ( b_Length > 0 )
                                            {
                                                *u_frameOffset = ( ctr - 1 );
                                                *u_frameLength = b_Length + 2 * crcOffset + 11;

                                                /
                                                f2crc = tvb_get_guint8 ( message_tvb, ctr + 3 + 5 + b_Length );
                                                if ( b_Length > 8 )
                                                    f2crc = tvb_get_letohs ( message_tvb, ctr + 3 + 5 + b_Length );
                                                if ( crc != f2crc )
                                                {
                                                    found = 1;
                                                    break;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            *u_frameLength = 2 * b_Length + 2 * crcOffset + 11;
                                            *u_frameOffset = ( ctr - 1 );

                                            /
                                            found = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    /
                    if ( rem_length == OSS_MINIMUM_LENGTH )
                    {
                        b_ID = tvb_get_guint8(message_tvb, ctr );
                        b_Length = tvb_get_guint8(message_tvb, ctr + 2 );
                        if ( ( b_ID >> 3 ) == ( OPENSAFETY_SLIM_SSDO_MESSAGE_TYPE >> 3 ) )
                            b_Length = ( 11 + ( b_Length > 8 ? 2 : 0 ) + b_Length );
                        else
                            b_Length = ( 11 + ( b_Length > 8 ? 2 : 0 ) + 2 * b_Length );

                        if ( rem_length == b_Length )
                        {
                            ctr++;
                            rem_length++;
                            continue;
                        }
                    }
                }
            }
        }

        ctr++;
        rem_length = tvb_reported_length_remaining(message_tvb, ctr);

    }

    /
    if ( b_frame2first && found )
        *u_frameOffset = u_Offset;

    return (found ? 1 : 0);
}
