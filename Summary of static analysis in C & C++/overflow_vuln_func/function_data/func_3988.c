static void mclms_predict(WmallDecodeCtx *s, int icoef, int *pred)
{
    int ich, i;
    int order        = s->mclms_order;
    int num_channels = s->num_channels;

    for (ich = 0; ich < num_channels; ich++) {
        pred[ich] = 0;
        if (!s->is_channel_coded[ich])
            continue;
        for (i = 0; i < order * num_channels; i++)
            pred[ich] += s->mclms_prevvalues[i + s->mclms_recent] *
                         s->mclms_coeffs[i + order * num_channels * ich];
        for (i = 0; i < ich; i++)
            pred[ich] += s->channel_residues[i][icoef] *
                         s->mclms_coeffs_cur[i + num_channels * ich];
        pred[ich] += 1 << s->mclms_scaling - 1;
        pred[ich] >>= s->mclms_scaling;
        s->channel_residues[ich][icoef] += pred[ich];
    }
}
