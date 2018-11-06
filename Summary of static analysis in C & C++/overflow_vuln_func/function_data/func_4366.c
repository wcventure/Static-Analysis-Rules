static av_always_inline int small_diamond_search(MpegEncContext * s, int *best, int dmin,
                                       int src_index, int ref_index, int const penalty_factor,
                                       int size, int h, int flags)
{
    MotionEstContext * const c= &s->me;
    me_cmp_func cmpf, chroma_cmpf;
    int next_dir=-1;
    LOAD_COMMON
    LOAD_COMMON2
    unsigned map_generation = c->map_generation;

    cmpf        = s->mecc.me_cmp[size];
    chroma_cmpf = s->mecc.me_cmp[size + 1];

    { /
        const unsigned key = (best[1]<<ME_MAP_MV_BITS) + best[0] + map_generation;
        const int index= ((best[1]<<ME_MAP_SHIFT) + best[0])&(ME_MAP_SIZE-1);
        if(map[index]!=key){ //this will be executed only very rarey
            score_map[index]= cmp(s, best[0], best[1], 0, 0, size, h, ref_index, src_index, cmpf, chroma_cmpf, flags);
            map[index]= key;
        }
    }

    for(;;){
        int d;
        const int dir= next_dir;
        const int x= best[0];
        const int y= best[1];
        next_dir=-1;

        if(dir!=2 && x>xmin) CHECK_MV_DIR(x-1, y  , 0)
        if(dir!=3 && y>ymin) CHECK_MV_DIR(x  , y-1, 1)
        if(dir!=0 && x<xmax) CHECK_MV_DIR(x+1, y  , 2)
        if(dir!=1 && y<ymax) CHECK_MV_DIR(x  , y+1, 3)

        if(next_dir==-1){
            return dmin;
        }
    }
}
