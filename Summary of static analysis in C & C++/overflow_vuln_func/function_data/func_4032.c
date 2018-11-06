static av_always_inline int check_4block_inter(SnowContext *s, int mb_x, int mb_y, int p0, int p1, int ref, int *best_rd){
    const int b_stride= s->b_width << s->block_max_depth;
    BlockNode *block= &s->block[mb_x + mb_y * b_stride];
    BlockNode backup[4]= {block[0], block[1], block[b_stride], block[b_stride+1]};
    int rd, index, value;

    assert(mb_x>=0 && mb_y>=0);
    assert(mb_x<b_stride);
    assert(((mb_x|mb_y)&1) == 0);

    index= (p0 + 31*p1) & (ME_CACHE_SIZE-1);
    value= s->me_cache_generation + (p0>>10) + (p1<<6) + (block->ref<<12);
    if(s->me_cache[index] == value)
        return 0;
    s->me_cache[index]= value;

    block->mx= p0;
    block->my= p1;
    block->ref= ref;
    block->type &= ~BLOCK_INTRA;
    block[1]= block[b_stride]= block[b_stride+1]= *block;

    rd= get_4block_rd(s, mb_x, mb_y, 0);

//FIXME chroma
    if(rd < *best_rd){
        *best_rd= rd;
        return 1;
    }else{
        block[0]= backup[0];
        block[1]= backup[1];
        block[b_stride]= backup[2];
        block[b_stride+1]= backup[3];
        return 0;
    }
}
