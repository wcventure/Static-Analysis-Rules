static int
fTagHeader (tvbuff_t *tvb, guint *offset, guint8 *tag_no, guint8* class_tag, guint32 *lvt)
{
	int tmp, retVal = 0;
	guint32 lv;
	int i;

	tmp = tvb_get_guint8(tvb, *offset);
	*class_tag = tmp & 0x08;
	lv = tmp & 0x07;
	*tag_no = tmp >> 4;
	if (*tag_no == 15) { /
		*tag_no = tvb_get_guint8(tvb, (*offset)+1);
		retVal++;
	}
	if (lv == 5) { /
		lv = tvb_get_guint8(tvb, (*offset)+retVal+1);
		retVal++;
		if (lv == 254) { /
			lv = 0;
			for (i = 0; i < 2; i++) {
				lv = (lv << 8) + tvb_get_guint8(tvb, (*offset)+retVal+1);
				retVal++;
			}
		} else if (lv == 255) { /
			lv = 0;
			for (i = 0; i < 4; i++) {
				lv = (lv << 8) + tvb_get_guint8(tvb, (*offset)+retVal+1);
				retVal++;
			}
		}
	}
	*lvt = lv;

	return retVal;
}
