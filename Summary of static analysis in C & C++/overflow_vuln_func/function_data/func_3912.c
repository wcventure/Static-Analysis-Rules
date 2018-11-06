static int libopenjpeg_copy_unpacked16(AVCodecContext *avctx, const AVFrame *frame, opj_image_t *image)
{
    int compno;
    int x;
    int y;
    int width;
    int height;
    int *image_line;
    int frame_index;
    const int numcomps = image->numcomps;
    uint16_t *frame_ptr;

    for (compno = 0; compno < numcomps; ++compno) {
        if (image->comps[compno].w > frame->linesize[compno]) {
            av_log(avctx, AV_LOG_ERROR, "Error: frame's linesize is too small for the image\n");
            return 0;
        }
    }

    for (compno = 0; compno < numcomps; ++compno) {
        width     = avctx->width / image->comps[compno].dx;
        height    = avctx->height / image->comps[compno].dy;
        frame_ptr = (uint16_t *)frame->data[compno];
        for (y = 0; y < height; ++y) {
            image_line = image->comps[compno].data + y * image->comps[compno].w;
            frame_index = y * (frame->linesize[compno] / 2);
            for (x = 0; x < width; ++x)
                image_line[x] = frame_ptr[frame_index++];
            for (; x < image->comps[compno].w; ++x) {
                image_line[x] = image_line[x - 1];
            }
        }
        for (; y < image->comps[compno].h; ++y) {
            image_line = image->comps[compno].data + y * image->comps[compno].w;
            for (x = 0; x < image->comps[compno].w; ++x) {
                image_line[x] = image_line[x - image->comps[compno].w];
            }
        }
    }

    return 1;
}
