gint logcat_exported_pdu_length(const guint8 *pd) {
    guint16 *tag;
    guint16 *tag_length;
    gint     length = 0;

    tag = (guint16 *) pd;

    while(GINT16_FROM_BE(*tag)) {
        tag_length = (guint16 *) (pd + 2);
        length += 2 + 2 + GINT16_FROM_BE(*tag_length);

        pd += 2 + 2 + GINT16_FROM_BE(*tag_length);
        tag = (guint16 *) pd;
    }

    length += 2 + 2;

    return length;
}
