int MPV_encode_picture(AVCodecContext *avctx,
                       unsigned char *buf, int buf_size, void *data)
{
    MpegEncContext *s = avctx->priv_data;
    AVFrame *pic_arg = data;
    int i, stuffing_count;

    if(avctx->pix_fmt != PIX_FMT_YUV420P && avctx->pix_fmt != PIX_FMT_YUVJ420P){
        av_log(avctx, AV_LOG_ERROR, "this codec supports only YUV420P\n");
        return -1;
    }
    
    for(i=0; i<avctx->thread_count; i++){
        int start_y= s->thread_context[i]->start_mb_y;
        int   end_y= s->thread_context[i]->  end_mb_y;
        int h= s->mb_height;
        uint8_t *start= buf + buf_size*start_y/h;
        uint8_t *end  = buf + buf_size*  end_y/h;

        init_put_bits(&s->thread_context[i]->pb, start, end - start);
    }

    s->picture_in_gop_number++;

    if(load_input_picture(s, pic_arg) < 0)
        return -1;
    
    select_input_picture(s);
    
    /
    if(s->new_picture.data[0]){
        s->pict_type= s->new_picture.pict_type;
//emms_c();
//printf("qs:%f %f %d\n", s->new_picture.quality, s->current_picture.quality, s->qscale);
        MPV_frame_start(s, avctx);

        encode_picture(s, s->picture_number);
        
        avctx->real_pict_num  = s->picture_number;
        avctx->header_bits = s->header_bits;
        avctx->mv_bits     = s->mv_bits;
        avctx->misc_bits   = s->misc_bits;
        avctx->i_tex_bits  = s->i_tex_bits;
        avctx->p_tex_bits  = s->p_tex_bits;
        avctx->i_count     = s->i_count;
        avctx->p_count     = s->mb_num - s->i_count - s->skip_count; //FIXME f/b_count in avctx
        avctx->skip_count  = s->skip_count;

        MPV_frame_end(s);

        if (s->out_format == FMT_MJPEG)
            mjpeg_picture_trailer(s);
        
        if(s->flags&CODEC_FLAG_PASS1)
            ff_write_pass1_stats(s);

        for(i=0; i<4; i++){
            avctx->error[i] += s->current_picture_ptr->error[i];
        }

        if(s->flags&CODEC_FLAG_PASS1)
            assert(avctx->header_bits + avctx->mv_bits + avctx->misc_bits + avctx->i_tex_bits + avctx->p_tex_bits == put_bits_count(&s->pb));
        flush_put_bits(&s->pb);
        s->frame_bits  = put_bits_count(&s->pb);

        stuffing_count= ff_vbv_update(s, s->frame_bits);
        if(stuffing_count){
            if(s->pb.buf_end - s->pb.buf - (put_bits_count(&s->pb)>>3) < stuffing_count + 50){
                av_log(s->avctx, AV_LOG_ERROR, "stuffing too large\n");
                return -1;
            }

            switch(s->codec_id){
            case CODEC_ID_MPEG1VIDEO:
            case CODEC_ID_MPEG2VIDEO:
                while(stuffing_count--){
                    put_bits(&s->pb, 8, 0);
                }
            break;
            case CODEC_ID_MPEG4:
                put_bits(&s->pb, 16, 0);
                put_bits(&s->pb, 16, 0x1C3);
                stuffing_count -= 4;
                while(stuffing_count--){
                    put_bits(&s->pb, 8, 0xFF);
                }
            break;
            default:
                av_log(s->avctx, AV_LOG_ERROR, "vbv buffer overflow\n");
            }
            flush_put_bits(&s->pb);
            s->frame_bits  = put_bits_count(&s->pb);
        }

        /    
        if(s->avctx->rc_max_rate && s->avctx->rc_min_rate == s->avctx->rc_max_rate && s->out_format == FMT_MPEG1
           && 90000LL * (avctx->rc_buffer_size-1) <= s->avctx->rc_max_rate*0xFFFFLL){
            int vbv_delay;

            assert(s->repeat_first_field==0);
            
            vbv_delay= lrintf(90000 * s->rc_context.buffer_index / s->avctx->rc_max_rate);
            assert(vbv_delay < 0xFFFF);

            s->vbv_delay_ptr[0] &= 0xF8;
            s->vbv_delay_ptr[0] |= vbv_delay>>13;
            s->vbv_delay_ptr[1]  = vbv_delay>>5;
            s->vbv_delay_ptr[2] &= 0x07;
            s->vbv_delay_ptr[2] |= vbv_delay<<3;
        }
        s->total_bits += s->frame_bits;
        avctx->frame_bits  = s->frame_bits;
    }else{
        assert((pbBufPtr(&s->pb) == s->pb.buf));
        s->frame_bits=0;
    }
    assert((s->frame_bits&7)==0);
    
    return s->frame_bits/8;
}
