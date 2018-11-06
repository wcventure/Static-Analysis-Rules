static guint
get_text_string(tvbuff_t *tvb, guint offset, char *strval)
{
    guint	 len;

    len = tvb_strsize(tvb, offset);
    if (tvb_get_guint8(tvb, offset) == MM_QUOTE)
	tvb_memcpy(tvb, strval, offset + 1, len - 1);
    else
	tvb_memcpy(tvb, strval, offset, len);
    return len;
}
