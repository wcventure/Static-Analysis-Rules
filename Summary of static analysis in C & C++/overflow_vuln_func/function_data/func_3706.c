static int msrle_decode_8_16_24_32(AVCodecContext *avctx, AVPicture *pic, int depth,
                                    const uint8_t *data, int srcsize)
{
    uint8_t *output, *output_end;
    const uint8_t* src = data;
    int p1, p2, line=avctx->height - 1, pos=0, i;
    uint16_t av_uninit(pix16);
    uint32_t av_uninit(pix32);

    output = pic->data[0] + (avctx->height - 1) * pic->linesize[0];
    output_end = pic->data[0] + (avctx->height) * pic->linesize[0];
    while(src < data + srcsize) {
        p1 = *src++;
        if(p1 == 0) { //Escape code
            p2 = *src++;
            if(p2 == 0) { //End-of-line
                output = pic->data[0] + (--line) * pic->linesize[0];
                if (line < 0 && !(src+1 < data + srcsize && AV_RB16(src) == 1)) {
                    av_log(avctx, AV_LOG_ERROR, "Next line is beyond picture bounds\n");
                    return -1;
                }
                pos = 0;
                continue;
            } else if(p2 == 1) { //End-of-picture
                return 0;
            } else if(p2 == 2) { //Skip
                p1 = *src++;
                p2 = *src++;
                line -= p2;
                if (line < 0){
                    av_log(avctx, AV_LOG_ERROR, "Skip beyond picture bounds\n");
                    return -1;
                }
                pos += p1;
                output = pic->data[0] + line * pic->linesize[0] + pos * (depth >> 3);
                continue;
            }
            // Copy data
            if ((pic->linesize[0] > 0 && output + p2 * (depth >> 3) > output_end)
              ||(pic->linesize[0] < 0 && output + p2 * (depth >> 3) < output_end)) {
                src += p2 * (depth >> 3);
                continue;
            }
            if ((depth == 8) || (depth == 24)) {
                for(i = 0; i < p2 * (depth >> 3); i++) {
                    *output++ = *src++;
                }
                // RLE8 copy is actually padded - and runs are not!
                if(depth == 8 && (p2 & 1)) {
                    src++;
                }
            } else if (depth == 16) {
                for(i = 0; i < p2; i++) {
                    pix16 = AV_RL16(src);
                    src += 2;
                    *(uint16_t*)output = pix16;
                    output += 2;
                }
            } else if (depth == 32) {
                for(i = 0; i < p2; i++) {
                    pix32 = AV_RL32(src);
                    src += 4;
                    *(uint32_t*)output = pix32;
                    output += 4;
                }
            }
            pos += p2;
        } else { //run of pixels
            uint8_t pix[3]; //original pixel
            switch(depth){
            case  8: pix[0] = *src++;
                     break;
            case 16: pix16 = AV_RL16(src);
                     src += 2;
                     break;
            case 24: pix[0] = *src++;
                     pix[1] = *src++;
                     pix[2] = *src++;
                     break;
            case 32: pix32 = AV_RL32(src);
                     src += 4;
                     break;
            }
            if ((pic->linesize[0] > 0 && output + p1 * (depth >> 3) > output_end)
              ||(pic->linesize[0] < 0 && output + p1 * (depth >> 3) < output_end))
                continue;
            for(i = 0; i < p1; i++) {
                switch(depth){
                case  8: *output++ = pix[0];
                         break;
                case 16: *(uint16_t*)output = pix16;
                         output += 2;
                         break;
                case 24: *output++ = pix[0];
                         *output++ = pix[1];
                         *output++ = pix[2];
                         break;
                case 32: *(uint32_t*)output = pix32;
                         output += 4;
                         break;
                }
            }
            pos += p1;
        }
    }

    av_log(avctx, AV_LOG_WARNING, "MS RLE warning: no end-of-picture code\n");
    return 0;
}
