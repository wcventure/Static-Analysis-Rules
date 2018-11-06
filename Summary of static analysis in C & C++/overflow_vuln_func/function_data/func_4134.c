static void idcin_decode_vlcs(IdcinContext *s)
{
    hnode_t *hnodes;
    long x, y;
    int prev;
    unsigned char v = 0;
    int bit_pos, node_num, dat_pos;

    prev = bit_pos = dat_pos = 0;
    for (y = 0; y < (s->frame.linesize[0] * s->avctx->height);
        y += s->frame.linesize[0]) {
        for (x = y; x < y + s->avctx->width; x++) {
            node_num = s->num_huff_nodes[prev];
            hnodes = s->huff_nodes[prev];

            while(node_num >= HUF_TOKENS) {
                if(!bit_pos) {
                    if(dat_pos > s->size) {
                        av_log(s->avctx, AV_LOG_ERROR, "Huffman decode error.\n");
                        return;
                    }
                    bit_pos = 8;
                    v = s->buf[dat_pos++];
                }

                node_num = hnodes[node_num].children[v & 0x01];
                v = v >> 1;
                bit_pos--;
            }

            s->frame.data[0][x] = node_num;
            prev = node_num;
        }
    }
}
