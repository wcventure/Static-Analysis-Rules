static int avi_sync(AVFormatContext *s, int exit_early)
{
    AVIContext *avi = s->priv_data;
    AVIOContext *pb = s->pb;
    int n;
    unsigned int d[8];
    unsigned int size;
    int64_t i, sync;

start_sync:
    memset(d, -1, sizeof(d));
    for(i=sync=avio_tell(pb); !url_feof(pb); i++) {
        int j;

        for(j=0; j<7; j++)
            d[j]= d[j+1];
        d[7]= avio_r8(pb);

        size= d[4] + (d[5]<<8) + (d[6]<<16) + (d[7]<<24);

        n= get_stream_idx(d+2);
        av_dlog(s, "%X %X %X %X %X %X %X %X %"PRId64" %u %d\n",
                d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], i, size, n);
        if(i + (uint64_t)size > avi->fsize || d[0] > 127)
            continue;

        //parse ix##
        if(  (d[0] == 'i' && d[1] == 'x' && n < s->nb_streams)
        //parse JUNK
           ||(d[0] == 'J' && d[1] == 'U' && d[2] == 'N' && d[3] == 'K')
           ||(d[0] == 'i' && d[1] == 'd' && d[2] == 'x' && d[3] == '1')){
            avio_skip(pb, size);
            goto start_sync;
        }

        //parse stray LIST
        if(d[0] == 'L' && d[1] == 'I' && d[2] == 'S' && d[3] == 'T'){
            avio_skip(pb, 4);
            goto start_sync;
        }

        n= get_stream_idx(d);

        if(!((i-avi->last_pkt_pos)&1) && get_stream_idx(d+1) < s->nb_streams)
            continue;

        //detect ##ix chunk and skip
        if(d[2] == 'i' && d[3] == 'x' && n < s->nb_streams){
            avio_skip(pb, size);
            goto start_sync;
        }

        //parse ##dc/##wb
        if(n < s->nb_streams){
            AVStream *st;
            AVIStream *ast;
            st = s->streams[n];
            ast = st->priv_data;

            if (!ast) {
                av_log(s, AV_LOG_WARNING, "Skiping foreign stream %d packet\n", n);
                continue;
            }

            if(s->nb_streams>=2){
                AVStream *st1  = s->streams[1];
                AVIStream *ast1= st1->priv_data;
                //workaround for broken small-file-bug402.avi
                if(   d[2] == 'w' && d[3] == 'b'
                   && n==0
                   && st ->codec->codec_type == AVMEDIA_TYPE_VIDEO
                   && st1->codec->codec_type == AVMEDIA_TYPE_AUDIO
                   && ast->prefix == 'd'*256+'c'
                   && (d[2]*256+d[3] == ast1->prefix || !ast1->prefix_count)
                  ){
                    n=1;
                    st = st1;
                    ast = ast1;
                    av_log(s, AV_LOG_WARNING, "Invalid stream + prefix combination, assuming audio.\n");
                }
            }


            if(   (st->discard >= AVDISCARD_DEFAULT && size==0)
               / //FIXME needs a little reordering
               || st->discard >= AVDISCARD_ALL){
                if (!exit_early) {
                    ast->frame_offset += get_duration(ast, size);
                }
                avio_skip(pb, size);
                goto start_sync;
            }

            if (d[2] == 'p' && d[3] == 'c' && size<=4*256+4) {
                int k = avio_r8(pb);
                int last = (k + avio_r8(pb) - 1) & 0xFF;

                avio_rl16(pb); //flags

                for (; k <= last; k++)
                    ast->pal[k] = 0xFF<<24 | avio_rb32(pb)>>8;// b + (g << 8) + (r << 16);
                ast->has_pal= 1;
                goto start_sync;
            } else if(   ((ast->prefix_count<5 || sync+9 > i) && d[2]<128 && d[3]<128) ||
                         d[2]*256+d[3] == ast->prefix /) {

                if (exit_early)
                    return 0;
                if(d[2]*256+d[3] == ast->prefix)
                    ast->prefix_count++;
                else{
                    ast->prefix= d[2]*256+d[3];
                    ast->prefix_count= 0;
                }

                avi->stream_index= n;
                ast->packet_size= size + 8;
                ast->remaining= size;

                if(size || !ast->sample_size){
                    uint64_t pos= avio_tell(pb) - 8;
                    if(!st->index_entries || !st->nb_index_entries || st->index_entries[st->nb_index_entries - 1].pos < pos){
                        av_add_index_entry(st, pos, ast->frame_offset, size, 0, AVINDEX_KEYFRAME);
                    }
                }
                return 0;
            }
        }
    }

    if(pb->error)
        return pb->error;
    return AVERROR_EOF;
}
