static int rtps_util_add_bitmap(proto_tree *tree,
                        tvbuff_t * tvb,
                        gint       offset,
                        gboolean   little_endian,
                        const char *label _U_) {
  guint64 seq_base;
  gint32 num_bits;
  guint32 data;
  char temp_buff[MAX_BITMAP_SIZE];
  int i, j, idx;
  proto_item * ti;
  proto_tree * bitmap_tree;
  const gint original_offset = offset;
  guint32 datamask;

  /
  seq_base = rtps_util_add_seq_number(NULL,
                        tvb,
                        offset,
                        little_endian,
                        NULL);
  offset += 8;

  /
  num_bits = NEXT_guint32(tvb, offset, little_endian);

  offset += 4;

  /
  idx = 0;
  temp_buff[0] = '\0';
  for (i = 0; i < num_bits; i += 32) {
    data = NEXT_guint32(tvb, offset, little_endian);
    offset += 4;
    for (j = 0; j < 32; ++j) {
      datamask = (1 << (31-j));
      temp_buff[idx] = ((data & datamask) == datamask) ? '1':'0';
      ++idx;
      if (idx >= num_bits) {
        break;
      }
      if (idx >= MAX_BITMAP_SIZE-1) {
        break;
      }
    }
  }
  temp_buff[idx] = '\0';

  /
  for (i = (int)strlen(temp_buff) - 1; (i>0 && temp_buff[i] == '0'); --i) {
      temp_buff[i] = '\0';
  }

  if (tree) {
    ti = proto_tree_add_text(tree,
                          tvb,
                          original_offset,
                          offset-original_offset,
                          "%s: %" G_GINT64_MODIFIER "u/%d:%s",
                          label,
                          seq_base,
                          num_bits,
                          temp_buff);
    bitmap_tree = proto_item_add_subtree(ti, ett_rtps_bitmap);
    proto_tree_add_text(bitmap_tree,
                          tvb,
                          original_offset,
                          8,
                          "bitmapBase: %" G_GINT64_MODIFIER "u",
                          seq_base);
    proto_tree_add_text(bitmap_tree,
                          tvb,
                          original_offset + 8,
                          4,
                          "numBits: %u",
                          num_bits);
    if (temp_buff[0] != '\0') {
      proto_tree_add_text(bitmap_tree,
                          tvb,
                          original_offset + 12,
                          offset - original_offset - 12,
                          "bitmap: %s",
                          temp_buff);
    }
  }
  return offset;
}
