static guint64
_tvb_get_bits64(tvbuff_t *tvb, gint bit_offset, const gint total_no_of_bits)
{
	guint64 value;
	guint octet_offset = bit_offset >> 3;
	guint8 required_bits_in_first_octet = 8 - (bit_offset % 8);

	if(required_bits_in_first_octet > total_no_of_bits)
	{
		/
		guint8 right_shift = required_bits_in_first_octet - total_no_of_bits;
		value = (tvb_get_guint8(tvb, octet_offset) >> right_shift) & bit_mask8[total_no_of_bits];
	}
	else
	{
		guint8 remaining_bit_length = total_no_of_bits;

		/
		value = 0;
		required_bits_in_first_octet %= 8;
		if(required_bits_in_first_octet != 0)
		{
			value = tvb_get_guint8(tvb, octet_offset) & bit_mask8[required_bits_in_first_octet];
			remaining_bit_length -= required_bits_in_first_octet;
			octet_offset ++;
		}
		/
		while (remaining_bit_length > 7)
		{
			switch (remaining_bit_length >> 4)
			{
			case 0:
				/
				value <<= 8;
				value += tvb_get_guint8(tvb, octet_offset);
				remaining_bit_length -= 8;
				octet_offset ++;
				break;

			case 1:
				/
				value <<= 16;
				value += tvb_get_ntohs(tvb, octet_offset);
				remaining_bit_length -= 16;
				octet_offset += 2;
				break;

			case 2:
			case 3:
				/
				value <<= 32;
				value += tvb_get_ntohl(tvb, octet_offset);
				remaining_bit_length -= 32;
				octet_offset += 4;
				break;

			default:
				/
				value = tvb_get_ntoh64(tvb, octet_offset);
				remaining_bit_length -= 64;
				octet_offset += 8;
				break;
			}
		}
		/
		if(remaining_bit_length)
		{
			value <<= remaining_bit_length;
			value += (tvb_get_guint8(tvb, octet_offset) >> (8 - remaining_bit_length));
		}
	}
	return value;
}
