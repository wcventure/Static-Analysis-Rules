static int alac_decode_frame(AVCodecContext *avctx,
                             void *outbuffer, int *outputsize,
                             const uint8_t *inbuffer, int input_buffer_size)
{
    ALACContext *alac = avctx->priv_data;

    int channels;
    int32_t outputsamples;
    int hassize;
    int readsamplesize;
    int wasted_bytes;
    int isnotcompressed;
    uint8_t interlacing_shift;
    uint8_t interlacing_leftweight;

    /
    if (!inbuffer || !input_buffer_size)
        return input_buffer_size;

    /
    if (!alac->context_initialized) {
        if (alac->avctx->extradata_size != ALAC_EXTRADATA_SIZE) {
            av_log(avctx, AV_LOG_ERROR, "alac: expected %d extradata bytes\n",
                ALAC_EXTRADATA_SIZE);
            return input_buffer_size;
        }
        if (alac_set_info(alac)) {
            av_log(avctx, AV_LOG_ERROR, "alac: set_info failed\n");
            return input_buffer_size;
        }
        alac->context_initialized = 1;
    }

    init_get_bits(&alac->gb, inbuffer, input_buffer_size * 8);

    channels = get_bits(&alac->gb, 3) + 1;
    if (channels > MAX_CHANNELS) {
        av_log(avctx, AV_LOG_ERROR, "channels > %d not supported\n",
               MAX_CHANNELS);
        return input_buffer_size;
    }

    /
    skip_bits(&alac->gb, 4);

    skip_bits(&alac->gb, 12); /

    /
    hassize = get_bits1(&alac->gb);

    wasted_bytes = get_bits(&alac->gb, 2); /

    /
    isnotcompressed = get_bits1(&alac->gb);

    if (hassize) {
        /
        outputsamples = get_bits(&alac->gb, 32);
    } else
        outputsamples = alac->setinfo_max_samples_per_frame;

    *outputsize = outputsamples * alac->bytespersample;
    readsamplesize = alac->setinfo_sample_size - (wasted_bytes * 8) + channels - 1;

    if (!isnotcompressed) {
        /
        int16_t predictor_coef_table[channels][32];
        int predictor_coef_num[channels];
        int prediction_type[channels];
        int prediction_quantitization[channels];
        int ricemodifier[channels];
        int i, chan;

        interlacing_shift = get_bits(&alac->gb, 8);
        interlacing_leftweight = get_bits(&alac->gb, 8);

        for (chan = 0; chan < channels; chan++) {
            prediction_type[chan] = get_bits(&alac->gb, 4);
            prediction_quantitization[chan] = get_bits(&alac->gb, 4);

            ricemodifier[chan] = get_bits(&alac->gb, 3);
            predictor_coef_num[chan] = get_bits(&alac->gb, 5);

            /
            for (i = 0; i < predictor_coef_num[chan]; i++)
                predictor_coef_table[chan][i] = (int16_t)get_bits(&alac->gb, 16);
        }

        if (wasted_bytes)
            av_log(avctx, AV_LOG_ERROR, "FIXME: unimplemented, unhandling of wasted_bytes\n");

        for (chan = 0; chan < channels; chan++) {
            bastardized_rice_decompress(alac,
                                        alac->predicterror_buffer[chan],
                                        outputsamples,
                                        readsamplesize,
                                        alac->setinfo_rice_initialhistory,
                                        alac->setinfo_rice_kmodifier,
                                        ricemodifier[chan] * alac->setinfo_rice_historymult / 4,
                                        (1 << alac->setinfo_rice_kmodifier) - 1);

            if (prediction_type[chan] == 0) {
                /
                predictor_decompress_fir_adapt(alac->predicterror_buffer[chan],
                                               alac->outputsamples_buffer[chan],
                                               outputsamples,
                                               readsamplesize,
                                               predictor_coef_table[chan],
                                               predictor_coef_num[chan],
                                               prediction_quantitization[chan]);
            } else {
                av_log(avctx, AV_LOG_ERROR, "FIXME: unhandled prediction type: %i\n", prediction_type[chan]);
                /
            }
        }
    } else {
        /
        if (alac->setinfo_sample_size <= 16) {
            int i, chan;
            for (chan = 0; chan < channels; chan++)
                for (i = 0; i < outputsamples; i++) {
                    int32_t audiobits;

                    audiobits = get_bits(&alac->gb, alac->setinfo_sample_size);
                    audiobits = extend_sign32(audiobits, readsamplesize);

                    alac->outputsamples_buffer[chan][i] = audiobits;
                }
        } else {
            int i, chan;
            for (chan = 0; chan < channels; chan++)
                for (i = 0; i < outputsamples; i++) {
                    int32_t audiobits;

                    audiobits = get_bits(&alac->gb, 16);
                    /
                    audiobits = audiobits << 16;
                    audiobits = audiobits >> (32 - alac->setinfo_sample_size);
                    audiobits |= get_bits(&alac->gb, alac->setinfo_sample_size - 16);

                    alac->outputsamples_buffer[chan][i] = audiobits;
                }
        }
        /
        interlacing_shift = 0;
        interlacing_leftweight = 0;
    }
    if (get_bits(&alac->gb, 3) != 7)
        av_log(avctx, AV_LOG_ERROR, "Error : Wrong End Of Frame\n");

    switch(alac->setinfo_sample_size) {
    case 16:
        if (channels == 2) {
            reconstruct_stereo_16(alac->outputsamples_buffer,
                                  (int16_t*)outbuffer,
                                  alac->numchannels,
                                  outputsamples,
                                  interlacing_shift,
                                  interlacing_leftweight);
        } else {
            int i;
            for (i = 0; i < outputsamples; i++) {
                int16_t sample = alac->outputsamples_buffer[0][i];
                ((int16_t*)outbuffer)[i * alac->numchannels] = sample;
            }
        }
        break;
    case 20:
    case 24:
        // It is not clear if there exist any encoder that creates 24 bit ALAC
        // files. iTunes convert 24 bit raw files to 16 bit before encoding.
    case 32:
        av_log(avctx, AV_LOG_ERROR, "FIXME: unimplemented sample size %i\n", alac->setinfo_sample_size);
        break;
    default:
        break;
    }

    if (input_buffer_size * 8 - get_bits_count(&alac->gb) > 8)
        av_log(avctx, AV_LOG_ERROR, "Error : %d bits left\n", input_buffer_size * 8 - get_bits_count(&alac->gb));

    return input_buffer_size;
}
