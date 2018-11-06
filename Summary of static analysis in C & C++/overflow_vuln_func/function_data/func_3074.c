int ff_h264_decode_sei(H264Context *h){
    while (get_bits_left(&h->gb) > 16) {
        int size, type;

        type=0;
        do{
            if (get_bits_left(&h->gb) < 8)
                return AVERROR_INVALIDDATA;
            type+= show_bits(&h->gb, 8);
        }while(get_bits(&h->gb, 8) == 255);

        size=0;
        do{
            if (get_bits_left(&h->gb) < 8)
                return AVERROR_INVALIDDATA;
            size+= show_bits(&h->gb, 8);
        }while(get_bits(&h->gb, 8) == 255);

        if(h->avctx->debug&FF_DEBUG_STARTCODE)
            av_log(h->avctx, AV_LOG_DEBUG, "SEI %d len:%d\n", type, size);

        switch(type){
        case SEI_TYPE_PIC_TIMING: // Picture timing SEI
            if(decode_picture_timing(h) < 0)
                return -1;
            break;
        case SEI_TYPE_USER_DATA_ITU_T_T35:
            if(decode_user_data_itu_t_t35(h, size) < 0)
                return -1;
            break;
        case SEI_TYPE_USER_DATA_UNREGISTERED:
            if(decode_unregistered_user_data(h, size) < 0)
                return -1;
            break;
        case SEI_TYPE_RECOVERY_POINT:
            if(decode_recovery_point(h) < 0)
                return -1;
            break;
        case SEI_BUFFERING_PERIOD:
            if(decode_buffering_period(h) < 0)
                return -1;
            break;
        case SEI_TYPE_FRAME_PACKING:
            if(decode_frame_packing(h, size) < 0)
                return -1;
        default:
            skip_bits(&h->gb, 8*size);
        }

        //FIXME check bits here
        align_get_bits(&h->gb);
    }

    return 0;
}
