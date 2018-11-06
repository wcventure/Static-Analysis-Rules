static void
cms_verify_msg_digest(proto_item *pi, tvbuff_t *content, const char *alg, tvbuff_t *tvb, int offset)
{
  sha1_context sha1_ctx;
  md5_state_t md5_ctx;
  int i= 0, buffer_size = 0;

  /

  if(strcmp(alg, HASH_SHA1) == 0) {

    sha1_starts(&sha1_ctx);

    sha1_update(&sha1_ctx, tvb_get_ptr(content, 0, tvb_captured_length(content)),
		tvb_captured_length(content));

    sha1_finish(&sha1_ctx, digest_buf);

    buffer_size = SHA1_BUFFER_SIZE;

  } else if(strcmp(alg, HASH_MD5) == 0) {

    md5_init(&md5_ctx);

    md5_append(&md5_ctx, tvb_get_ptr(content, 0, tvb_captured_length(content)),
	       tvb_captured_length(content));

    md5_finish(&md5_ctx, digest_buf);

    buffer_size = MD5_BUFFER_SIZE;
  }

  if(buffer_size) {
    /

    if(tvb_bytes_exist(tvb, offset, buffer_size) &&
       (tvb_memeql(tvb, offset, digest_buf, buffer_size) != 0)) {
      proto_item_append_text(pi, " [incorrect, should be ");
      for(i = 0; i < buffer_size; i++)
	proto_item_append_text(pi, "%02X", digest_buf[i]);

      proto_item_append_text(pi, "]");
    }
    else
      proto_item_append_text(pi, " [correct]");
  } else {
    proto_item_append_text(pi, " [unable to verify]");
  }

}
