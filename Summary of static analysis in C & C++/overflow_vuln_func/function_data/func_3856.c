static int avi_read_header(AVFormatContext *s)
{
    AVIContext *avi = s->priv_data;
    AVIOContext *pb = s->pb;
    unsigned int tag, tag1, handler;
    int codec_type, stream_index, frame_period;
    unsigned int size;
    int i;
    AVStream *st;
    AVIStream *ast = NULL;
    int avih_width=0, avih_height=0;
    int amv_file_format=0;
    uint64_t list_end = 0;
    int ret;

    avi->stream_index= -1;

    if (get_riff(s, pb) < 0)
        return -1;

    av_log(avi, AV_LOG_DEBUG, "use odml:%d\n", avi->use_odml);

    avi->fsize = avio_size(pb);
    if(avi->fsize<=0 || avi->fsize < avi->riff_end)
        avi->fsize= avi->riff_end == 8 ? INT64_MAX : avi->riff_end;

    /
    stream_index = -1;
    codec_type = -1;
    frame_period = 0;
    for(;;) {
        if (url_feof(pb))
            goto fail;
        tag = avio_rl32(pb);
        size = avio_rl32(pb);

        print_tag("tag", tag, size);

        switch(tag) {
        case MKTAG('L', 'I', 'S', 'T'):
            list_end = avio_tell(pb) + size;
            /
            tag1 = avio_rl32(pb);

            print_tag("list", tag1, 0);

            if (tag1 == MKTAG('m', 'o', 'v', 'i')) {
                avi->movi_list = avio_tell(pb) - 4;
                if(size) avi->movi_end = avi->movi_list + size + (size & 1);
                else     avi->movi_end = avi->fsize;
                av_dlog(NULL, "movi end=%"PRIx64"\n", avi->movi_end);
                goto end_of_header;
            }
            else if (tag1 == MKTAG('I', 'N', 'F', 'O'))
                ff_read_riff_info(s, size - 4);
            else if (tag1 == MKTAG('n', 'c', 'd', 't'))
                avi_read_nikon(s, list_end);

            break;
        case MKTAG('I', 'D', 'I', 'T'): {
            unsigned char date[64] = {0};
            size += (size & 1);
            size -= avio_read(pb, date, FFMIN(size, sizeof(date)-1));
            avio_skip(pb, size);
            avi_metadata_creation_time(&s->metadata, date);
            break;
        }
        case MKTAG('d', 'm', 'l', 'h'):
            avi->is_odml = 1;
            avio_skip(pb, size + (size & 1));
            break;
        case MKTAG('a', 'm', 'v', 'h'):
            amv_file_format=1;
        case MKTAG('a', 'v', 'i', 'h'):
            /
            /
            frame_period = avio_rl32(pb);
            avio_rl32(pb); /
            avio_rl32(pb);
            avi->non_interleaved |= avio_rl32(pb) & AVIF_MUSTUSEINDEX;

            avio_skip(pb, 2 * 4);
            avio_rl32(pb);
            avio_rl32(pb);
            avih_width=avio_rl32(pb);
            avih_height=avio_rl32(pb);

            avio_skip(pb, size - 10 * 4);
            break;
        case MKTAG('s', 't', 'r', 'h'):
            /

            tag1 = avio_rl32(pb);
            handler = avio_rl32(pb); /

            if(tag1 == MKTAG('p', 'a', 'd', 's')){
                avio_skip(pb, size - 8);
                break;
            }else{
                stream_index++;
                st = avformat_new_stream(s, NULL);
                if (!st)
                    goto fail;

                st->id = stream_index;
                ast = av_mallocz(sizeof(AVIStream));
                if (!ast)
                    goto fail;
                st->priv_data = ast;
            }
            if(amv_file_format)
                tag1 = stream_index ? MKTAG('a','u','d','s') : MKTAG('v','i','d','s');

            print_tag("strh", tag1, -1);

            if(tag1 == MKTAG('i', 'a', 'v', 's') || tag1 == MKTAG('i', 'v', 'a', 's')){
                int64_t dv_dur;

                /
                if (s->nb_streams != 1)
                    goto fail;

                if (handler != MKTAG('d', 'v', 's', 'd') &&
                    handler != MKTAG('d', 'v', 'h', 'd') &&
                    handler != MKTAG('d', 'v', 's', 'l'))
                   goto fail;

                ast = s->streams[0]->priv_data;
                av_freep(&s->streams[0]->codec->extradata);
                av_freep(&s->streams[0]->codec);
                av_freep(&s->streams[0]->info);
                av_freep(&s->streams[0]);
                s->nb_streams = 0;
                if (CONFIG_DV_DEMUXER) {
                    avi->dv_demux = avpriv_dv_init_demux(s);
                    if (!avi->dv_demux)
                        goto fail;
                }
                s->streams[0]->priv_data = ast;
                avio_skip(pb, 3 * 4);
                ast->scale = avio_rl32(pb);
                ast->rate = avio_rl32(pb);
                avio_skip(pb, 4);  /

                dv_dur = avio_rl32(pb);
                if (ast->scale > 0 && ast->rate > 0 && dv_dur > 0) {
                    dv_dur *= AV_TIME_BASE;
                    s->duration = av_rescale(dv_dur, ast->scale, ast->rate);
                }
                /

                stream_index = s->nb_streams - 1;
                avio_skip(pb, size - 9*4);
                break;
            }

            av_assert0(stream_index < s->nb_streams);
            st->codec->stream_codec_tag= handler;

            avio_rl32(pb); /
            avio_rl16(pb); /
            avio_rl16(pb); /
            avio_rl32(pb); /
            ast->scale = avio_rl32(pb);
            ast->rate = avio_rl32(pb);
            if(!(ast->scale && ast->rate)){
                av_log(s, AV_LOG_WARNING, "scale/rate is %u/%u which is invalid. (This file has been generated by broken software.)\n", ast->scale, ast->rate);
                if(frame_period){
                    ast->rate = 1000000;
                    ast->scale = frame_period;
                }else{
                    ast->rate = 25;
                    ast->scale = 1;
                }
            }
            avpriv_set_pts_info(st, 64, ast->scale, ast->rate);

            ast->cum_len=avio_rl32(pb); /
            st->nb_frames = avio_rl32(pb);

            st->start_time = 0;
            avio_rl32(pb); /
            avio_rl32(pb); /
            if (ast->cum_len*ast->scale/ast->rate > 3600) {
                av_log(s, AV_LOG_ERROR, "crazy start time, iam scared, giving up\n");
                return AVERROR_INVALIDDATA;
            }
            ast->sample_size = avio_rl32(pb); /
            ast->cum_len *= FFMAX(1, ast->sample_size);
            av_dlog(s, "%"PRIu32" %"PRIu32" %d\n",
                    ast->rate, ast->scale, ast->sample_size);

            switch(tag1) {
            case MKTAG('v', 'i', 'd', 's'):
                codec_type = AVMEDIA_TYPE_VIDEO;

                ast->sample_size = 0;
                break;
            case MKTAG('a', 'u', 'd', 's'):
                codec_type = AVMEDIA_TYPE_AUDIO;
                break;
            case MKTAG('t', 'x', 't', 's'):
                codec_type = AVMEDIA_TYPE_SUBTITLE;
                break;
            case MKTAG('d', 'a', 't', 's'):
                codec_type = AVMEDIA_TYPE_DATA;
                break;
            default:
                av_log(s, AV_LOG_INFO, "unknown stream type %X\n", tag1);
            }
            if(ast->sample_size == 0)
                st->duration = st->nb_frames;
            ast->frame_offset= ast->cum_len;
            avio_skip(pb, size - 12 * 4);
            break;
        case MKTAG('s', 't', 'r', 'f'):
            /
            if (!size)
                break;
            if (stream_index >= (unsigned)s->nb_streams || avi->dv_demux) {
                avio_skip(pb, size);
            } else {
                uint64_t cur_pos = avio_tell(pb);
                unsigned esize;
                if (cur_pos < list_end)
                    size = FFMIN(size, list_end - cur_pos);
                st = s->streams[stream_index];
                switch(codec_type) {
                case AVMEDIA_TYPE_VIDEO:
                    if(amv_file_format){
                        st->codec->width=avih_width;
                        st->codec->height=avih_height;
                        st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
                        st->codec->codec_id = AV_CODEC_ID_AMV;
                        avio_skip(pb, size);
                        break;
                    }
                    tag1 = ff_get_bmp_header(pb, st, &esize);

                    if (tag1 == MKTAG('D', 'X', 'S', 'B') || tag1 == MKTAG('D','X','S','A')) {
                        st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
                        st->codec->codec_tag = tag1;
                        st->codec->codec_id = AV_CODEC_ID_XSUB;
                        break;
                    }

                    if(size > 10*4 && size<(1<<30) && size < avi->fsize){
                        if(esize == size-1 && (esize&1)) st->codec->extradata_size= esize - 10*4;
                        else                             st->codec->extradata_size=  size - 10*4;
                        st->codec->extradata= av_malloc(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                        if (!st->codec->extradata) {
                            st->codec->extradata_size= 0;
                            return AVERROR(ENOMEM);
                        }
                        avio_read(pb, st->codec->extradata, st->codec->extradata_size);
                    }

                    if(st->codec->extradata_size & 1) //FIXME check if the encoder really did this correctly
                        avio_r8(pb);

                    /
                    /
                    /
                    if (st->codec->extradata_size && (st->codec->bits_per_coded_sample <= 8)) {
                        int pal_size = (1 << st->codec->bits_per_coded_sample) << 2;
                        const uint8_t *pal_src;

                        pal_size = FFMIN(pal_size, st->codec->extradata_size);
                        pal_src = st->codec->extradata + st->codec->extradata_size - pal_size;
                        for (i = 0; i < pal_size/4; i++)
                            ast->pal[i] = 0xFF<<24 | AV_RL32(pal_src+4*i);
                        ast->has_pal = 1;
                    }

                    print_tag("video", tag1, 0);

                    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
                    st->codec->codec_tag = tag1;
                    st->codec->codec_id = ff_codec_get_id(ff_codec_bmp_tags, tag1);
                    st->need_parsing = AVSTREAM_PARSE_HEADERS; // This is needed to get the pict type which is necessary for generating correct pts.

                    if(st->codec->codec_tag==0 && st->codec->height > 0 && st->codec->extradata_size < 1U<<30){
                        st->codec->extradata_size+= 9;
                        st->codec->extradata= av_realloc_f(st->codec->extradata, 1, st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                        if(st->codec->extradata)
                            memcpy(st->codec->extradata + st->codec->extradata_size - 9, "BottomUp", 9);
                    }
                    st->codec->height= FFABS(st->codec->height);

//                    avio_skip(pb, size - 5 * 4);
                    break;
                case AVMEDIA_TYPE_AUDIO:
                    ret = ff_get_wav_header(pb, st->codec, size);
                    if (ret < 0)
                        return ret;
                    ast->dshow_block_align= st->codec->block_align;
                    if(ast->sample_size && st->codec->block_align && ast->sample_size != st->codec->block_align){
                        av_log(s, AV_LOG_WARNING, "sample size (%d) != block align (%d)\n", ast->sample_size, st->codec->block_align);
                        ast->sample_size= st->codec->block_align;
                    }
                    if (size&1) /
                        avio_skip(pb, 1);
                    /
                    st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
                    /
                    if (st->codec->codec_id == AV_CODEC_ID_AAC && st->codec->extradata_size)
                        st->need_parsing = AVSTREAM_PARSE_NONE;
                    /
                    if (st->codec->stream_codec_tag == AV_RL32("Axan")){
                        st->codec->codec_id  = AV_CODEC_ID_XAN_DPCM;
                        st->codec->codec_tag = 0;
                        ast->dshow_block_align = 0;
                    }
                    if (amv_file_format){
                        st->codec->codec_id  = AV_CODEC_ID_ADPCM_IMA_AMV;
                        ast->dshow_block_align = 0;
                    }
                    if(st->codec->codec_id == AV_CODEC_ID_AAC && ast->dshow_block_align <= 4 && ast->dshow_block_align) {
                        av_log(s, AV_LOG_DEBUG, "overriding invalid dshow_block_align of %d\n", ast->dshow_block_align);
                        ast->dshow_block_align = 0;
                    }
                    if(st->codec->codec_id == AV_CODEC_ID_AAC && ast->dshow_block_align == 1024 && ast->sample_size == 1024 ||
                       st->codec->codec_id == AV_CODEC_ID_AAC && ast->dshow_block_align == 4096 && ast->sample_size == 4096 ||
                       st->codec->codec_id == AV_CODEC_ID_MP3 && ast->dshow_block_align == 1152 && ast->sample_size == 1152) {
                        av_log(s, AV_LOG_DEBUG, "overriding sample_size\n");
                        ast->sample_size = 0;
                    }
                    break;
                case AVMEDIA_TYPE_SUBTITLE:
                    st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
                    st->request_probe= 1;
                    break;
                default:
                    st->codec->codec_type = AVMEDIA_TYPE_DATA;
                    st->codec->codec_id= AV_CODEC_ID_NONE;
                    st->codec->codec_tag= 0;
                    avio_skip(pb, size);
                    break;
                }
            }
            break;
        case MKTAG('s', 't', 'r', 'd'):
            if (stream_index >= (unsigned)s->nb_streams || s->streams[stream_index]->codec->extradata_size) {
                avio_skip(pb, size);
            } else {
                uint64_t cur_pos = avio_tell(pb);
                if (cur_pos < list_end)
                    size = FFMIN(size, list_end - cur_pos);
                st = s->streams[stream_index];

                if(size<(1<<30)){
                    st->codec->extradata_size= size;
                    st->codec->extradata= av_mallocz(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                    if (!st->codec->extradata) {
                        st->codec->extradata_size= 0;
                        return AVERROR(ENOMEM);
                    }
                    avio_read(pb, st->codec->extradata, st->codec->extradata_size);
                }

                if(st->codec->extradata_size & 1) //FIXME check if the encoder really did this correctly
                    avio_r8(pb);
            }
            break;
        case MKTAG('i', 'n', 'd', 'x'):
            i= avio_tell(pb);
            if(pb->seekable && !(s->flags & AVFMT_FLAG_IGNIDX) && avi->use_odml &&
               read_braindead_odml_indx(s, 0) < 0 && (s->error_recognition & AV_EF_EXPLODE))
                goto fail;
            avio_seek(pb, i+size, SEEK_SET);
            break;
        case MKTAG('v', 'p', 'r', 'p'):
            if(stream_index < (unsigned)s->nb_streams && size > 9*4){
                AVRational active, active_aspect;

                st = s->streams[stream_index];
                avio_rl32(pb);
                avio_rl32(pb);
                avio_rl32(pb);
                avio_rl32(pb);
                avio_rl32(pb);

                active_aspect.den= avio_rl16(pb);
                active_aspect.num= avio_rl16(pb);
                active.num       = avio_rl32(pb);
                active.den       = avio_rl32(pb);
                avio_rl32(pb); //nbFieldsPerFrame

                if(active_aspect.num && active_aspect.den && active.num && active.den){
                    st->sample_aspect_ratio= av_div_q(active_aspect, active);
                    av_dlog(s, "vprp %d/%d %d/%d\n",
                            active_aspect.num, active_aspect.den,
                            active.num, active.den);
                }
                size -= 9*4;
            }
            avio_skip(pb, size);
            break;
        case MKTAG('s', 't', 'r', 'n'):
            if(s->nb_streams){
                avi_read_tag(s, s->streams[s->nb_streams-1], tag, size);
                break;
            }
        default:
            if(size > 1000000){
                av_log(s, AV_LOG_ERROR, "Something went wrong during header parsing, "
                                        "I will ignore it and try to continue anyway.\n");
                if (s->error_recognition & AV_EF_EXPLODE)
                    goto fail;
                avi->movi_list = avio_tell(pb) - 4;
                avi->movi_end  = avi->fsize;
                goto end_of_header;
            }
            /
            size += (size & 1);
            avio_skip(pb, size);
            break;
        }
    }
 end_of_header:
    /
    if (stream_index != s->nb_streams - 1) {
    fail:
        return -1;
    }

    if(!avi->index_loaded && pb->seekable)
        avi_load_index(s);
    avi->index_loaded |= 1;
    avi->non_interleaved |= guess_ni_flag(s) | (s->flags & AVFMT_FLAG_SORT_DTS);
    for(i=0; i<s->nb_streams; i++){
        AVStream *st = s->streams[i];
        if(st->nb_index_entries)
            break;
    }
    // DV-in-AVI cannot be non-interleaved, if set this must be
    // a mis-detection.
    if(avi->dv_demux)
        avi->non_interleaved=0;
    if(i==s->nb_streams && avi->non_interleaved) {
        av_log(s, AV_LOG_WARNING, "non-interleaved AVI without index, switching to interleaved\n");
        avi->non_interleaved=0;
    }

    if(avi->non_interleaved) {
        av_log(s, AV_LOG_INFO, "non-interleaved AVI\n");
        clean_index(s);
    }

    ff_metadata_conv_ctx(s, NULL, avi_metadata_conv);
    ff_metadata_conv_ctx(s, NULL, ff_riff_info_conv);

    return 0;
}
