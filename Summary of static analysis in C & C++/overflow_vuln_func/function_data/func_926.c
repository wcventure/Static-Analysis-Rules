static int rtps_util_add_fragment_number_set(proto_tree *tree,
                        tvbuff_t * tvb,
                        gint       offset,
                        gboolean   little_endian,
                        const char *label _U_,
                        gint       section_size) {
  guint64 base;
  gint32 num_bits;
  guint32 data;
  char temp_buff[MAX_BITMAP_SIZE];
  int i, j, idx;
  proto_item * ti;
  proto_tree * bitmap_tree;
  const gint original_offset = offset;
  guint32 datamask;
  gint expected_size;
  gint base_size;

  /
  num_bits = NEXT_guint32(tvb, offset+4, little_endian);
  expected_size = (((num_bits / 8) + 3) / 4) * 4 + 8;
  if (expected_size == section_size) {
    base = (guint64)NEXT_guint32(tvb, offset, little_endian);
    base_size = 4;
    offset += 8;
  } else {
    /
    num_bits = NEXT_guint32(tvb, offset+8, little_endian);
    /
    expected_size = (((num_bits / 8) + 3) / 4) * 4 + 12;
    if (expected_size == section_size) {
      guint64 hi = (guint64)NEXT_guint32(tvb, offset, little_endian);
      guint64 lo = (guint64)NEXT_guint32(tvb, offset+4, little_endian);
      base = (hi << 32) | lo;
      base_size = 8;
      offset += 12;
    } else {
      /
      if (tree) {
        proto_tree_add_text(tree,
                        tvb,
                        original_offset,
                        section_size,
                        "Packet malformed: illegal size for fragment number set");
      }
      return -1;
    }
  }

  /
  idx = 0;
  temp_buff[0] = '\0';
  for (i = 0; i < num_bits; i += 32) {
    data = NEXT_guint32(tvb, offset, little_endian);
    offset += 4;
    for (j = 0; j < 32; ++j) {
      datamask = (1 << (31-j));
      temp_buff[idx] = ((data & datamask) != 0) ? '1':'0';
      ++idx;
      if (idx > num_bits) {
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
                        base,
                        num_bits,
                        temp_buff);
    bitmap_tree = proto_item_add_subtree(ti, ett_rtps_bitmap);
    proto_tree_add_text(bitmap_tree,
                        tvb,
                        original_offset,
                        base_size,
                        "bitmapBase: %" G_GINT64_MODIFIER "u",
                        base);
    proto_tree_add_text(bitmap_tree,
                        tvb,
                        original_offset + base_size,
                        4,
                        "numBits: %u",
                        num_bits);
    if (temp_buff[0] != '\0') {
      proto_tree_add_text(bitmap_tree,
                        tvb,
                        original_offset + base_size + 4,
                        offset - original_offset - base_size - 4,
                        "bitmap: %s",
                        temp_buff);
    }
  }
  return offset;
}
