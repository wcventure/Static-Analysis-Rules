static int hevc_decode_extradata(HEVCContext *s, uint8_t *buf, int length)
{
    int ret, i;

    ret = ff_hevc_decode_extradata(buf, length, &s->ps, &s->sei, &s->is_nalff,
                                   &s->nal_length_size, s->avctx->err_recognition,
                                   s->apply_defdispwin, s->avctx);
    if (ret < 0)
        return ret;

    /
    for (i = 0; i < FF_ARRAY_ELEMS(s->ps.sps_list); i++) {
        if (s->ps.sps_list[i]) {
            const HEVCSPS *sps = (const HEVCSPS*)s->ps.sps_list[i]->data;
            export_stream_params(s->avctx, &s->ps, sps);
            break;
        }
    }

    return 0;
}
