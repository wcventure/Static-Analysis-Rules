static int mov_text_tx3g(AVCodecContext *avctx, MovTextContext *m)
{
    char *tx3g_ptr = avctx->extradata;
    int i, box_size, font_length;
    int8_t v_align, h_align;
    int style_fontID;
    StyleBox s_default;

    m->count_f = 0;
    m->ftab_entries = 0;
    box_size = BOX_SIZE_INITIAL; /
    if (avctx->extradata_size < box_size)
        return -1;

    // Display Flags
    tx3g_ptr += 4;
    // Alignment
    h_align = *tx3g_ptr++;
    v_align = *tx3g_ptr++;
    if (h_align == 0) {
        if (v_align == 0)
            m->d.alignment = TOP_LEFT;
        if (v_align == 1)
            m->d.alignment = MIDDLE_LEFT;
        if (v_align == -1)
            m->d.alignment = BOTTOM_LEFT;
    }
    if (h_align == 1) {
        if (v_align == 0)
            m->d.alignment = TOP_CENTER;
        if (v_align == 1)
            m->d.alignment = MIDDLE_CENTER;
        if (v_align == -1)
            m->d.alignment = BOTTOM_CENTER;
    }
    if (h_align == -1) {
        if (v_align == 0)
            m->d.alignment = TOP_RIGHT;
        if (v_align == 1)
            m->d.alignment = MIDDLE_RIGHT;
        if (v_align == -1)
            m->d.alignment = BOTTOM_RIGHT;
    }
    // Background Color
    m->d.back_color = AV_RB24(tx3g_ptr);
    tx3g_ptr += 4;
    // BoxRecord
    tx3g_ptr += 8;
    // StyleRecord
    tx3g_ptr += 4;
    // fontID
    style_fontID = AV_RB16(tx3g_ptr);
    tx3g_ptr += 2;
    // face-style-flags
    s_default.style_flag = *tx3g_ptr++;
    m->d.bold = s_default.style_flag & STYLE_FLAG_BOLD;
    m->d.italic = s_default.style_flag & STYLE_FLAG_ITALIC;
    m->d.underline = s_default.style_flag & STYLE_FLAG_UNDERLINE;
    // fontsize
    m->d.fontsize = *tx3g_ptr++;
    // Primary color
    m->d.color = AV_RB24(tx3g_ptr);
    tx3g_ptr += 4;
    // FontRecord
    // FontRecord Size
    tx3g_ptr += 4;
    // ftab
    tx3g_ptr += 4;

    m->ftab_entries = AV_RB16(tx3g_ptr);
    tx3g_ptr += 2;

    for (i = 0; i < m->ftab_entries; i++) {

        box_size += 3;
        if (avctx->extradata_size < box_size) {
            mov_text_cleanup_ftab(m);
            m->ftab_entries = 0;
            return -1;
        }
        m->ftab_temp = av_malloc(sizeof(*m->ftab_temp));
        if (!m->ftab_temp) {
            mov_text_cleanup_ftab(m);
            return AVERROR(ENOMEM);
        }
        m->ftab_temp->fontID = AV_RB16(tx3g_ptr);
        tx3g_ptr += 2;
        font_length = *tx3g_ptr++;

        box_size = box_size + font_length;
        if (avctx->extradata_size < box_size) {
            mov_text_cleanup_ftab(m);
            m->ftab_entries = 0;
            return -1;
        }
        m->ftab_temp->font = av_malloc(font_length + 1);
        if (!m->ftab_temp->font) {
            mov_text_cleanup_ftab(m);
            return AVERROR(ENOMEM);
        }
        memcpy(m->ftab_temp->font, tx3g_ptr, font_length);
        m->ftab_temp->font[font_length] = '\0';
        av_dynarray_add(&m->ftab, &m->count_f, m->ftab_temp);
        if (!m->ftab) {
            mov_text_cleanup_ftab(m);
            return AVERROR(ENOMEM);
        }
        tx3g_ptr = tx3g_ptr + font_length;
    }
    for (i = 0; i < m->ftab_entries; i++) {
        if (style_fontID == m->ftab[i]->fontID)
            m->d.font = m->ftab[i]->font;
    }
    return 0;
}
