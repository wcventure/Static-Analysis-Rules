static void listOfByte(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      if (*offsetp + length > next_offset) {
	    /
	    length = next_offset -  *offsetp;
      }
      if (length <= 0) length = 1;
      proto_tree_add_item(t, hf, tvb, *offsetp, length, little_endian);
      *offsetp += length;
}
