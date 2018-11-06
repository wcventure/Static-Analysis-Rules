	if(parameter>tvb_length_remaining(tvb, offset)){\
		len=tvb_length_remaining(tvb, offset);\
		if (check_col(pinfo->cinfo, COL_INFO)) {\
			col_append_fstr(pinfo->cinfo, COL_INFO, "[Short" x " PDU]");\
		}\
	}
