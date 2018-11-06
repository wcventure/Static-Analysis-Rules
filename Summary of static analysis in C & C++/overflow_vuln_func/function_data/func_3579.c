static void filter(SPPContext *p, uint8_t *dst, uint8_t *src,
                   int dst_linesize, int src_linesize, int width, int height,
                   const uint8_t *qp_table, int qp_stride, int is_luma, int sample_bytes)
{
    int x, y, i;
    const int count = 1 << p->log2_count;
    const int linesize = is_luma ? p->temp_linesize : FFALIGN(width+16, 16);
    DECLARE_ALIGNED(16, uint64_t, block_align)[32];
    int16_t *block  = (int16_t *)block_align;
    int16_t *block2 = (int16_t *)(block_align + 16);
    uint16_t *psrc16 = (uint16_t*)p->src;

    for (y = 0; y < height; y++) {
        int index = 8 + 8*linesize + y*linesize;
        memcpy(p->src + index*sample_bytes, src + y*src_linesize, width*sample_bytes);
        if (sample_bytes == 1) {
            for (x = 0; x < 8; x++) {
                p->src[index         - x - 1] = p->src[index +         x    ];
                p->src[index + width + x    ] = p->src[index + width - x - 1];
            }
        } else {
            for (x = 0; x < 8; x++) {
                psrc16[index         - x - 1] = psrc16[index +         x    ];
                psrc16[index + width + x    ] = psrc16[index + width - x - 1];
            }
        }
    }
    for (y = 0; y < 8; y++) {
        memcpy(p->src + (       7-y)*linesize * sample_bytes, p->src + (       y+8)*linesize * sample_bytes, linesize * sample_bytes);
        memcpy(p->src + (height+8+y)*linesize * sample_bytes, p->src + (height-y+7)*linesize * sample_bytes, linesize * sample_bytes);
    }

    for (y = 0; y < height + 8; y += 8) {
        memset(p->temp + (8 + y) * linesize, 0, 8 * linesize * sizeof(*p->temp));
        for (x = 0; x < width + 8; x += 8) {
            int qp;

            if (p->qp) {
                qp = p->qp;
            } else{
                const int qps = 3 + is_luma;
                qp = qp_table[(FFMIN(x, width - 1) >> qps) + (FFMIN(y, height - 1) >> qps) * qp_stride];
                qp = FFMAX(1, norm_qscale(qp, p->qscale_type));
            }
            for (i = 0; i < count; i++) {
                const int x1 = x + offset[i + count - 1][0];
                const int y1 = y + offset[i + count - 1][1];
                const int index = x1 + y1*linesize;
                p->dct->get_pixels(block, p->src + sample_bytes*index, sample_bytes*linesize);
                p->dct->fdct(block);
                p->requantize(block2, block, qp, p->dct->idct_permutation);
                p->dct->idct(block2);
                add_block(p->temp + index, linesize, block2);
            }
        }
        if (y) {
            if (sample_bytes == 1) {
                p->store_slice(dst + (y - 8) * dst_linesize, p->temp + 8 + y*linesize,
                               dst_linesize, linesize, width,
                               FFMIN(8, height + 8 - y), MAX_LEVEL - p->log2_count,
                               ldither);
            } else {
                store_slice16_c((uint16_t*)(dst + (y - 8) * dst_linesize), p->temp + 8 + y*linesize,
                                dst_linesize/2, linesize, width,
                                FFMIN(8, height + 8 - y), MAX_LEVEL - p->log2_count,
                                ldither);
            }
        }
    }
}
