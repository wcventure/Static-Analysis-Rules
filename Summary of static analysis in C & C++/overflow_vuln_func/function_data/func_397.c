static double getOlsrTime(guint8 timeval) {
  int high_bits, low_bits;

  high_bits = ((timeval & 0xF0) >> 4);
  low_bits = (timeval & 0x0F);
  return ((1 << low_bits) / 16.0) * (1 + (high_bits / 16.0));
}
