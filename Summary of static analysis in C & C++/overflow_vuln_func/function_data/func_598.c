static guint32 size_unit_combo_box_convert_value(
  GtkWidget *unit_combo_box,
guint32 value)
{
  int unit;

  unit = gtk_combo_box_get_active (GTK_COMBO_BOX(unit_combo_box));

  switch(unit) {
  case(SIZE_UNIT_KILOBYTES):
    return value;
  case(SIZE_UNIT_MEGABYTES):
    if (value > G_MAXINT / 1024) {
      return 0;
    } else {
      return value * 1024;
    }
  case(SIZE_UNIT_GIGABYTES):
    if (value > G_MAXINT / (1024 * 1024)) {
      return 0;
    } else {
      return value * 1024 * 1024;
    }
  default:
    g_assert_not_reached();
    return 0;
  }
}
