static int encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                        const AVFrame *pict, int *got_packet)
{
    FFV1Context *f = avctx->priv_data;
    RangeCoder * const c= &f->slice_context[0]->c;
    AVFrame * const p= &f->picture;
    int used_count= 0;
    uint8_t keystate=128;
    uint8_t *buf_p;
    int i, ret;

    if ((ret = ff_alloc_packet2(avctx, pkt, avctx->width*avctx->height*((8*2+1+1)*4)/8
                                  + FF_MIN_BUFFER_SIZE)) < 0)
        return ret;

    ff_init_range_encoder(c, pkt->data, pkt->size);
    ff_build_rac_states(c, 0.05*(1LL<<32), 256-8);

    *p = *pict;
    p->pict_type= AV_PICTURE_TYPE_I;

    if(avctx->gop_size==0 || f->picture_number % avctx->gop_size == 0){
        put_rac(c, &keystate, 1);
        p->key_frame= 1;
        f->gob_count++;
        write_header(f);
    }else{
        put_rac(c, &keystate, 0);
        p->key_frame= 0;
    }

    if (f->ac>1){
        int i;
        for(i=1; i<256; i++){
            c->one_state[i]= f->state_transition[i];
            c->zero_state[256-i]= 256-c->one_state[i];
        }
    }

    for(i=1; i<f->slice_count; i++){
        FFV1Context *fs= f->slice_context[i];
        uint8_t *start = pkt->data + (pkt->size-used_count)*i/f->slice_count;
        int len = pkt->size/f->slice_count;

        ff_init_range_encoder(&fs->c, start, len);
    }
    avctx->execute(avctx, encode_slice, &f->slice_context[0], NULL, f->slice_count, sizeof(void*));

    buf_p = pkt->data;
    for(i=0; i<f->slice_count; i++){
        FFV1Context *fs= f->slice_context[i];
        int bytes;

        if(fs->ac){
            uint8_t state=129;
            put_rac(&fs->c, &state, 0);
            bytes= ff_rac_terminate(&fs->c);
        }else{
            flush_put_bits(&fs->pb); //nicer padding FIXME
            bytes= fs->ac_byte_count + (put_bits_count(&fs->pb)+7)/8;
        }
        if(i>0 || f->version>2){
            av_assert0(bytes < pkt->size/f->slice_count);
            memmove(buf_p, fs->c.bytestream_start, bytes);
            av_assert0(bytes < (1<<24));
            AV_WB24(buf_p+bytes, bytes);
            bytes+=3;
        }
        if(f->ec){
            unsigned v;
            buf_p[bytes++] = 0;
            v = av_crc(av_crc_get_table(AV_CRC_32_IEEE), 0, buf_p, bytes);
            AV_WL32(buf_p + bytes, v); bytes += 4;
        }
        buf_p += bytes;
    }

    if((avctx->flags&CODEC_FLAG_PASS1) && (f->picture_number&31)==0){
        int j, k, m;
        char *p= avctx->stats_out;
        char *end= p + STATS_OUT_SIZE;

        memset(f->rc_stat, 0, sizeof(f->rc_stat));
        for(i=0; i<f->quant_table_count; i++)
            memset(f->rc_stat2[i], 0, f->context_count[i]*sizeof(*f->rc_stat2[i]));

        for(j=0; j<f->slice_count; j++){
            FFV1Context *fs= f->slice_context[j];
            for(i=0; i<256; i++){
                f->rc_stat[i][0] += fs->rc_stat[i][0];
                f->rc_stat[i][1] += fs->rc_stat[i][1];
            }
            for(i=0; i<f->quant_table_count; i++){
                for(k=0; k<f->context_count[i]; k++){
                    for(m=0; m<32; m++){
                        f->rc_stat2[i][k][m][0] += fs->rc_stat2[i][k][m][0];
                        f->rc_stat2[i][k][m][1] += fs->rc_stat2[i][k][m][1];
                    }
                }
            }
        }

        for(j=0; j<256; j++){
            snprintf(p, end-p, "%"PRIu64" %"PRIu64" ", f->rc_stat[j][0], f->rc_stat[j][1]);
            p+= strlen(p);
        }
        snprintf(p, end-p, "\n");

        for(i=0; i<f->quant_table_count; i++){
            for(j=0; j<f->context_count[i]; j++){
                for(m=0; m<32; m++){
                    snprintf(p, end-p, "%"PRIu64" %"PRIu64" ", f->rc_stat2[i][j][m][0], f->rc_stat2[i][j][m][1]);
                    p+= strlen(p);
                }
            }
        }
        snprintf(p, end-p, "%d\n", f->gob_count);
    } else if(avctx->flags&CODEC_FLAG_PASS1)
        avctx->stats_out[0] = '\0';

    f->picture_number++;
    pkt->size   = buf_p - pkt->data;
    pkt->flags |= AV_PKT_FLAG_KEY*p->key_frame;
    *got_packet = 1;

    return 0;
}
