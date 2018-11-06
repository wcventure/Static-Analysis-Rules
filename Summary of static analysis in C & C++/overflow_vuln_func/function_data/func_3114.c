static int encode_block(SVQ1Context *s, uint8_t *src, uint8_t *ref, uint8_t *decoded, int stride, int level, int threshold, int lambda, int intra){
    int count, y, x, i, j, split, best_mean, best_score, best_count;
    int best_vector[6];
    int block_sum[7]= {0, 0, 0, 0, 0, 0};
    int w= 2<<((level+2)>>1);
    int h= 2<<((level+1)>>1);
    int size=w*h;
    int16_t block[7][256];
    const int8_t *codebook_sum, *codebook;
    const uint16_t (*mean_vlc)[2];
    const uint8_t (*multistage_vlc)[2];

    best_score=0;
    //FIXME optimize, this doenst need to be done multiple times
    if(intra){
        codebook_sum= svq1_intra_codebook_sum[level];
        codebook= ff_svq1_intra_codebooks[level];
        mean_vlc= ff_svq1_intra_mean_vlc;
        multistage_vlc= ff_svq1_intra_multistage_vlc[level];
        for(y=0; y<h; y++){
            for(x=0; x<w; x++){
                int v= src[x + y*stride];
                block[0][x + w*y]= v;
                best_score += v*v;
                block_sum[0] += v;
            }
        }
    }else{
        codebook_sum= svq1_inter_codebook_sum[level];
        codebook= ff_svq1_inter_codebooks[level];
        mean_vlc= ff_svq1_inter_mean_vlc + 256;
        multistage_vlc= ff_svq1_inter_multistage_vlc[level];
        for(y=0; y<h; y++){
            for(x=0; x<w; x++){
                int v= src[x + y*stride] - ref[x + y*stride];
                block[0][x + w*y]= v;
                best_score += v*v;
                block_sum[0] += v;
            }
        }
    }

    best_count=0;
    best_score -= ((block_sum[0]*block_sum[0])>>(level+3));
    best_mean= (block_sum[0] + (size>>1)) >> (level+3);

    if(level<4){
        for(count=1; count<7; count++){
            int best_vector_score= INT_MAX;
            int best_vector_sum=-999, best_vector_mean=-999;
            const int stage= count-1;
            const int8_t *vector;

            for(i=0; i<16; i++){
                int sum= codebook_sum[stage*16 + i];
                int sqr, diff, score;

                vector = codebook + stage*size*16 + i*size;
                sqr = s->dsp.ssd_int8_vs_int16(vector, block[stage], size);
                diff= block_sum[stage] - sum;
                score= sqr - ((diff*(int64_t)diff)>>(level+3)); //FIXME 64bit slooow
                if(score < best_vector_score){
                    int mean= (diff + (size>>1)) >> (level+3);
                    assert(mean >-300 && mean<300);
                    mean= av_clip(mean, intra?0:-256, 255);
                    best_vector_score= score;
                    best_vector[stage]= i;
                    best_vector_sum= sum;
                    best_vector_mean= mean;
                }
            }
            assert(best_vector_mean != -999);
            vector= codebook + stage*size*16 + best_vector[stage]*size;
            for(j=0; j<size; j++){
                block[stage+1][j] = block[stage][j] - vector[j];
            }
            block_sum[stage+1]= block_sum[stage] - best_vector_sum;
            best_vector_score +=
                lambda*(+ 1 + 4*count
                        + multistage_vlc[1+count][1]
                        + mean_vlc[best_vector_mean][1]);

            if(best_vector_score < best_score){
                best_score= best_vector_score;
                best_count= count;
                best_mean= best_vector_mean;
            }
        }
    }

    split=0;
    if(best_score > threshold && level){
        int score=0;
        int offset= (level&1) ? stride*h/2 : w/2;
        PutBitContext backup[6];

        for(i=level-1; i>=0; i--){
            backup[i]= s->reorder_pb[i];
        }
        score += encode_block(s, src         , ref         , decoded         , stride, level-1, threshold>>1, lambda, intra);
        score += encode_block(s, src + offset, ref + offset, decoded + offset, stride, level-1, threshold>>1, lambda, intra);
        score += lambda;

        if(score < best_score){
            best_score= score;
            split=1;
        }else{
            for(i=level-1; i>=0; i--){
                s->reorder_pb[i]= backup[i];
            }
        }
    }
    if (level > 0)
        put_bits(&s->reorder_pb[level], 1, split);

    if(!split){
        assert((best_mean >= 0 && best_mean<256) || !intra);
        assert(best_mean >= -256 && best_mean<256);
        assert(best_count >=0 && best_count<7);
        assert(level<4 || best_count==0);

        /
        put_bits(&s->reorder_pb[level],
            multistage_vlc[1 + best_count][1],
            multistage_vlc[1 + best_count][0]);
        put_bits(&s->reorder_pb[level], mean_vlc[best_mean][1],
            mean_vlc[best_mean][0]);

        for (i = 0; i < best_count; i++){
            assert(best_vector[i]>=0 && best_vector[i]<16);
            put_bits(&s->reorder_pb[level], 4, best_vector[i]);
        }

        for(y=0; y<h; y++){
            for(x=0; x<w; x++){
                decoded[x + y*stride]= src[x + y*stride] - block[best_count][x + w*y] + best_mean;
            }
        }
    }

    return best_score;
}
