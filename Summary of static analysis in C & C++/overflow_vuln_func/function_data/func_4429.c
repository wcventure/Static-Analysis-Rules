static inline int check_input_motion(MpegEncContext * s, int mb_x, int mb_y, int p_type){
    MotionEstContext * const c= &s->me;
    Picture *p= s->current_picture_ptr;
    int mb_xy= mb_x + mb_y*s->mb_stride;
    int xy= 2*mb_x + 2*mb_y*s->b8_stride;
    int mb_type= s->current_picture.mb_type[mb_xy];
    int flags= c->flags;
    int shift= (flags&FLAG_QPEL) + 1;
    int mask= (1<<shift)-1;
    int x, y, i;
    int d=0;
    me_cmp_func cmpf= s->dsp.sse[0];
    me_cmp_func chroma_cmpf= s->dsp.sse[1];
    
    if(p_type && USES_LIST(mb_type, 1)){
        av_log(c->avctx, AV_LOG_ERROR, "backward motion vector in P frame\n");
        return INT_MAX;
    }
    assert(IS_INTRA(mb_type) || USES_LIST(mb_type,0) || USES_LIST(mb_type,1));
    
    for(i=0; i<4; i++){
        int xy= s->block_index[i];
        clip_input_mv(s, p->motion_val[0][xy], !!IS_INTERLACED(mb_type));
        clip_input_mv(s, p->motion_val[1][xy], !!IS_INTERLACED(mb_type));
    }

    if(IS_INTERLACED(mb_type)){
        int xy2= xy  + s->b8_stride;
        s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_INTRA;
        c->stride<<=1;
        c->uvstride<<=1;
        
        if(!(s->flags & CODEC_FLAG_INTERLACED_ME)){
            av_log(c->avctx, AV_LOG_ERROR, "Interlaced macroblock selected but interlaced motion estimation disabled\n");
            return INT_MAX;
        }

        if(USES_LIST(mb_type, 0)){
            int field_select0= p->ref_index[0][xy ];
            int field_select1= p->ref_index[0][xy2];
            assert(field_select0==0 ||field_select0==1);
            assert(field_select1==0 ||field_select1==1);
            init_interlaced_ref(s, 0);

            if(p_type){
                s->p_field_select_table[0][mb_xy]= field_select0;
                s->p_field_select_table[1][mb_xy]= field_select1;
                *(uint32_t*)s->p_field_mv_table[0][field_select0][mb_xy]= *(uint32_t*)p->motion_val[0][xy ];
                *(uint32_t*)s->p_field_mv_table[1][field_select1][mb_xy]= *(uint32_t*)p->motion_val[0][xy2];
                s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_INTER_I;
            }else{
                s->b_field_select_table[0][0][mb_xy]= field_select0;
                s->b_field_select_table[0][1][mb_xy]= field_select1;
                *(uint32_t*)s->b_field_mv_table[0][0][field_select0][mb_xy]= *(uint32_t*)p->motion_val[0][xy ];
                *(uint32_t*)s->b_field_mv_table[0][1][field_select1][mb_xy]= *(uint32_t*)p->motion_val[0][xy2];
                s->mb_type[mb_xy]= CANDIDATE_MB_TYPE_FORWARD_I;
            }

            x= p->motion_val[0][xy ][0]; 
            y= p->motion_val[0][xy ][1];
            d = cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 8, field_select0, 0, cmpf, chroma_cmpf, flags);
            x= p->motion_val[0][xy2][0]; 
            y= p->motion_val[0][xy2][1];
            d+= cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 8, field_select1, 1, cmpf, chroma_cmpf, flags);
        }
        if(USES_LIST(mb_type, 1)){
            int field_select0= p->ref_index[1][xy ];
            int field_select1= p->ref_index[1][xy2];
            assert(field_select0==0 ||field_select0==1);
            assert(field_select1==0 ||field_select1==1);
            init_interlaced_ref(s, 2);

            s->b_field_select_table[1][0][mb_xy]= field_select0;
            s->b_field_select_table[1][1][mb_xy]= field_select1;
            *(uint32_t*)s->b_field_mv_table[1][0][field_select0][mb_xy]= *(uint32_t*)p->motion_val[1][xy ];
            *(uint32_t*)s->b_field_mv_table[1][1][field_select1][mb_xy]= *(uint32_t*)p->motion_val[1][xy2];
            if(USES_LIST(mb_type, 0)){
                s->mb_type[mb_xy]= CANDIDATE_MB_TYPE_BIDIR_I;
            }else{
                s->mb_type[mb_xy]= CANDIDATE_MB_TYPE_BACKWARD_I;
            }

            x= p->motion_val[1][xy ][0]; 
            y= p->motion_val[1][xy ][1];
            d = cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 8, field_select0+2, 0, cmpf, chroma_cmpf, flags);
            x= p->motion_val[1][xy2][0]; 
            y= p->motion_val[1][xy2][1];
            d+= cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 8, field_select1+2, 1, cmpf, chroma_cmpf, flags);
            //FIXME bidir scores
        }
        c->stride>>=1;
        c->uvstride>>=1;
    }else if(IS_8X8(mb_type)){
        if(!(s->flags & CODEC_FLAG_4MV)){
            av_log(c->avctx, AV_LOG_ERROR, "4MV macroblock selected but 4MV encoding disabled\n");
            return INT_MAX;
        }
        cmpf= s->dsp.sse[1];
        chroma_cmpf= s->dsp.sse[1];
        init_mv4_ref(c);
        for(i=0; i<4; i++){
            xy= s->block_index[i];
            x= p->motion_val[0][xy][0]; 
            y= p->motion_val[0][xy][1];
            d+= cmp(s, x>>shift, y>>shift, x&mask, y&mask, 1, 8, i, i, cmpf, chroma_cmpf, flags);
        }
        s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_INTER4V;
    }else{
        if(USES_LIST(mb_type, 0)){
            if(p_type){
                *(uint32_t*)s->p_mv_table[mb_xy]= *(uint32_t*)p->motion_val[0][xy];
                s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_INTER;
            }else if(USES_LIST(mb_type, 1)){
                *(uint32_t*)s->b_bidir_forw_mv_table[mb_xy]= *(uint32_t*)p->motion_val[0][xy];
                *(uint32_t*)s->b_bidir_back_mv_table[mb_xy]= *(uint32_t*)p->motion_val[1][xy];
                s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_BIDIR;
            }else{
                *(uint32_t*)s->b_forw_mv_table[mb_xy]= *(uint32_t*)p->motion_val[0][xy];
                s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_FORWARD;
            }
            x= p->motion_val[0][xy][0]; 
            y= p->motion_val[0][xy][1];
            d = cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 16, 0, 0, cmpf, chroma_cmpf, flags);
        }else if(USES_LIST(mb_type, 1)){
            *(uint32_t*)s->b_back_mv_table[mb_xy]= *(uint32_t*)p->motion_val[1][xy];
            s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_BACKWARD;
           
            x= p->motion_val[1][xy][0]; 
            y= p->motion_val[1][xy][1];
            d = cmp(s, x>>shift, y>>shift, x&mask, y&mask, 0, 16, 2, 0, cmpf, chroma_cmpf, flags);
        }else
            s->mb_type[mb_xy]=CANDIDATE_MB_TYPE_INTRA;
    }
    return d;
}
