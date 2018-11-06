static int
dcom_tvb_get_nwstringz0(tvbuff_t *tvb, gint offset, guint32 inLength, gchar *pszStr, guint32 outLength, gboolean *isPrintable)
{
	guint32 u32Idx;
	guint32 u32IdxA;
	guint32 u32IdxW;

	guint8	u8Tmp1;
	guint8	u8Tmp2;


	*isPrintable = TRUE;

	/
	DISSECTOR_ASSERT(outLength >= 1);

	/
	for(u32Idx = 0; u32Idx < inLength-1; u32Idx+=2) {
		/
		u8Tmp1 = tvb_get_guint8(tvb, offset+u32Idx);
		u8Tmp2 = tvb_get_guint8(tvb, offset+u32Idx+1);

		/
		if (u8Tmp1 == 0 && u8Tmp2 == 0) {
			u32Idx+=2;
			break;
		}

		/
		/
		if(!g_ascii_isprint(u8Tmp1) || u8Tmp2 != 0) {
			*isPrintable = FALSE;
		}
	}

	/
	/

	/
	if(*isPrintable == TRUE) {
		/
		/
		for(u32IdxA = 0, u32IdxW = 0;
		    u32IdxW < u32Idx && u32IdxA < outLength-2;
		    u32IdxW+=2, u32IdxA++) {
			pszStr[u32IdxA] = tvb_get_guint8(tvb, offset+u32IdxW);
		}
	} else {
		/
		for(u32IdxA = 0, u32IdxW = 0;
		    u32IdxW < u32Idx && u32IdxA < outLength-2;
		    u32IdxW++, u32IdxA+=2) {
			g_snprintf(&pszStr[u32IdxA], 3, "%02X", tvb_get_guint8(tvb, offset+u32IdxW));
		}
	}

	/
	DISSECTOR_ASSERT(u32IdxA < outLength);
	pszStr[u32IdxA] = 0;

	return offset + u32Idx;
}
