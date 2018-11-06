void
tvb_get_bits_buf(tvbuff_t *tvb, gint bit_offset, gint no_of_bits, guint8 *buf, gboolean lsb0)
{
	guint8 bit_mask, bit_shift;
	/
	gint   offset = bit_offset >> 3;

	bit_offset    = bit_offset & 0x7;

	bit_mask  = (lsb0) ? 0xff : inverse_bit_mask8[bit_offset];
	bit_shift = (lsb0) ? bit_offset : (8 - bit_offset);

	if (G_LIKELY(bit_offset != 0)) {
		guint16 value = (guint16) tvb_get_guint8(tvb, offset);

		while (no_of_bits >= 8) {
			offset++;
			value = ((value & bit_mask) << 8) | tvb_get_guint8(tvb, offset);

			if (lsb0)
				*buf++ = (guint8) (GUINT16_SWAP_LE_BE(value) >> bit_shift);
			else
				*buf++ = (guint8) (value >> bit_shift);
			no_of_bits -= 8;
		}

		/
		if (no_of_bits > 0) {
			guint8 tot_no_bits = bit_offset+no_of_bits;

			/
			if (tot_no_bits > 8) {
				offset++;
				value = ((value & bit_mask) << 8) | tvb_get_guint8(tvb, offset);
			}

			if (lsb0) {
				if (tot_no_bits > 8)
					value = (GUINT16_SWAP_LE_BE(value) >> bit_offset) & (bit_mask8[no_of_bits]);
				else
					value = (value >> bit_offset) & (bit_mask8[no_of_bits]);

				/

			} else {
				if (tot_no_bits > 8)
					value = value >> (16 - tot_no_bits);
				else
					value = (value & bit_mask) >> (8-tot_no_bits);
			}
			*buf = (guint8) value;
		}

	} else {
		/
		while (no_of_bits >= 8) {
			*buf++ = tvb_get_guint8(tvb, offset);
			offset++;
			no_of_bits -= 8;
		}

		/
		if (no_of_bits > 0) {
			if (lsb0)
				*buf = tvb_get_guint8(tvb, offset) & bit_mask8[no_of_bits]; /
			else
				*buf = tvb_get_guint8(tvb, offset) >> (8-no_of_bits);
		}
	}
}
