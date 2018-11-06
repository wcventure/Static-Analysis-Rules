static void
rfc3825_lci_to_fixpoint(const unsigned char lci[16], struct rfc3825_location_fixpoint_t *fixpoint)
{
	fixpoint->latitude_res = (lci[0]>>2) & 0x3F; /
	if (lci[0] & 2) { /
		/
		fixpoint->latitude = (((gint64)lci[0] & 3)<<32) | ((gint64)lci[1]<<24) | 
		                           ((gint64)lci[2]<<16) | ((gint64)lci[3]<<8)  | 
		                            (gint64)lci[4]      | ((gint64)0x3FFFFFFF<<34);

	} else {
		/
		fixpoint->latitude = (((gint64)lci[0] & 3)<<32) | ((gint64)lci[1]<<24) | 
		                           ((gint64)lci[2]<<16) | ((gint64)lci[3]<<8)  | 
		                            (gint64)lci[4];
	}
	fixpoint->longitude_res = (lci[5]>>2) & 0x3F;  /
	if (lci[5] & 2) { /
		/
		fixpoint->longitude = (((gint64)lci[5] & 3)<<32) | ((gint64)lci[6]<<24) | 
		                            ((gint64)lci[7]<<16) | ((gint64)lci[8]<<8)  | 
		                             (gint64)lci[9]      | ((gint64)0x3FFFFFFF<<34);

	} else {
		/
		fixpoint->longitude = (((gint64)lci[5] & 3)<<32) | ((gint64)lci[6]<<24) | 
		                            ((gint64)lci[7]<<16) | ((gint64)lci[8]<<8)  | 
		                             (gint64)lci[9];
	}
	fixpoint->altitude_type = (lci[10]>>4) & 0x0F;  /
	fixpoint->altitude_res  = ((lci[10] & 0x0F) << 2) | ((lci[11]>>6) & 0x03);
	if (lci[11] & 0x20) { /
		/
		fixpoint->altitude = (((gint32)lci[11] & 0x3F)<<24) | ((gint32)lci[12]<<16) | 
		                     ((gint32)lci[13]<<8) | ((gint32)lci[14]) | 
		                      ((gint32)0x03<<30);

	} else {
		/
		fixpoint->altitude = (((gint32)lci[11] & 0x3F)<<24) | ((gint32)lci[12]<<16) | 
		                     ((gint32)lci[13]<<8) | ((gint32)lci[14]);
	}

	fixpoint->datum_type = lci[15];

}
