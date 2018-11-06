static int alac_set_info(ALACContext *alac)
{
    GetByteContext gb;

    bytestream2_init(&gb, alac->avctx->extradata,
                     alac->avctx->extradata_size);

    bytestream2_skipu(&gb, 12); // size:4, alac:4, version:4

    alac->max_samples_per_frame = bytestream2_get_be32u(&gb);
    if (!alac->max_samples_per_frame || alac->max_samples_per_frame > INT_MAX) {
        av_log(alac->avctx, AV_LOG_ERROR, "max samples per frame invalid: %u\n",
               alac->max_samples_per_frame);
        return AVERROR_INVALIDDATA;
    }
    bytestream2_skipu(&gb, 1);  // compatible version
    alac->sample_size          = bytestream2_get_byteu(&gb);
    alac->rice_history_mult    = bytestream2_get_byteu(&gb);
    alac->rice_initial_history = bytestream2_get_byteu(&gb);
    alac->rice_limit           = bytestream2_get_byteu(&gb);
    alac->channels             = bytestream2_get_byteu(&gb);
    bytestream2_get_be16u(&gb); // maxRun
    bytestream2_get_be32u(&gb); // max coded frame size
    bytestream2_get_be32u(&gb); // average bitrate
    bytestream2_get_be32u(&gb); // samplerate

    return 0;
}
