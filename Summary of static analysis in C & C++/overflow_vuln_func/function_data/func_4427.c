static int encode_frame(AVCodecContext *avctx, unsigned char *buf, int buf_size, void *data){
    HYuvContext *s = avctx->priv_data;
    AVFrame *pict = data;
    const int width= s->width;
    const int width2= s->width>>1;
    const int height= s->height;
    const int fake_ystride= s->interlaced ? pict->linesize[0]*2  : pict->linesize[0];
    const int fake_ustride= s->interlaced ? pict->linesize[1]*2  : pict->linesize[1];
    const int fake_vstride= s->interlaced ? pict->linesize[2]*2  : pict->linesize[2];
    AVFrame * const p= &s->picture;
    int i, j, size=0;

    *p = *pict;
    p->pict_type= FF_I_TYPE;
    p->key_frame= 1;
    
    if(s->context){
        for(i=0; i<3; i++){
            generate_len_table(s->len[i], s->stats[i], 256);
            if(generate_bits_table(s->bits[i], s->len[i])<0)
                return -1;
            size+= store_table(s, s->len[i], &buf[size]);
        }

        for(i=0; i<3; i++)
            for(j=0; j<256; j++)
                s->stats[i][j] >>= 1;
    }

    init_put_bits(&s->pb, buf+size, buf_size-size);

    if(avctx->pix_fmt == PIX_FMT_YUV422P || avctx->pix_fmt == PIX_FMT_YUV420P){
        int lefty, leftu, leftv, y, cy;

        put_bits(&s->pb, 8, leftv= p->data[2][0]);
        put_bits(&s->pb, 8, lefty= p->data[0][1]);
        put_bits(&s->pb, 8, leftu= p->data[1][0]);
        put_bits(&s->pb, 8,        p->data[0][0]);
        
        lefty= sub_left_prediction(s, s->temp[0], p->data[0]+2, width-2 , lefty);
        leftu= sub_left_prediction(s, s->temp[1], p->data[1]+1, width2-1, leftu);
        leftv= sub_left_prediction(s, s->temp[2], p->data[2]+1, width2-1, leftv);
        
        encode_422_bitstream(s, width-2);
        
        if(s->predictor==MEDIAN){
            int lefttopy, lefttopu, lefttopv;
            cy=y=1;
            if(s->interlaced){
                lefty= sub_left_prediction(s, s->temp[0], p->data[0]+p->linesize[0], width , lefty);
                leftu= sub_left_prediction(s, s->temp[1], p->data[1]+p->linesize[1], width2, leftu);
                leftv= sub_left_prediction(s, s->temp[2], p->data[2]+p->linesize[2], width2, leftv);
        
                encode_422_bitstream(s, width);
                y++; cy++;
            }
            
            lefty= sub_left_prediction(s, s->temp[0], p->data[0]+fake_ystride, 4, lefty);
            leftu= sub_left_prediction(s, s->temp[1], p->data[1]+fake_ustride, 2, leftu);
            leftv= sub_left_prediction(s, s->temp[2], p->data[2]+fake_vstride, 2, leftv);
        
            encode_422_bitstream(s, 4);

            lefttopy= p->data[0][3];
            lefttopu= p->data[1][1];
            lefttopv= p->data[2][1];
            s->dsp.sub_hfyu_median_prediction(s->temp[0], p->data[0]+4, p->data[0] + fake_ystride+4, width-4 , &lefty, &lefttopy);
            s->dsp.sub_hfyu_median_prediction(s->temp[1], p->data[1]+2, p->data[1] + fake_ustride+2, width2-2, &leftu, &lefttopu);
            s->dsp.sub_hfyu_median_prediction(s->temp[2], p->data[2]+2, p->data[2] + fake_vstride+2, width2-2, &leftv, &lefttopv);
            encode_422_bitstream(s, width-4);
            y++; cy++;

            for(; y<height; y++,cy++){
                uint8_t *ydst, *udst, *vdst;
                    
                if(s->bitstream_bpp==12){
                    while(2*cy > y){
                        ydst= p->data[0] + p->linesize[0]*y;
                        s->dsp.sub_hfyu_median_prediction(s->temp[0], ydst - fake_ystride, ydst, width , &lefty, &lefttopy);
                        encode_gray_bitstream(s, width);
                        y++;
                    }
                    if(y>=height) break;
                }
                ydst= p->data[0] + p->linesize[0]*y;
                udst= p->data[1] + p->linesize[1]*cy;
                vdst= p->data[2] + p->linesize[2]*cy;

                s->dsp.sub_hfyu_median_prediction(s->temp[0], ydst - fake_ystride, ydst, width , &lefty, &lefttopy);
                s->dsp.sub_hfyu_median_prediction(s->temp[1], udst - fake_ustride, udst, width2, &leftu, &lefttopu);
                s->dsp.sub_hfyu_median_prediction(s->temp[2], vdst - fake_vstride, vdst, width2, &leftv, &lefttopv);

                encode_422_bitstream(s, width);
            }
        }else{
            for(cy=y=1; y<height; y++,cy++){
                uint8_t *ydst, *udst, *vdst;
                
                /
                if(s->bitstream_bpp==12){
                    ydst= p->data[0] + p->linesize[0]*y;

                    if(s->predictor == PLANE && s->interlaced < y){
                        s->dsp.diff_bytes(s->temp[1], ydst, ydst - fake_ystride, width);

                        lefty= sub_left_prediction(s, s->temp[0], s->temp[1], width , lefty);
                    }else{
                        lefty= sub_left_prediction(s, s->temp[0], ydst, width , lefty);
                    }
                    encode_gray_bitstream(s, width);
                    y++;
                    if(y>=height) break;
                }
                
                ydst= p->data[0] + p->linesize[0]*y;
                udst= p->data[1] + p->linesize[1]*cy;
                vdst= p->data[2] + p->linesize[2]*cy;

                if(s->predictor == PLANE && s->interlaced < cy){
                    s->dsp.diff_bytes(s->temp[1], ydst, ydst - fake_ystride, width);
                    s->dsp.diff_bytes(s->temp[2], udst, udst - fake_ustride, width2);
                    s->dsp.diff_bytes(s->temp[2] + 1250, vdst, vdst - fake_vstride, width2);

                    lefty= sub_left_prediction(s, s->temp[0], s->temp[1], width , lefty);
                    leftu= sub_left_prediction(s, s->temp[1], s->temp[2], width2, leftu);
                    leftv= sub_left_prediction(s, s->temp[2], s->temp[2] + 1250, width2, leftv);
                }else{
                    lefty= sub_left_prediction(s, s->temp[0], ydst, width , lefty);
                    leftu= sub_left_prediction(s, s->temp[1], udst, width2, leftu);
                    leftv= sub_left_prediction(s, s->temp[2], vdst, width2, leftv);
                }

                encode_422_bitstream(s, width);
            }
        }        
    }else{
        av_log(avctx, AV_LOG_ERROR, "Format not supported!\n");
    }
    emms_c();
    
    size+= (put_bits_count(&s->pb)+31)/8;
    size/= 4;
    
    if((s->flags&CODEC_FLAG_PASS1) && (s->picture_number&31)==0){
        int j;
        char *p= avctx->stats_out;
        char *end= p + 1024*30;
        for(i=0; i<3; i++){
            for(j=0; j<256; j++){
                snprintf(p, end-p, "%llu ", s->stats[i][j]);
                p+= strlen(p);
                s->stats[i][j]= 0;
            }
            snprintf(p, end-p, "\n");
            p++;
        }
    }
    if(!(s->avctx->flags2 & CODEC_FLAG2_NO_OUTPUT)){
        flush_put_bits(&s->pb);
        s->dsp.bswap_buf((uint32_t*)buf, (uint32_t*)buf, size);
        avctx->stats_out[0] = '\0';
    }
    
    s->picture_number++;

    return size*4;
}
