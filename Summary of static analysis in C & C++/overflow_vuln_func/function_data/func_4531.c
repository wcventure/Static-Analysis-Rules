void ff_estimate_b_frame_motion(MpegEncContext * s,
                             int mb_x, int mb_y)
{
    const int quant= s->qscale;
    int fmin, bmin, dmin, fbmin;
    int type=0;
    
    dmin= direct_search(s, mb_x, mb_y);

    fmin= ff_estimate_motion_b(s, mb_x, mb_y, s->b_forw_mv_table, s->last_picture[0], s->f_code);
    bmin= ff_estimate_motion_b(s, mb_x, mb_y, s->b_back_mv_table, s->next_picture[0], s->b_code) - quant;
//printf(" %d %d ", s->b_forw_mv_table[xy][0], s->b_forw_mv_table[xy][1]);

    fbmin= bidir_refine(s, mb_x, mb_y);

    {
        int score= dmin;
        type=MB_TYPE_DIRECT;
        
        if(fmin<score){
            score=fmin;
            type= MB_TYPE_FORWARD; 
        }
        if(bmin<score){
            score=bmin;
            type= MB_TYPE_BACKWARD; 
        }
        if(fbmin<score){
            score=fbmin;
            type= MB_TYPE_BIDIR;
        }
        score= (score*score + 128*256)>>16;
        s->mc_mb_var_sum += score;
        s->mc_mb_var[mb_y*s->mb_width + mb_x] = score; //FIXME use SSD
    }

    if(s->flags&CODEC_FLAG_HQ){
        type= MB_TYPE_FORWARD | MB_TYPE_BACKWARD | MB_TYPE_BIDIR | MB_TYPE_DIRECT; //FIXME something smarter
    }

/

    s->mb_type[mb_y*s->mb_width + mb_x]= type;
/
}
