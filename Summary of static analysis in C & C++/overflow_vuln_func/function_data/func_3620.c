static void ir2_decode_plane_inter(Ir2Context *ctx, int width, int height, uint8_t *dst, int stride,
                             const uint8_t *table)
{
    int j;
    int out = 0;
    int c;
    int t;
    
    for (j = 0; j < height; j++){
        out = 0;
        while (out < width){
            c = ir2_get_code(&ctx->gb);
            if(c > 0x80) { /
                c -= 0x80;
                out += c * 2;
            } else { /
                t = dst[out] + (table[c * 2] - 128);
                CLAMP_TO_BYTE(t);
                dst[out] = t;
                out++;
                t = dst[out] + (table[(c * 2) + 1] - 128);
                CLAMP_TO_BYTE(t);
                dst[out] = t;
                out++;
            }
        }
        dst += stride;
    }
}
