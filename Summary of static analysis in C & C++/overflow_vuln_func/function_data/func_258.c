static float decode_half(const int half) {
	int exponent = (half >> 10) & 0x1f;
	int mantissa = half & 0x3ff;
	float val;

	if (exponent == 0)
		val = ldexpf((float)mantissa, -24);
	else if (exponent != 31)
		val = ldexpf((float)(mantissa + 1024), exponent - 25);
	else {
		if (mantissa == 0)
			val = INFINITY;
		else
			val = NAN;
	}
	return half & 0x8000 ? -val : val;
}
