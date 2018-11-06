static char *
vendor_id_to_string(gint offset, tvbuff_t *tvb, char *buff)
{
  guint8              major, minor;
  VendorId            vendorId_rti;

  VENDOR_ID_RTI(vendorId_rti);

  major = tvb_get_guint8(tvb, offset);
  minor = tvb_get_guint8(tvb, (offset+1));

  if (major == vendorId_rti.major &&
      minor == vendorId_rti.minor)
  { sprintf(buff,"Real-Time Innovations,Inc.,CA,USA");
    return(buff); }

  sprintf(buff,"Vendor unknown");
  return(buff);
}
