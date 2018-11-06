int ff_qsv_encode(AVCodecContext *avctx, QSVEncContext *q,
                  AVPacket *pkt, const AVFrame *frame, int *got_packet)
{
    mfxBitstream bs = { { { 0 } } };

    mfxFrameSurface1 *surf = NULL;
    mfxSyncPoint sync      = NULL;
    int ret;

    if (frame) {
        ret = submit_frame(q, frame, &surf);
        if (ret < 0) {
            av_log(avctx, AV_LOG_ERROR, "Error submitting the frame for encoding.\n");
            return ret;
        }
    }

    ret = ff_alloc_packet(pkt, q->packet_size);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "Error allocating the output packet\n");
        return ret;
    }
    bs.Data      = pkt->data;
    bs.MaxLength = pkt->size;

    do {
        ret = MFXVideoENCODE_EncodeFrameAsync(q->session, NULL, surf, &bs, &sync);
        if (ret == MFX_WRN_DEVICE_BUSY)
            av_usleep(1);
    } while (ret > 0);

    if (ret < 0)
        return (ret == MFX_ERR_MORE_DATA) ? 0 : ff_qsv_error(ret);

    if (ret == MFX_WRN_INCOMPATIBLE_VIDEO_PARAM && frame->interlaced_frame)
        print_interlace_msg(avctx, q);

    if (sync) {
        MFXVideoCORE_SyncOperation(q->session, sync, 60000);

        if (bs.FrameType & MFX_FRAMETYPE_I || bs.FrameType & MFX_FRAMETYPE_xI)
            avctx->coded_frame->pict_type = AV_PICTURE_TYPE_I;
        else if (bs.FrameType & MFX_FRAMETYPE_P || bs.FrameType & MFX_FRAMETYPE_xP)
            avctx->coded_frame->pict_type = AV_PICTURE_TYPE_P;
        else if (bs.FrameType & MFX_FRAMETYPE_B || bs.FrameType & MFX_FRAMETYPE_xB)
            avctx->coded_frame->pict_type = AV_PICTURE_TYPE_B;

        pkt->dts  = av_rescale_q(bs.DecodeTimeStamp, (AVRational){1, 90000}, avctx->time_base);
        pkt->pts  = av_rescale_q(bs.TimeStamp,       (AVRational){1, 90000}, avctx->time_base);
        pkt->size = bs.DataLength;

        if (bs.FrameType & MFX_FRAMETYPE_IDR ||
            bs.FrameType & MFX_FRAMETYPE_xIDR)
            pkt->flags |= AV_PKT_FLAG_KEY;

        *got_packet = 1;
    }

    return 0;
}
