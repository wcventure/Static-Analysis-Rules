static guint32 erf_meta_read_tag(struct erf_meta_tag* tag, guint8 *tag_ptr, guint32 remaining_len)
{
  guint16 tagtype;
  guint16 taglength;

  if (!tag_ptr || !tag || remaining_len < ERF_META_TAG_HEADERLEN)
    return 0;

  /
  tagtype = pntoh16(&tag_ptr[0]);

  /
  taglength = pntoh16(&tag_ptr[2]);

  if (remaining_len < (guint16) ERF_META_TAG_ALIGNED_LENGTH(taglength)) {
    return 0;
  }

  tag->type = tagtype;
  tag->length = taglength;
  tag->value = &tag_ptr[4];

  return ERF_META_TAG_ALIGNED_LENGTH(tag->length);
}
