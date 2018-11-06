static int ac3_sync(uint64_t state, AACAC3ParseContext *hdr_info,
        int *need_next_header, int *new_frame_start)
{
    int err;
    union {
        uint64_t u64;
        uint8_t  u8[8];
    } tmp = { av_be2ne64(state) };
    AC3HeaderInfo hdr, *phdr = &hdr;
    GetBitContext gbc;

    init_get_bits(&gbc, tmp.u8+8-AC3_HEADER_SIZE, 54);
    err = avpriv_ac3_parse_header2(&gbc, &phdr);

    if(err < 0)
        return 0;

    hdr_info->sample_rate = hdr.sample_rate;
    hdr_info->bit_rate = hdr.bit_rate;
    hdr_info->channels = hdr.channels;
    hdr_info->channel_layout = hdr.channel_layout;
    hdr_info->samples = hdr.num_blocks * 256;
    hdr_info->service_type = hdr.bitstream_mode;
    if (hdr.bitstream_mode == 0x7 && hdr.channels > 1)
        hdr_info->service_type = AV_AUDIO_SERVICE_TYPE_KARAOKE;
    if(hdr.bitstream_id>10)
        hdr_info->codec_id = AV_CODEC_ID_EAC3;
    else if (hdr_info->codec_id == AV_CODEC_ID_NONE)
        hdr_info->codec_id = AV_CODEC_ID_AC3;

    *need_next_header = (hdr.frame_type != EAC3_FRAME_TYPE_AC3_CONVERT);
    *new_frame_start  = (hdr.frame_type != EAC3_FRAME_TYPE_DEPENDENT);
    return hdr.frame_size;
}
