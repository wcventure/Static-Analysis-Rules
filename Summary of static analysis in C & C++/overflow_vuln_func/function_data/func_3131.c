int ff_h264_pred_weight_table(GetBitContext *gb, const SPS *sps,
                              const int *ref_count, int slice_type_nos,
                              H264PredWeightTable *pwt, void *logctx)
{
    int list, i, j;
    int luma_def, chroma_def;

    pwt->use_weight             = 0;
    pwt->use_weight_chroma      = 0;
    pwt->luma_log2_weight_denom = get_ue_golomb(gb);
    if (sps->chroma_format_idc)
        pwt->chroma_log2_weight_denom = get_ue_golomb(gb);

    if (pwt->luma_log2_weight_denom > 7U) {
        av_log(logctx, AV_LOG_ERROR, "luma_log2_weight_denom %d is out of range\n", pwt->luma_log2_weight_denom);
        pwt->luma_log2_weight_denom = 0;
    }
    if (pwt->chroma_log2_weight_denom > 7U) {
        av_log(logctx, AV_LOG_ERROR, "chroma_log2_weight_denom %d is out of range\n", pwt->chroma_log2_weight_denom);
        pwt->chroma_log2_weight_denom = 0;
    }

    luma_def   = 1 << pwt->luma_log2_weight_denom;
    chroma_def = 1 << pwt->chroma_log2_weight_denom;

    for (list = 0; list < 2; list++) {
        pwt->luma_weight_flag[list]   = 0;
        pwt->chroma_weight_flag[list] = 0;
        for (i = 0; i < ref_count[list]; i++) {
            int luma_weight_flag, chroma_weight_flag;

            luma_weight_flag = get_bits1(gb);
            if (luma_weight_flag) {
                pwt->luma_weight[i][list][0] = get_se_golomb(gb);
                pwt->luma_weight[i][list][1] = get_se_golomb(gb);
                if ((int8_t)pwt->luma_weight[i][list][0] != pwt->luma_weight[i][list][0] ||
                    (int8_t)pwt->luma_weight[i][list][1] != pwt->luma_weight[i][list][1])
                    goto out_range_weight;
                if (pwt->luma_weight[i][list][0] != luma_def ||
                    pwt->luma_weight[i][list][1] != 0) {
                    pwt->use_weight             = 1;
                    pwt->luma_weight_flag[list] = 1;
                }
            } else {
                pwt->luma_weight[i][list][0] = luma_def;
                pwt->luma_weight[i][list][1] = 0;
            }

            if (sps->chroma_format_idc) {
                chroma_weight_flag = get_bits1(gb);
                if (chroma_weight_flag) {
                    int j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = get_se_golomb(gb);
                        pwt->chroma_weight[i][list][j][1] = get_se_golomb(gb);
                        if ((int8_t)pwt->chroma_weight[i][list][j][0] != pwt->chroma_weight[i][list][j][0] ||
                            (int8_t)pwt->chroma_weight[i][list][j][1] != pwt->chroma_weight[i][list][j][1])
                            goto out_range_weight;
                        if (pwt->chroma_weight[i][list][j][0] != chroma_def ||
                            pwt->chroma_weight[i][list][j][1] != 0) {
                            pwt->use_weight_chroma        = 1;
                            pwt->chroma_weight_flag[list] = 1;
                        }
                    }
                } else {
                    int j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = chroma_def;
                        pwt->chroma_weight[i][list][j][1] = 0;
                    }
                }
            }

            // for MBAFF
            pwt->luma_weight[16 + 2 * i][list][0] = pwt->luma_weight[16 + 2 * i + 1][list][0] = pwt->luma_weight[i][list][0];
            pwt->luma_weight[16 + 2 * i][list][1] = pwt->luma_weight[16 + 2 * i + 1][list][1] = pwt->luma_weight[i][list][1];
            for (j = 0; j < 2; j++) {
                pwt->chroma_weight[16 + 2 * i][list][j][0] = pwt->chroma_weight[16 + 2 * i + 1][list][j][0] = pwt->chroma_weight[i][list][j][0];
                pwt->chroma_weight[16 + 2 * i][list][j][1] = pwt->chroma_weight[16 + 2 * i + 1][list][j][1] = pwt->chroma_weight[i][list][j][1];
            }
        }
        if (slice_type_nos != AV_PICTURE_TYPE_B)
            break;
    }
    pwt->use_weight = pwt->use_weight || pwt->use_weight_chroma;
    return 0;
out_range_weight:
    avpriv_request_sample(logctx, "Out of range weight\n");
    return AVERROR_INVALIDDATA;
}
