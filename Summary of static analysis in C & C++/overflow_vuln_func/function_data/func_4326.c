static int avi_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    AVIContext *avi = s->priv_data;
    ByteIOContext *pb = &s->pb;
    uint32_t tag, tag1, handler;
    int codec_type, stream_index, frame_period, bit_rate, scale, rate;
    unsigned int size, nb_frames;
    int i, n;
    AVStream *st;
    AVIStream *ast;
    int xan_video = 0;  /

    if (get_riff(avi, pb) < 0)
        return -1;

    /
    stream_index = -1;
    codec_type = -1;
    frame_period = 0;
    for(;;) {
        if (url_feof(pb))
            goto fail;
        tag = get_le32(pb);
        size = get_le32(pb);
#ifdef DEBUG
        print_tag("tag", tag, size);
#endif

        switch(tag) {
        case MKTAG('L', 'I', 'S', 'T'):
            /
            tag1 = get_le32(pb);
#ifdef DEBUG
            print_tag("list", tag1, 0);
#endif
            if (tag1 == MKTAG('m', 'o', 'v', 'i')) {
                avi->movi_list = url_ftell(pb) - 4;
                if(size) avi->movi_end = avi->movi_list + size;
                else     avi->movi_end = url_filesize(url_fileno(pb));
#ifdef DEBUG
                printf("movi end=%Lx\n", avi->movi_end);
#endif
                goto end_of_header;
            }
            break;
        case MKTAG('d', 'm', 'l', 'h'):
	    avi->is_odml = 1;
	    url_fskip(pb, size + (size & 1));
	    break;
        case MKTAG('a', 'v', 'i', 'h'):
	    /
            /
            frame_period = get_le32(pb);
            bit_rate = get_le32(pb) * 8;
	    url_fskip(pb, 4 * 4);
            n = get_le32(pb);
            for(i=0;i<n;i++) {
                AVIStream *ast;
                st = av_new_stream(s, i);
                if (!st)
                    goto fail;

                ast = av_mallocz(sizeof(AVIStream));
                if (!ast)
                    goto fail;
                st->priv_data = ast;
	    }
            url_fskip(pb, size - 7 * 4);
            break;
        case MKTAG('s', 't', 'r', 'h'):
            /
            stream_index++;
            tag1 = get_le32(pb);
            handler = get_le32(pb); /
#ifdef DEBUG
        print_tag("strh", tag1, -1);
#endif
            switch(tag1) {
            case MKTAG('i', 'a', 'v', 's'):
	    case MKTAG('i', 'v', 'a', 's'):
                /
	        if (s->nb_streams != 1)
		    goto fail;
	        
		if (handler != MKTAG('d', 'v', 's', 'd') &&
	            handler != MKTAG('d', 'v', 'h', 'd') &&
		    handler != MKTAG('d', 'v', 's', 'l'))
	           goto fail;
                
		ast = s->streams[0]->priv_data;
		av_freep(&s->streams[0]->codec.extradata);
		av_freep(&s->streams[0]);
		s->nb_streams = 0;
	        avi->dv_demux = dv_init_demux(s);
		if (!avi->dv_demux)
		    goto fail;
		s->streams[0]->priv_data = ast;
		url_fskip(pb, 3 * 4);
		ast->scale = get_le32(pb);
		ast->rate = get_le32(pb);
	        stream_index = s->nb_streams - 1;
		url_fskip(pb, size - 7*4);
		break;
	    case MKTAG('v', 'i', 'd', 's'):
                codec_type = CODEC_TYPE_VIDEO;

                if (stream_index >= s->nb_streams) {
                    url_fskip(pb, size - 8);
                    break;
                } 

                st = s->streams[stream_index];
                ast = st->priv_data;
                st->codec.stream_codec_tag= handler;
                
                get_le32(pb); /
                get_le16(pb); /
                get_le16(pb); /
                get_le32(pb); /
                scale = get_le32(pb); /
                rate = get_le32(pb); /

                if(scale && rate){
                }else if(frame_period){
                    rate = 1000000;
                    scale = frame_period;
                }else{
                    rate = 25;
                    scale = 1;
                }
                ast->rate = rate;
                ast->scale = scale;
                av_set_pts_info(st, 64, scale, rate);
                st->codec.frame_rate = rate;
                st->codec.frame_rate_base = scale;
                get_le32(pb); /
                nb_frames = get_le32(pb);
                st->start_time = 0;
                st->duration = av_rescale(nb_frames,
                    st->codec.frame_rate_base * AV_TIME_BASE,
                    st->codec.frame_rate);
		url_fskip(pb, size - 9 * 4);
                break;
            case MKTAG('a', 'u', 'd', 's'):
                {
                    unsigned int length;

                    codec_type = CODEC_TYPE_AUDIO;

                    if (stream_index >= s->nb_streams) {
                        url_fskip(pb, size - 8);
                        break;
                    } 
                    st = s->streams[stream_index];
                    ast = st->priv_data;
                    
                    get_le32(pb); /
                    get_le16(pb); /
                    get_le16(pb); /
                    get_le32(pb); /
                    ast->scale = get_le32(pb); /
                    ast->rate = get_le32(pb);
                    if(!ast->rate)
                        ast->rate= 1; //wrong but better then 1/0
                    if(!ast->scale)
                        ast->scale= 1; //wrong but better then 1/0
                    av_set_pts_info(st, 64, ast->scale, ast->rate);
                    ast->start= get_le32(pb); /
                    length = get_le32(pb); /
                    get_le32(pb); /
                    get_le32(pb); /
                    ast->sample_size = get_le32(pb); /
//av_log(NULL, AV_LOG_DEBUG, "%d %d %d %d\n", ast->scale, ast->rate, ast->sample_size, ast->start);
                    st->start_time = 0;
                    if (ast->rate != 0)
                        st->duration = (int64_t)length * AV_TIME_BASE / ast->rate;
                    url_fskip(pb, size - 12 * 4);
                }
                break;
            case MKTAG('t', 'x', 't', 's'):
                //FIXME 
                codec_type = CODEC_TYPE_DATA; //CODEC_TYPE_SUB ?  FIXME
                url_fskip(pb, size - 8);
                break;
            case MKTAG('p', 'a', 'd', 's'):
                codec_type = CODEC_TYPE_UNKNOWN;
                url_fskip(pb, size - 8);
                stream_index--;
                break;
            default:
                av_log(s, AV_LOG_ERROR, "unknown stream type %X\n", tag1);
                goto fail;
            }
            break;
        case MKTAG('s', 't', 'r', 'f'):
            /
            if (stream_index >= s->nb_streams || avi->dv_demux) {
                url_fskip(pb, size);
            } else {
                st = s->streams[stream_index];
                switch(codec_type) {
                case CODEC_TYPE_VIDEO:
                    get_le32(pb); /
                    st->codec.width = get_le32(pb);
                    st->codec.height = get_le32(pb);
                    get_le16(pb); /
                    st->codec.bits_per_sample= get_le16(pb); /
                    tag1 = get_le32(pb);
                    get_le32(pb); /
                    get_le32(pb); /
                    get_le32(pb); /
                    get_le32(pb); /
                    get_le32(pb); /

                 if(size > 10*4 && size<(1<<30)){
                    st->codec.extradata_size= size - 10*4;
                    st->codec.extradata= av_malloc(st->codec.extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                    get_buffer(pb, st->codec.extradata, st->codec.extradata_size);
                 }
                    
                    if(st->codec.extradata_size & 1) //FIXME check if the encoder really did this correctly
                        get_byte(pb);

                    /
                    /
                    /
                    if (st->codec.extradata_size && (st->codec.bits_per_sample <= 8)) {
                        st->codec.palctrl = av_mallocz(sizeof(AVPaletteControl));
#ifdef WORDS_BIGENDIAN
                        for (i = 0; i < FFMIN(st->codec.extradata_size, AVPALETTE_SIZE)/4; i++)
                            st->codec.palctrl->palette[i] = bswap_32(((uint32_t*)st->codec.extradata)[i]);
#else
                        memcpy(st->codec.palctrl->palette, st->codec.extradata,
                               FFMIN(st->codec.extradata_size, AVPALETTE_SIZE));
#endif
                        st->codec.palctrl->palette_changed = 1;
                    }

#ifdef DEBUG
                    print_tag("video", tag1, 0);
#endif
                    st->codec.codec_type = CODEC_TYPE_VIDEO;
                    st->codec.codec_tag = tag1;
                    st->codec.codec_id = codec_get_id(codec_bmp_tags, tag1);
                    if (st->codec.codec_id == CODEC_ID_XAN_WC4)
                        xan_video = 1;
//                    url_fskip(pb, size - 5 * 4);
                    break;
                case CODEC_TYPE_AUDIO:
                    get_wav_header(pb, &st->codec, size);
                    if (size%2) /
                        url_fskip(pb, 1);
                    /
                    st->need_parsing = 1;
                    /
                    if (xan_video)
                        st->codec.codec_id = CODEC_ID_XAN_DPCM;
                    break;
                default:
                    st->codec.codec_type = CODEC_TYPE_DATA;
                    st->codec.codec_id= CODEC_ID_NONE;
                    st->codec.codec_tag= 0;
                    url_fskip(pb, size);
                    break;
                }
            }
            break;
        default:
            /
            size += (size & 1);
            url_fskip(pb, size);
            break;
        }
    }
 end_of_header:
    /
    if (stream_index != s->nb_streams - 1) {
    fail:
        for(i=0;i<s->nb_streams;i++) {
            av_freep(&s->streams[i]->codec.extradata);
            av_freep(&s->streams[i]);
        }
        return -1;
    }

    assert(!avi->index_loaded);
    avi_load_index(s);
    avi->index_loaded = 1;
 
    return 0;
}
