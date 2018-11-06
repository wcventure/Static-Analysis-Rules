static int xan_wc3_decode_frame(XanContext *s) {

    int width = s->avctx->width;
    int height = s->avctx->height;
    int total_pixels = width * height;
    unsigned char opcode;
    unsigned char flag = 0;
    int size = 0;
    int motion_x, motion_y;
    int x, y;

    unsigned char *opcode_buffer = s->buffer1;
    int opcode_buffer_size = s->buffer1_size;
    const unsigned char *imagedata_buffer = s->buffer2;

    /
    const unsigned char *huffman_segment;
    const unsigned char *size_segment;
    const unsigned char *vector_segment;
    const unsigned char *imagedata_segment;
    int huffman_offset, size_offset, vector_offset, imagedata_offset;

    if (s->size < 8)
        return AVERROR_INVALIDDATA;

    huffman_offset    = AV_RL16(&s->buf[0]);
    size_offset       = AV_RL16(&s->buf[2]);
    vector_offset     = AV_RL16(&s->buf[4]);
    imagedata_offset  = AV_RL16(&s->buf[6]);

    if (huffman_offset   >= s->size ||
        size_offset      >= s->size ||
        vector_offset    >= s->size ||
        imagedata_offset >= s->size)
        return AVERROR_INVALIDDATA;

    huffman_segment   = s->buf + huffman_offset;
    size_segment      = s->buf + size_offset;
    vector_segment    = s->buf + vector_offset;
    imagedata_segment = s->buf + imagedata_offset;

    if (xan_huffman_decode(opcode_buffer, opcode_buffer_size,
                           huffman_segment, s->size - huffman_offset) < 0)
        return AVERROR_INVALIDDATA;

    if (imagedata_segment[0] == 2)
        xan_unpack(s->buffer2, &imagedata_segment[1], s->buffer2_size);
    else
        imagedata_buffer = &imagedata_segment[1];

    /
    x = y = 0;
    while (total_pixels) {

        opcode = *opcode_buffer++;
        size = 0;

        switch (opcode) {

        case 0:
            flag ^= 1;
            continue;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            size = opcode;
            break;

        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            size += (opcode - 10);
            break;

        case 9:
        case 19:
            size = *size_segment++;
            break;

        case 10:
        case 20:
            size = AV_RB16(&size_segment[0]);
            size_segment += 2;
            break;

        case 11:
        case 21:
            size = AV_RB24(size_segment);
            size_segment += 3;
            break;
        }

        if (opcode < 12) {
            flag ^= 1;
            if (flag) {
                /
                xan_wc3_copy_pixel_run(s, x, y, size, 0, 0);
            } else {
                /
                xan_wc3_output_pixel_run(s, imagedata_buffer, x, y, size);
                imagedata_buffer += size;
            }
        } else {
            /
            motion_x = sign_extend(*vector_segment >> 4,  4);
            motion_y = sign_extend(*vector_segment & 0xF, 4);
            vector_segment++;

            /
            xan_wc3_copy_pixel_run(s, x, y, size, motion_x, motion_y);

            flag = 0;
        }

        /
        total_pixels -= size;
        y += (x + size) / width;
        x  = (x + size) % width;
    }
    return 0;
}
