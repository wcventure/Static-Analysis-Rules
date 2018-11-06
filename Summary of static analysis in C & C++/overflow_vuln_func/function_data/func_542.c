static guint
get_encoded_strval(tvbuff_t *tvb, guint offset, char *strval)
{
    guint	 field;
    guint	 length;
    guint	 count;

    field = tvb_get_guint8(tvb, offset);

    if (field < 32) {
	length = get_value_length(tvb, offset, &count);
	/
	tvb_memcpy(tvb, strval, offset + count + 1, length - 1);
	strval[length - 1] = '\0';	/
	return count + length;
    } else
	return get_text_string(tvb, offset, strval);
}
