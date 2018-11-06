static AVFrame *do_psnr(AVFilterContext *ctx, AVFrame *main,
                        const AVFrame *ref)
{
    PSNRContext *s = ctx->priv;
    double comp_mse[4], mse = 0;
    int j, c;
    AVDictionary **metadata = avpriv_frame_get_metadatap(main);

    s->compute_mse(s, (const uint8_t **)main->data, main->linesize,
                      (const uint8_t **)ref->data, ref->linesize,
                       main->width, main->height, comp_mse);

    for (j = 0; j < s->nb_components; j++)
        mse += comp_mse[j] * s->planeweight[j];

    s->min_mse = FFMIN(s->min_mse, mse);
    s->max_mse = FFMAX(s->max_mse, mse);

    s->mse += mse;
    for (j = 0; j < s->nb_components; j++)
        s->mse_comp[j] += comp_mse[j];
    s->nb_frames++;

    for (j = 0; j < s->nb_components; j++) {
        c = s->is_rgb ? s->rgba_map[j] : j;
        set_meta(metadata, "lavfi.psnr.mse.", s->comps[j], comp_mse[c]);
        set_meta(metadata, "lavfi.psnr.psnr.", s->comps[j], get_psnr(comp_mse[c], 1, s->max[c]));
    }
    set_meta(metadata, "lavfi.psnr.mse_avg", 0, mse);
    set_meta(metadata, "lavfi.psnr.psnr_avg", 0, get_psnr(mse, 1, s->average_max));

    if (s->stats_file) {
        fprintf(s->stats_file, "n:%"PRId64" mse_avg:%0.2f ", s->nb_frames, mse);
        for (j = 0; j < s->nb_components; j++) {
            c = s->is_rgb ? s->rgba_map[j] : j;
            fprintf(s->stats_file, "mse_%c:%0.2f ", s->comps[j], comp_mse[c]);
        }
        for (j = 0; j < s->nb_components; j++) {
            c = s->is_rgb ? s->rgba_map[j] : j;
            fprintf(s->stats_file, "psnr_%c:%0.2f ", s->comps[j],
                    get_psnr(comp_mse[c], 1, s->max[c]));
        }
        fprintf(s->stats_file, "\n");
    }

    return main;
}
