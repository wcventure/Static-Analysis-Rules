static void floor_fit(venc_context_t * venc, floor_t * fc, float * coeffs, int * posts, int samples) {
    int range = 255 / fc->multiplier + 1;
    int i;
    for (i = 0; i < fc->values; i++) {
        int position = fc->list[fc->list[i].sort].x;
        int begin = fc->list[fc->list[FFMAX(i-1, 0)].sort].x;
        int end   = fc->list[fc->list[FFMIN(i+1, fc->values - 1)].sort].x;
        int j;
        float average = 0;
        begin = (position + begin) / 2;
        end   = (position + end  ) / 2;

        assert(end <= samples);
        for (j = begin; j < end; j++) average += fabs(coeffs[j]);
        average /= end - begin;
        average /= 32; // MAGIC!
        for (j = 0; j < range; j++) if (floor1_inverse_db_table[j * fc->multiplier] > average) break;
        posts[fc->list[i].sort] = j;
    }
}
