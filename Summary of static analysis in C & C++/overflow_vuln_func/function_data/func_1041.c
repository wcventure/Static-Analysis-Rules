static int
rfc3825_fixpoint_to_decimal(struct rfc3825_location_fixpoint_t *fixpoint, struct rfc3825_location_decimal_t *decimal)
{
	/
	decimal->latitude = (double) fixpoint->latitude / (1 << 25);
	if ((decimal->latitude > 90) || (decimal->latitude < -90)) {
		return RFC3825_LATITUDE_OUTOFRANGE;
	}

	/
	if (fixpoint->latitude_res > 34) {
		return RFC3825_LATITUDE_UNCERTAINTY_OUTOFRANGE;
	}
	if (fixpoint->latitude_res > 8 ) {
		decimal->latitude_res = (double) 1  / (1 << (fixpoint->latitude_res - 8));
	} else {
		decimal->latitude_res = 1 << (8 - fixpoint->latitude_res);
	}

	/
	decimal->longitude = (double) fixpoint->longitude / (1 << 25);
	if ((decimal->longitude > 180) || (decimal->longitude < -180)) {
		return RFC3825_LONGITUDE_OUTOFRANGE;
	}

	/
	if (fixpoint->longitude_res > 34) {
		return RFC3825_LONGITUDE_UNCERTAINTY_OUTOFRANGE;
	}
	if (fixpoint->longitude_res > 8 ) {
		decimal->longitude_res = (double) 1 / (1 << (fixpoint->longitude_res - 8));
	} else {
		decimal->longitude_res = 1 << (8 - fixpoint->longitude_res);
	}

	/
	decimal->altitude_type = fixpoint->altitude_type;
	decimal->altitude = 0;
	decimal->altitude_res = 0;

	if (decimal->altitude_type == 0) { /
	} else if (decimal->altitude_type == 1) { /
		/
		decimal->altitude = (double) fixpoint->altitude / (1 << 8);
		if ((decimal->altitude > ((gint32) 1<<21)-1) || (decimal->altitude < ((gint32) -(1<<21))))
			return RFC3825_ALTITUDE_OUTOFRANGE;

		/
		if (fixpoint->altitude_res > 30) {
			return RFC3825_ALTITUDE_UNCERTAINTY_OUTOFRANGE;
		}
		if (fixpoint->altitude_res > 21 ) {
			decimal->altitude_res = (double) 1 / (1 << (fixpoint->altitude_res - 21));
		} else {
			decimal->altitude_res = 1 << (21 - fixpoint->altitude_res);
		}
	} else if (decimal->altitude_type == 2) { /
		/
		if ((fixpoint->altitude_res != 30) && (fixpoint->altitude_res != 0)) {
			return RFC3825_ALTITUDE_UNCERTAINTY_OUTOFRANGE;
		}
		decimal->altitude = (double) fixpoint->altitude / (1 << 8);
	} else { /
		return RFC3825_ALTITUDE_TYPE_OUTOFRANGE;
	}

	/
	decimal->datum_type = 0;
	if ((fixpoint->datum_type > 3) || (fixpoint->datum_type < 1)) {
		return RFC3825_DATUM_TYPE_OUTOFRANGE;
	}
	decimal->datum_type = fixpoint->datum_type;

	return RFC3825_NOERROR;
}
