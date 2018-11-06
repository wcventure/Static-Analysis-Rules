int ff_snow_frame_start(SnowContext *s){
   AVFrame tmp;
   int i, ret;
   int w= s->avctx->width; //FIXME round up to x16 ?
   int h= s->avctx->height;

    if (s->current_picture.data[0] && !(s->avctx->flags&CODEC_FLAG_EMU_EDGE)) {
        s->dsp.draw_edges(s->current_picture.data[0],
                          s->current_picture.linesize[0], w   , h   ,
                          EDGE_WIDTH  , EDGE_WIDTH  , EDGE_TOP | EDGE_BOTTOM);
        s->dsp.draw_edges(s->current_picture.data[1],
                          s->current_picture.linesize[1], w>>s->chroma_h_shift, h>>s->chroma_v_shift,
                          EDGE_WIDTH>>s->chroma_h_shift, EDGE_WIDTH>>s->chroma_v_shift, EDGE_TOP | EDGE_BOTTOM);
        s->dsp.draw_edges(s->current_picture.data[2],
                          s->current_picture.linesize[2], w>>s->chroma_h_shift, h>>s->chroma_v_shift,
                          EDGE_WIDTH>>s->chroma_h_shift, EDGE_WIDTH>>s->chroma_v_shift, EDGE_TOP | EDGE_BOTTOM);
    }

    ff_snow_release_buffer(s->avctx);

    av_frame_move_ref(&tmp, &s->last_picture[s->max_ref_frames-1]);
    for(i=s->max_ref_frames-1; i>0; i--)
        av_frame_move_ref(&s->last_picture[i+1], &s->last_picture[i]);
    memmove(s->halfpel_plane+1, s->halfpel_plane, (s->max_ref_frames-1)*sizeof(void*)*4*4);
    if(USE_HALFPEL_PLANE && s->current_picture.data[0])
        halfpel_interpol(s, s->halfpel_plane[0], &s->current_picture);
    av_frame_move_ref(&s->last_picture[0], &s->current_picture);
    av_frame_move_ref(&s->current_picture, &tmp);

    if(s->keyframe){
        s->ref_frames= 0;
    }else{
        int i;
        for(i=0; i<s->max_ref_frames && s->last_picture[i].data[0]; i++)
            if(i && s->last_picture[i-1].key_frame)
                break;
        s->ref_frames= i;
        if(s->ref_frames==0){
            av_log(s->avctx,AV_LOG_ERROR, "No reference frames\n");
            return -1;
        }
    }

    if ((ret = ff_get_buffer(s->avctx, &s->current_picture, AV_GET_BUFFER_FLAG_REF)) < 0)
        return ret;

    s->current_picture.key_frame= s->keyframe;

    return 0;
}
