static void encode_cblk(Jpeg2000EncoderContext *s, Jpeg2000T1Context *t1, Jpeg2000Cblk *cblk, Jpeg2000Tile *tile,
                        int width, int height, int bandpos, int lev)
{
    int pass_t = 2, passno, x, y, max=0, nmsedec, bpno;
    int64_t wmsedec = 0;

    memset(t1->flags, 0, t1->stride * (height + 2) * sizeof(*t1->flags));

    for (y = 0; y < height; y++){
        for (x = 0; x < width; x++){
            if (t1->data[(y) * t1->stride + x] < 0){
                t1->flags[(y+1) * t1->stride + x+1] |= JPEG2000_T1_SGN;
                t1->data[(y) * t1->stride + x] = -t1->data[(y) * t1->stride + x];
            }
            max = FFMAX(max, t1->data[(y) * t1->stride + x]);
        }
    }

    if (max == 0){
        cblk->nonzerobits = 0;
        bpno = 0;
    } else{
        cblk->nonzerobits = av_log2(max) + 1 - NMSEDEC_FRACBITS;
        bpno = cblk->nonzerobits - 1;
    }

    cblk->data[0] = 0;
    ff_mqc_initenc(&t1->mqc, cblk->data + 1);

    for (passno = 0; bpno >= 0; passno++){
        nmsedec=0;

        switch(pass_t){
            case 0: encode_sigpass(t1, width, height, bandpos, &nmsedec, bpno);
                    break;
            case 1: encode_refpass(t1, width, height, &nmsedec, bpno);
                    break;
            case 2: encode_clnpass(t1, width, height, bandpos, &nmsedec, bpno);
                    break;
        }

        cblk->passes[passno].rate = ff_mqc_flush_to(&t1->mqc, cblk->passes[passno].flushed, &cblk->passes[passno].flushed_len);
        wmsedec += (int64_t)nmsedec << (2*bpno);
        cblk->passes[passno].disto = wmsedec;

        if (++pass_t == 3){
            pass_t = 0;
            bpno--;
        }
    }
    cblk->npasses = passno;
    cblk->ninclpasses = passno;

    cblk->passes[passno-1].rate = ff_mqc_flush_to(&t1->mqc, cblk->passes[passno-1].flushed, &cblk->passes[passno-1].flushed_len);
}
