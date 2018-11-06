int ff_h264_decode_ref_pic_list_reordering(H264Context *h, H264SliceContext *sl)
{
    int list, index, pic_structure;

    print_short_term(h);
    print_long_term(h);

    h264_initialise_ref_list(h, sl);

    for (list = 0; list < sl->list_count; list++) {
        if (get_bits1(&sl->gb)) {    // ref_pic_list_modification_flag_l[01]
            int pred = h->curr_pic_num;

            for (index = 0; ; index++) {
                unsigned int modification_of_pic_nums_idc = get_ue_golomb_31(&sl->gb);
                unsigned int pic_id;
                int i;
                H264Picture *ref = NULL;

                if (modification_of_pic_nums_idc == 3)
                    break;

                if (index >= sl->ref_count[list]) {
                    av_log(h->avctx, AV_LOG_ERROR, "reference count overflow\n");
                    return -1;
                }

                switch (modification_of_pic_nums_idc) {
                case 0:
                case 1: {
                    const unsigned int abs_diff_pic_num = get_ue_golomb(&sl->gb) + 1;
                    int frame_num;

                    if (abs_diff_pic_num > h->max_pic_num) {
                        av_log(h->avctx, AV_LOG_ERROR,
                               "abs_diff_pic_num overflow\n");
                        return AVERROR_INVALIDDATA;
                    }

                    if (modification_of_pic_nums_idc == 0)
                        pred -= abs_diff_pic_num;
                    else
                        pred += abs_diff_pic_num;
                    pred &= h->max_pic_num - 1;

                    frame_num = pic_num_extract(h, pred, &pic_structure);

                    for (i = h->short_ref_count - 1; i >= 0; i--) {
                        ref = h->short_ref[i];
                        assert(ref->reference);
                        assert(!ref->long_ref);
                        if (ref->frame_num == frame_num &&
                            (ref->reference & pic_structure))
                            break;
                    }
                    if (i >= 0)
                        ref->pic_id = pred;
                    break;
                }
                case 2: {
                    int long_idx;
                    pic_id = get_ue_golomb(&sl->gb); // long_term_pic_idx

                    long_idx = pic_num_extract(h, pic_id, &pic_structure);

                    if (long_idx > 31) {
                        av_log(h->avctx, AV_LOG_ERROR,
                               "long_term_pic_idx overflow\n");
                        return AVERROR_INVALIDDATA;
                    }
                    ref = h->long_ref[long_idx];
                    assert(!(ref && !ref->reference));
                    if (ref && (ref->reference & pic_structure) && !mismatches_ref(h, ref)) {
                        ref->pic_id = pic_id;
                        assert(ref->long_ref);
                        i = 0;
                    } else {
                        i = -1;
                    }
                    break;
                }
                default:
                    av_log(h->avctx, AV_LOG_ERROR,
                           "illegal modification_of_pic_nums_idc %u\n",
                           modification_of_pic_nums_idc);
                    return AVERROR_INVALIDDATA;
                }

                if (i < 0) {
                    av_log(h->avctx, AV_LOG_ERROR,
                           "reference picture missing during reorder\n");
                    memset(&sl->ref_list[list][index], 0, sizeof(sl->ref_list[0][0])); // FIXME
                } else {
                    for (i = index; i + 1 < sl->ref_count[list]; i++) {
                        if (sl->ref_list[list][i].parent &&
                            ref->long_ref == sl->ref_list[list][i].parent->long_ref &&
                            ref->pic_id   == sl->ref_list[list][i].pic_id)
                            break;
                    }
                    for (; i > index; i--) {
                        sl->ref_list[list][i] = sl->ref_list[list][i - 1];
                    }
                    ref_from_h264pic(&sl->ref_list[list][index], ref);
                    if (FIELD_PICTURE(h)) {
                        pic_as_field(&sl->ref_list[list][index], pic_structure);
                    }
                }
            }
        }
    }
    for (list = 0; list < sl->list_count; list++) {
        for (index = 0; index < sl->ref_count[list]; index++) {
            if (   !sl->ref_list[list][index].parent
                || (!FIELD_PICTURE(h) && (sl->ref_list[list][index].reference&3) != 3)) {
                int i;
                av_log(h->avctx, AV_LOG_ERROR, "Missing reference picture\n");
                for (i = 0; i < FF_ARRAY_ELEMS(h->last_pocs); i++)
                    h->last_pocs[i] = INT_MIN;
                return -1;
            }
            av_assert0(av_buffer_get_ref_count(sl->ref_list[list][index].parent->f->buf[0]) > 0);
        }
    }

    return 0;
}
