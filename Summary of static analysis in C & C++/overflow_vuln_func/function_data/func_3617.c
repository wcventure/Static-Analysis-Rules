static int adpcm_decode_frame(AVCodecContext *avctx,
                            void *data, int *data_size,
                            uint8_t *buf, int buf_size)
{
    ADPCMContext *c = avctx->priv_data;
    ADPCMChannelStatus *cs;
    int n, m, channel, i;
    int block_predictor[2];
    short *samples;
    short *samples_end;
    uint8_t *src;
    int st; /

    /
    unsigned char last_byte = 0;
    unsigned char nibble;
    int decode_top_nibble_next = 0;
    int diff_channel;

    /
    uint32_t samples_in_chunk;
    int32_t previous_left_sample, previous_right_sample;
    int32_t current_left_sample, current_right_sample;
    int32_t next_left_sample, next_right_sample;
    int32_t coeff1l, coeff2l, coeff1r, coeff2r;
    uint8_t shift_left, shift_right;
    int count1, count2;

    if (!buf_size)
        return 0;

    //should protect all 4bit ADPCM variants
    //8 is needed for CODEC_ID_ADPCM_IMA_WAV with 2 channels
    //
    if(*data_size/4 < buf_size + 8)
        return -1;

    samples = data;
    samples_end= samples + *data_size/2;
    *data_size= 0;
    src = buf;

    st = avctx->channels == 2 ? 1 : 0;

    switch(avctx->codec->id) {
    case CODEC_ID_ADPCM_IMA_QT:
        n = (buf_size - 2);/
        channel = c->channel;
        cs = &(c->status[channel]);
        /

        /
        cs->predictor = (*src++) << 8;
        cs->predictor |= (*src & 0x80);
        cs->predictor &= 0xFF80;

        /
        if(cs->predictor & 0x8000)
            cs->predictor -= 0x10000;

        CLAMP_TO_SHORT(cs->predictor);

        cs->step_index = (*src++) & 0x7F;

        if (cs->step_index > 88){
            av_log(avctx, AV_LOG_ERROR, "ERROR: step_index = %i\n", cs->step_index);
            cs->step_index = 88;
        }

        cs->step = step_table[cs->step_index];

        if (st && channel)
            samples++;

        for(m=32; n>0 && m>0; n--, m--) { /
            *samples = adpcm_ima_expand_nibble(cs, src[0] & 0x0F, 3);
            samples += avctx->channels;
            *samples = adpcm_ima_expand_nibble(cs, (src[0] >> 4) & 0x0F, 3);
            samples += avctx->channels;
            src ++;
        }

        if(st) { /
            c->channel = (channel + 1) % 2; /
            if(channel == 1) { /
                return src - buf;
            }
        }
        break;
    case CODEC_ID_ADPCM_IMA_WAV:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

//        samples_per_block= (block_align-4*chanels)*8 / (bits_per_sample * chanels) + 1;

        for(i=0; i<avctx->channels; i++){
            cs = &(c->status[i]);
            cs->predictor = (int16_t)(src[0] + (src[1]<<8));
            src+=2;

        // XXX: is this correct ??: *samples++ = cs->predictor;

            cs->step_index = *src++;
            if (cs->step_index > 88){
                av_log(avctx, AV_LOG_ERROR, "ERROR: step_index = %i\n", cs->step_index);
                cs->step_index = 88;
            }
            if (*src++) av_log(avctx, AV_LOG_ERROR, "unused byte should be null but is %d!!\n", src[-1]); /
        }

        while(src < buf + buf_size){
            for(m=0; m<4; m++){
                for(i=0; i<=st; i++)
                    *samples++ = adpcm_ima_expand_nibble(&c->status[i], src[4*i] & 0x0F, 3);
                for(i=0; i<=st; i++)
                    *samples++ = adpcm_ima_expand_nibble(&c->status[i], src[4*i] >> 4  , 3);
                src++;
            }
            src += 4*st;
        }
        break;
    case CODEC_ID_ADPCM_4XM:
        cs = &(c->status[0]);
        c->status[0].predictor= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        if(st){
            c->status[1].predictor= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        }
        c->status[0].step_index= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        if(st){
            c->status[1].step_index= (int16_t)(src[0] + (src[1]<<8)); src+=2;
        }
        if (cs->step_index < 0) cs->step_index = 0;
        if (cs->step_index > 88) cs->step_index = 88;

        m= (buf_size - (src - buf))>>st;
        for(i=0; i<m; i++) {
            *samples++ = adpcm_ima_expand_nibble(&c->status[0], src[i] & 0x0F, 4);
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], src[i+m] & 0x0F, 4);
            *samples++ = adpcm_ima_expand_nibble(&c->status[0], src[i] >> 4, 4);
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1], src[i+m] >> 4, 4);
        }

        src += m<<st;

        break;
    case CODEC_ID_ADPCM_MS:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;
        n = buf_size - 7 * avctx->channels;
        if (n < 0)
            return -1;
        block_predictor[0] = av_clip(*src++, 0, 7);
        block_predictor[1] = 0;
        if (st)
            block_predictor[1] = av_clip(*src++, 0, 7);
        c->status[0].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st){
            c->status[1].idelta = (int16_t)((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
            src+=2;
        }
        c->status[0].coeff1 = AdaptCoeff1[block_predictor[0]];
        c->status[0].coeff2 = AdaptCoeff2[block_predictor[0]];
        c->status[1].coeff1 = AdaptCoeff1[block_predictor[1]];
        c->status[1].coeff2 = AdaptCoeff2[block_predictor[1]];

        c->status[0].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st) c->status[1].sample1 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        if (st) src+=2;
        c->status[0].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        src+=2;
        if (st) c->status[1].sample2 = ((*src & 0xFF) | ((src[1] << 8) & 0xFF00));
        if (st) src+=2;

        *samples++ = c->status[0].sample1;
        if (st) *samples++ = c->status[1].sample1;
        *samples++ = c->status[0].sample2;
        if (st) *samples++ = c->status[1].sample2;
        for(;n>0;n--) {
            *samples++ = adpcm_ms_expand_nibble(&c->status[0], (src[0] >> 4) & 0x0F);
            *samples++ = adpcm_ms_expand_nibble(&c->status[st], src[0] & 0x0F);
            src ++;
        }
        break;
    case CODEC_ID_ADPCM_IMA_DK4:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

        c->status[0].predictor = (int16_t)(src[0] | (src[1] << 8));
        c->status[0].step_index = src[2];
        src += 4;
        *samples++ = c->status[0].predictor;
        if (st) {
            c->status[1].predictor = (int16_t)(src[0] | (src[1] << 8));
            c->status[1].step_index = src[2];
            src += 4;
            *samples++ = c->status[1].predictor;
        }
        while (src < buf + buf_size) {

            /
            *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                (src[0] >> 4) & 0x0F, 3);

            /
            if (st)
                *samples++ = adpcm_ima_expand_nibble(&c->status[1],
                    src[0] & 0x0F, 3);
            else
                *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                    src[0] & 0x0F, 3);

            src++;
        }
        break;
    case CODEC_ID_ADPCM_IMA_DK3:
        if (avctx->block_align != 0 && buf_size > avctx->block_align)
            buf_size = avctx->block_align;

        if(buf_size + 16 > (samples_end - samples)*3/8)
            return -1;

        c->status[0].predictor = (int16_t)(src[10] | (src[11] << 8));
        c->status[1].predictor = (int16_t)(src[12] | (src[13] << 8));
        c->status[0].step_index = src[14];
        c->status[1].step_index = src[15];
        /
        src += 16;
        diff_channel = c->status[1].predictor;

        /
        while (1) {

            /

            /
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[0], nibble, 3);

            /
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[1], nibble, 3);

            /
            diff_channel = (diff_channel + c->status[1].predictor) / 2;
            *samples++ = c->status[0].predictor + c->status[1].predictor;
            *samples++ = c->status[0].predictor - c->status[1].predictor;

            /
            DK3_GET_NEXT_NIBBLE();
            adpcm_ima_expand_nibble(&c->status[0], nibble, 3);

            /
            diff_channel = (diff_channel + c->status[1].predictor) / 2;
            *samples++ = c->status[0].predictor + c->status[1].predictor;
            *samples++ = c->status[0].predictor - c->status[1].predictor;
        }
        break;
    case CODEC_ID_ADPCM_IMA_WS:
        /
        while (src < buf + buf_size) {

            if (st) {
                *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                    (src[0] >> 4) & 0x0F, 3);
                *samples++ = adpcm_ima_expand_nibble(&c->status[1],
                    src[0] & 0x0F, 3);
            } else {
                *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                    (src[0] >> 4) & 0x0F, 3);
                *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                    src[0] & 0x0F, 3);
            }

            src++;
        }
        break;
    case CODEC_ID_ADPCM_XA:
        c->status[0].sample1 = c->status[0].sample2 =
        c->status[1].sample1 = c->status[1].sample2 = 0;
        while (buf_size >= 128) {
            xa_decode(samples, src, &c->status[0], &c->status[1],
                avctx->channels);
            src += 128;
            samples += 28 * 8;
            buf_size -= 128;
        }
        break;
    case CODEC_ID_ADPCM_EA:
        samples_in_chunk = AV_RL32(src);
        if (samples_in_chunk >= ((buf_size - 12) * 2)) {
            src += buf_size;
            break;
        }
        src += 4;
        current_left_sample = (int16_t)AV_RL16(src);
        src += 2;
        previous_left_sample = (int16_t)AV_RL16(src);
        src += 2;
        current_right_sample = (int16_t)AV_RL16(src);
        src += 2;
        previous_right_sample = (int16_t)AV_RL16(src);
        src += 2;

        for (count1 = 0; count1 < samples_in_chunk/28;count1++) {
            coeff1l = ea_adpcm_table[(*src >> 4) & 0x0F];
            coeff2l = ea_adpcm_table[((*src >> 4) & 0x0F) + 4];
            coeff1r = ea_adpcm_table[*src & 0x0F];
            coeff2r = ea_adpcm_table[(*src & 0x0F) + 4];
            src++;

            shift_left = ((*src >> 4) & 0x0F) + 8;
            shift_right = (*src & 0x0F) + 8;
            src++;

            for (count2 = 0; count2 < 28; count2++) {
                next_left_sample = (((*src & 0xF0) << 24) >> shift_left);
                next_right_sample = (((*src & 0x0F) << 28) >> shift_right);
                src++;

                next_left_sample = (next_left_sample +
                    (current_left_sample * coeff1l) +
                    (previous_left_sample * coeff2l) + 0x80) >> 8;
                next_right_sample = (next_right_sample +
                    (current_right_sample * coeff1r) +
                    (previous_right_sample * coeff2r) + 0x80) >> 8;
                CLAMP_TO_SHORT(next_left_sample);
                CLAMP_TO_SHORT(next_right_sample);

                previous_left_sample = current_left_sample;
                current_left_sample = next_left_sample;
                previous_right_sample = current_right_sample;
                current_right_sample = next_right_sample;
                *samples++ = (unsigned short)current_left_sample;
                *samples++ = (unsigned short)current_right_sample;
            }
        }
        break;
    case CODEC_ID_ADPCM_IMA_SMJPEG:
        c->status[0].predictor = *src;
        src += 2;
        c->status[0].step_index = *src++;
        src++;  /
        while (src < buf + buf_size) {
            *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                *src & 0x0F, 3);
            *samples++ = adpcm_ima_expand_nibble(&c->status[0],
                (*src >> 4) & 0x0F, 3);
            src++;
        }
        break;
    case CODEC_ID_ADPCM_CT:
        while (src < buf + buf_size) {
            if (st) {
                *samples++ = adpcm_ct_expand_nibble(&c->status[0],
                    (src[0] >> 4) & 0x0F);
                *samples++ = adpcm_ct_expand_nibble(&c->status[1],
                    src[0] & 0x0F);
            } else {
                *samples++ = adpcm_ct_expand_nibble(&c->status[0],
                    (src[0] >> 4) & 0x0F);
                *samples++ = adpcm_ct_expand_nibble(&c->status[0],
                    src[0] & 0x0F);
            }
            src++;
        }
        break;
    case CODEC_ID_ADPCM_SBPRO_4:
    case CODEC_ID_ADPCM_SBPRO_3:
    case CODEC_ID_ADPCM_SBPRO_2:
        if (!c->status[0].step_index) {
            /
            *samples++ = 128 * (*src++ - 0x80);
            if (st)
              *samples++ = 128 * (*src++ - 0x80);
            c->status[0].step_index = 1;
        }
        if (avctx->codec->id == CODEC_ID_ADPCM_SBPRO_4) {
            while (src < buf + buf_size) {
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    (src[0] >> 4) & 0x0F, 4, 0);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[st],
                    src[0] & 0x0F, 4, 0);
                src++;
            }
        } else if (avctx->codec->id == CODEC_ID_ADPCM_SBPRO_3) {
            while (src < buf + buf_size && samples + 2 < samples_end) {
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    (src[0] >> 5) & 0x07, 3, 0);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    (src[0] >> 2) & 0x07, 3, 0);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    src[0] & 0x03, 2, 0);
                src++;
            }
        } else {
            while (src < buf + buf_size && samples + 3 < samples_end) {
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    (src[0] >> 6) & 0x03, 2, 2);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[st],
                    (src[0] >> 4) & 0x03, 2, 2);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[0],
                    (src[0] >> 2) & 0x03, 2, 2);
                *samples++ = adpcm_sbpro_expand_nibble(&c->status[st],
                    src[0] & 0x03, 2, 2);
                src++;
            }
        }
        break;
    case CODEC_ID_ADPCM_SWF:
    {
        GetBitContext gb;
        const int *table;
        int k0, signmask, nb_bits;
        int size = buf_size*8;

        init_get_bits(&gb, buf, size);

        //read bits & inital values
        nb_bits = get_bits(&gb, 2)+2;
        //av_log(NULL,AV_LOG_INFO,"nb_bits: %d\n", nb_bits);
        table = swf_index_tables[nb_bits-2];
        k0 = 1 << (nb_bits-2);
        signmask = 1 << (nb_bits-1);

        for (i = 0; i < avctx->channels; i++) {
            *samples++ = c->status[i].predictor = get_sbits(&gb, 16);
            c->status[i].step_index = get_bits(&gb, 6);
        }

        while (get_bits_count(&gb) < size)
        {
            int i;

            for (i = 0; i < avctx->channels; i++) {
                // similar to IMA adpcm
                int delta = get_bits(&gb, nb_bits);
                int step = step_table[c->status[i].step_index];
                long vpdiff = 0; // vpdiff = (delta+0.5)*step/4
                int k = k0;

                do {
                    if (delta & k)
                        vpdiff += step;
                    step >>= 1;
                    k >>= 1;
                } while(k);
                vpdiff += step;

                if (delta & signmask)
                    c->status[i].predictor -= vpdiff;
                else
                    c->status[i].predictor += vpdiff;

                c->status[i].step_index += table[delta & (~signmask)];

                c->status[i].step_index = av_clip(c->status[i].step_index, 0, 88);
                c->status[i].predictor = av_clip(c->status[i].predictor, -32768, 32767);

                *samples++ = c->status[i].predictor;
                if (samples >= samples_end) {
                    av_log(avctx, AV_LOG_ERROR, "allocated output buffer is too small\n");
                    return -1;
                }
            }
        }
        src += buf_size;
        break;
    }
    case CODEC_ID_ADPCM_YAMAHA:
        while (src < buf + buf_size) {
            if (st) {
                *samples++ = adpcm_yamaha_expand_nibble(&c->status[0],
                        src[0] & 0x0F);
                *samples++ = adpcm_yamaha_expand_nibble(&c->status[1],
                        (src[0] >> 4) & 0x0F);
            } else {
                *samples++ = adpcm_yamaha_expand_nibble(&c->status[0],
                        src[0] & 0x0F);
                *samples++ = adpcm_yamaha_expand_nibble(&c->status[0],
                        (src[0] >> 4) & 0x0F);
            }
            src++;
        }
        break;
    case CODEC_ID_ADPCM_THP:
    {
        int table[2][16];
        unsigned int samplecnt;
        int prev[2][2];
        int ch;

        if (buf_size < 80) {
            av_log(avctx, AV_LOG_ERROR, "frame too small\n");
            return -1;
        }

        src+=4;
        samplecnt = bytestream_get_be32(&src);

        for (i = 0; i < 32; i++)
            table[0][i] = (int16_t)bytestream_get_be16(&src);

        /
        for (i = 0; i < 4; i++)
            prev[0][i] = (int16_t)bytestream_get_be16(&src);

        if (samplecnt >= (samples_end - samples) /  (st + 1)) {
            av_log(avctx, AV_LOG_ERROR, "allocated output buffer is too small\n");
            return -1;
        }

        for (ch = 0; ch <= st; ch++) {
            samples = (unsigned short *) data + ch;

            /
            for (i = 0; i < samplecnt / 14; i++) {
                int index = (*src >> 4) & 7;
                unsigned int exp = 28 - (*src++ & 15);
                int factor1 = table[ch][index * 2];
                int factor2 = table[ch][index * 2 + 1];

                /
                for (n = 0; n < 14; n++) {
                    int32_t sampledat;
                    if(n&1) sampledat=  *src++    <<28;
                    else    sampledat= (*src&0xF0)<<24;

                    *samples = ((prev[ch][0]*factor1
                                + prev[ch][1]*factor2) >> 11) + (sampledat>>exp);
                    prev[ch][1] = prev[ch][0];
                    prev[ch][0] = *samples++;

                    /
                    samples += st;
                }
            }
        }

        /
        samples -= st;
        break;
    }

    default:
        return -1;
    }
    *data_size = (uint8_t *)samples - (uint8_t *)data;
    return src - buf;
}
