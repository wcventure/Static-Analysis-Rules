static int get_scale_factor(H264SliceContext *sl,
                            int poc, int poc1, int i)
{
    int poc0 = sl->ref_list[0][i].poc;
    int td = av_clip_int8(poc1 - poc0);
    if (td == 0 || sl->ref_list[0][i].parent->long_ref) {
        return 256;
    } else {
        int tb = av_clip_int8(poc - poc0);
        int tx = (16384 + (FFABS(td) >> 1)) / td;
        return av_clip_intp2((tb * tx + 32) >> 6, 10);
    }
}
