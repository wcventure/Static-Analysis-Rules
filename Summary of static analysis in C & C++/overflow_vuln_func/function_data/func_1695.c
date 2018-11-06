static void
fill_lapd_byte_state(lapd_byte_state_t *ptr, enum lapd_bitstream_states state, char full_byte, char bit_offset, int ones, char *data, int data_len)
{
	ptr->state = state;
	ptr->full_byte = full_byte;
	ptr->bit_offset = bit_offset;
	ptr->ones = ones;

	memcpy(ptr->data, data, data_len);
	ptr->data_len = data_len;
}
