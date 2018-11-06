static int dirac_decode_picture_header(DiracContext *s)
{
    int retire, picnum;
    int i, j, refnum, refdist;
    GetBitContext *gb = &s->gb;

    /
    picnum = s->current_picture->avframe.display_picture_number = get_bits_long(gb, 32);


    av_log(s->avctx,AV_LOG_DEBUG,"PICTURE_NUM: %d\n",picnum);

    /
    if (s->frame_number < 0)
        s->frame_number = picnum;

    s->ref_pics[0] = s->ref_pics[1] = NULL;
    for (i = 0; i < s->num_refs; i++) {
        refnum = picnum + dirac_get_se_golomb(gb);
        refdist = INT_MAX;

        /
        /
        for (j = 0; j < MAX_REFERENCE_FRAMES && refdist; j++)
            if (s->ref_frames[j]
                && FFABS(s->ref_frames[j]->avframe.display_picture_number - refnum) < refdist) {
                s->ref_pics[i] = s->ref_frames[j];
                refdist = FFABS(s->ref_frames[j]->avframe.display_picture_number - refnum);
            }

        if (!s->ref_pics[i] || refdist)
            av_log(s->avctx, AV_LOG_DEBUG, "Reference not found\n");

        /
        if (!s->ref_pics[i])
            for (j = 0; j < MAX_FRAMES; j++)
                if (!s->all_frames[j].avframe.data[0]) {
                    s->ref_pics[i] = &s->all_frames[j];
                    s->avctx->get_buffer(s->avctx, &s->ref_pics[i]->avframe);
                }
    }

    /
    if (s->current_picture->avframe.reference) {
        retire = picnum + dirac_get_se_golomb(gb);
        if (retire != picnum) {
            DiracFrame *retire_pic = remove_frame(s->ref_frames, retire);

            if (retire_pic)
                retire_pic->avframe.reference &= DELAYED_PIC_REF;
            else
                av_log(s->avctx, AV_LOG_DEBUG, "Frame to retire not found\n");
        }

        /
        while (add_frame(s->ref_frames, MAX_REFERENCE_FRAMES, s->current_picture)) {
            av_log(s->avctx, AV_LOG_ERROR, "Reference frame overflow\n");
            remove_frame(s->ref_frames, s->ref_frames[0]->avframe.display_picture_number)->avframe.reference &= DELAYED_PIC_REF;
        }
    }

    if (s->num_refs) {
        if (dirac_unpack_prediction_parameters(s))  /
            return -1;
        dirac_unpack_block_motion_data(s);          /
    }
    if (dirac_unpack_idwt_params(s))                /
        return -1;

    init_planes(s);
    return 0;
}
