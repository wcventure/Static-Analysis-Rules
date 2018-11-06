int ff_h264_decode_ref_pic_marking(H264Context *h, GetBitContext *gb){
    MpegEncContext * const s = &h->s;
    int i;

    h->mmco_index= 0;
    if(h->nal_unit_type == NAL_IDR_SLICE){ //FIXME fields
        s->broken_link= get_bits1(gb) -1;
        if(get_bits1(gb)){
            h->mmco[0].opcode= MMCO_LONG;
            h->mmco[0].long_arg= 0;
            h->mmco_index= 1;
        }
    }else{
        if(get_bits1(gb)){ // adaptive_ref_pic_marking_mode_flag
            for(i= 0; i<MAX_MMCO_COUNT; i++) {
                MMCOOpcode opcode= get_ue_golomb_31(gb);

                h->mmco[i].opcode= opcode;
                if(opcode==MMCO_SHORT2UNUSED || opcode==MMCO_SHORT2LONG){
                    h->mmco[i].short_pic_num= (h->curr_pic_num - get_ue_golomb(gb) - 1) & (h->max_pic_num - 1);
/
                }
                if(opcode==MMCO_SHORT2LONG || opcode==MMCO_LONG2UNUSED || opcode==MMCO_LONG || opcode==MMCO_SET_MAX_LONG){
                    unsigned int long_arg= get_ue_golomb_31(gb);
                    if(long_arg >= 32 || (long_arg >= 16 && !(opcode == MMCO_LONG2UNUSED && FIELD_PICTURE))){
                        av_log(h->s.avctx, AV_LOG_ERROR, "illegal long ref in memory management control operation %d\n", opcode);
                        return -1;
                    }
                    h->mmco[i].long_arg= long_arg;
                }

                if(opcode > (unsigned)MMCO_LONG){
                    av_log(h->s.avctx, AV_LOG_ERROR, "illegal memory management control operation %d\n", opcode);
                    return -1;
                }
                if(opcode == MMCO_END)
                    break;
            }
            h->mmco_index= i;
        }else{
            ff_generate_sliding_window_mmcos(h);
        }
    }

    return 0;
}
