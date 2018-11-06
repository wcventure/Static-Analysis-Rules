static void rtps_util_add_extra_flags(proto_tree *tree,
                        tvbuff_t *tvb,
                        gint offset,
                        const char *label) {
  if (tree) {
    guint16 flags = NEXT_guint16(tvb, offset, FALSE); /
    guint8  temp_buffer[20];
    int i;
    for (i = 0; i < 16; ++i) {
      temp_buffer[i] = ((flags & (1 << (15-i))) != 0) ? '1' : '0';
    }
    temp_buffer[16] = '\0';

    proto_tree_add_text(tree,
                        tvb,
                        offset,
                        2,
                        "%s: %s",
                        label,
                        temp_buffer);
  }
}
