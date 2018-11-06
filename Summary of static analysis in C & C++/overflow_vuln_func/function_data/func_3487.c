static int get_avc_nalsize(H264Context *h, const uint8_t *buf,
                           int buf_size, int *buf_index)
{
    int i, nalsize = 0;

    if (*buf_index >= buf_size - h->nal_length_size)
        return -1;

    for (i = 0; i < h->nal_length_size; i++)
        nalsize = (nalsize << 8) | buf[(*buf_index)++];
    if (nalsize <= 0 || nalsize > buf_size - *buf_index) {
        av_log(h->avctx, AV_LOG_ERROR,
               "AVC: nal size %d\n", nalsize);
        return -1;
    }
    return nalsize;
}
