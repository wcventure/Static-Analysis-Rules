av_cold int ff_yuv2rgb_c_init_tables(SwsContext *c, const int inv_table[4], int fullRange,
                                     int brightness, int contrast, int saturation)
{
    const int isRgb =      c->dstFormat==PIX_FMT_RGB32
                        || c->dstFormat==PIX_FMT_RGB32_1
                        || c->dstFormat==PIX_FMT_BGR24
                        || c->dstFormat==PIX_FMT_RGB565BE
                        || c->dstFormat==PIX_FMT_RGB565LE
                        || c->dstFormat==PIX_FMT_RGB555BE
                        || c->dstFormat==PIX_FMT_RGB555LE
                        || c->dstFormat==PIX_FMT_RGB444BE
                        || c->dstFormat==PIX_FMT_RGB444LE
                        || c->dstFormat==PIX_FMT_RGB8
                        || c->dstFormat==PIX_FMT_RGB4
                        || c->dstFormat==PIX_FMT_RGB4_BYTE
                        || c->dstFormat==PIX_FMT_MONOBLACK;
    const int isNotNe =    c->dstFormat==PIX_FMT_NE(RGB565LE,RGB565BE)
                        || c->dstFormat==PIX_FMT_NE(RGB555LE,RGB555BE)
                        || c->dstFormat==PIX_FMT_NE(RGB444LE,RGB444BE)
                        || c->dstFormat==PIX_FMT_NE(BGR565LE,BGR565BE)
                        || c->dstFormat==PIX_FMT_NE(BGR555LE,BGR555BE)
                        || c->dstFormat==PIX_FMT_NE(BGR444LE,BGR444BE);
    const int bpp = c->dstFormatBpp;
    uint8_t *y_table;
    uint16_t *y_table16;
    uint32_t *y_table32;
    int i, base, rbase, gbase, bbase, abase, needAlpha;
    const int yoffs = fullRange ? 384 : 326;

    int64_t crv =  inv_table[0];
    int64_t cbu =  inv_table[1];
    int64_t cgu = -inv_table[2];
    int64_t cgv = -inv_table[3];
    int64_t cy  = 1<<16;
    int64_t oy  = 0;

    int64_t yb = 0;

    if (!fullRange) {
        cy = (cy*255) / 219;
        oy = 16<<16;
    } else {
        crv = (crv*224) / 255;
        cbu = (cbu*224) / 255;
        cgu = (cgu*224) / 255;
        cgv = (cgv*224) / 255;
    }

    cy  = (cy *contrast             ) >> 16;
    crv = (crv*contrast * saturation) >> 32;
    cbu = (cbu*contrast * saturation) >> 32;
    cgu = (cgu*contrast * saturation) >> 32;
    cgv = (cgv*contrast * saturation) >> 32;
    oy -= 256*brightness;

    c->uOffset=   0x0400040004000400LL;
    c->vOffset=   0x0400040004000400LL;
    c->yCoeff=    roundToInt16(cy *8192) * 0x0001000100010001ULL;
    c->vrCoeff=   roundToInt16(crv*8192) * 0x0001000100010001ULL;
    c->ubCoeff=   roundToInt16(cbu*8192) * 0x0001000100010001ULL;
    c->vgCoeff=   roundToInt16(cgv*8192) * 0x0001000100010001ULL;
    c->ugCoeff=   roundToInt16(cgu*8192) * 0x0001000100010001ULL;
    c->yOffset=   roundToInt16(oy *   8) * 0x0001000100010001ULL;

    c->yuv2rgb_y_coeff  = (int16_t)roundToInt16(cy <<13);
    c->yuv2rgb_y_offset = (int16_t)roundToInt16(oy << 9);
    c->yuv2rgb_v2r_coeff= (int16_t)roundToInt16(crv<<13);
    c->yuv2rgb_v2g_coeff= (int16_t)roundToInt16(cgv<<13);
    c->yuv2rgb_u2g_coeff= (int16_t)roundToInt16(cgu<<13);
    c->yuv2rgb_u2b_coeff= (int16_t)roundToInt16(cbu<<13);

    //scale coefficients by cy
    crv = ((crv << 16) + 0x8000) / cy;
    cbu = ((cbu << 16) + 0x8000) / cy;
    cgu = ((cgu << 16) + 0x8000) / cy;
    cgv = ((cgv << 16) + 0x8000) / cy;

    av_free(c->yuvTable);

    switch (bpp) {
    case 1:
        c->yuvTable = av_malloc(1024);
        y_table = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024-110; i++) {
            y_table[i+110] = av_clip_uint8((yb + 0x8000) >> 16) >> 7;
            yb += cy;
        }
        fill_table(c->table_gU, 1, cgu, y_table + yoffs);
        fill_gv_table(c->table_gV, 1, cgv);
        break;
    case 4:
    case 4|128:
        rbase = isRgb ? 3 : 0;
        gbase = 1;
        bbase = isRgb ? 0 : 3;
        c->yuvTable = av_malloc(1024*3);
        y_table = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024-110; i++) {
            int yval = av_clip_uint8((yb + 0x8000) >> 16);
            y_table[i+110     ] =  (yval >> 7)       << rbase;
            y_table[i+ 37+1024] = ((yval + 43) / 85) << gbase;
            y_table[i+110+2048] =  (yval >> 7)       << bbase;
            yb += cy;
        }
        fill_table(c->table_rV, 1, crv, y_table + yoffs);
        fill_table(c->table_gU, 1, cgu, y_table + yoffs + 1024);
        fill_table(c->table_bU, 1, cbu, y_table + yoffs + 2048);
        fill_gv_table(c->table_gV, 1, cgv);
        break;
    case 8:
        rbase = isRgb ? 5 : 0;
        gbase = isRgb ? 2 : 3;
        bbase = isRgb ? 0 : 6;
        c->yuvTable = av_malloc(1024*3);
        y_table = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024-38; i++) {
            int yval = av_clip_uint8((yb + 0x8000) >> 16);
            y_table[i+16     ] = ((yval + 18) / 36) << rbase;
            y_table[i+16+1024] = ((yval + 18) / 36) << gbase;
            y_table[i+37+2048] = ((yval + 43) / 85) << bbase;
            yb += cy;
        }
        fill_table(c->table_rV, 1, crv, y_table + yoffs);
        fill_table(c->table_gU, 1, cgu, y_table + yoffs + 1024);
        fill_table(c->table_bU, 1, cbu, y_table + yoffs + 2048);
        fill_gv_table(c->table_gV, 1, cgv);
        break;
    case 12:
        rbase = isRgb ? 8 : 0;
        gbase = 4;
        bbase = isRgb ? 0 : 8;
        c->yuvTable = av_malloc(1024*3*2);
        y_table16 = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024; i++) {
            uint8_t yval = av_clip_uint8((yb + 0x8000) >> 16);
            y_table16[i     ] = (yval >> 4) << rbase;
            y_table16[i+1024] = (yval >> 4) << gbase;
            y_table16[i+2048] = (yval >> 4) << bbase;
            yb += cy;
        }
        if (isNotNe)
            for (i = 0; i < 1024*3; i++)
                y_table16[i] = av_bswap16(y_table16[i]);
        fill_table(c->table_rV, 2, crv, y_table16 + yoffs);
        fill_table(c->table_gU, 2, cgu, y_table16 + yoffs + 1024);
        fill_table(c->table_bU, 2, cbu, y_table16 + yoffs + 2048);
        fill_gv_table(c->table_gV, 2, cgv);
        break;
    case 15:
    case 16:
        rbase = isRgb ? bpp - 5 : 0;
        gbase = 5;
        bbase = isRgb ? 0 : (bpp - 5);
        c->yuvTable = av_malloc(1024*3*2);
        y_table16 = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024; i++) {
            uint8_t yval = av_clip_uint8((yb + 0x8000) >> 16);
            y_table16[i     ] = (yval >> 3)          << rbase;
            y_table16[i+1024] = (yval >> (18 - bpp)) << gbase;
            y_table16[i+2048] = (yval >> 3)          << bbase;
            yb += cy;
        }
        if(isNotNe)
            for (i = 0; i < 1024*3; i++)
                y_table16[i] = av_bswap16(y_table16[i]);
        fill_table(c->table_rV, 2, crv, y_table16 + yoffs);
        fill_table(c->table_gU, 2, cgu, y_table16 + yoffs + 1024);
        fill_table(c->table_bU, 2, cbu, y_table16 + yoffs + 2048);
        fill_gv_table(c->table_gV, 2, cgv);
        break;
    case 24:
    case 48:
        c->yuvTable = av_malloc(1024);
        y_table = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024; i++) {
            y_table[i] = av_clip_uint8((yb + 0x8000) >> 16);
            yb += cy;
        }
        fill_table(c->table_rV, 1, crv, y_table + yoffs);
        fill_table(c->table_gU, 1, cgu, y_table + yoffs);
        fill_table(c->table_bU, 1, cbu, y_table + yoffs);
        fill_gv_table(c->table_gV, 1, cgv);
        break;
    case 32:
        base = (c->dstFormat == PIX_FMT_RGB32_1 || c->dstFormat == PIX_FMT_BGR32_1) ? 8 : 0;
        rbase = base + (isRgb ? 16 : 0);
        gbase = base + 8;
        bbase = base + (isRgb ? 0 : 16);
        needAlpha = CONFIG_SWSCALE_ALPHA && isALPHA(c->srcFormat);
        if (!needAlpha)
            abase = (base + 24) & 31;
        c->yuvTable = av_malloc(1024*3*4);
        y_table32 = c->yuvTable;
        yb = -(384<<16) - oy;
        for (i = 0; i < 1024; i++) {
            uint8_t yval = av_clip_uint8((yb + 0x8000) >> 16);
            y_table32[i     ] = (yval << rbase) + (needAlpha ? 0 : (255 << abase));
            y_table32[i+1024] = yval << gbase;
            y_table32[i+2048] = yval << bbase;
            yb += cy;
        }
        fill_table(c->table_rV, 4, crv, y_table32 + yoffs);
        fill_table(c->table_gU, 4, cgu, y_table32 + yoffs + 1024);
        fill_table(c->table_bU, 4, cbu, y_table32 + yoffs + 2048);
        fill_gv_table(c->table_gV, 4, cgv);
        break;
    default:
        c->yuvTable = NULL;
        av_log(c, AV_LOG_ERROR, "%ibpp not supported by yuv2rgb\n", bpp);
        return -1;
    }
    return 0;
}
