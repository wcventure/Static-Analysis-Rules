static guint
pref_show(pref_t *pref, gpointer user_data)
{
  GtkWidget *main_tb = user_data;
  const char *title;
  char *label_string;
  char uint_str[10+1];

  /
  title = pref->title;
  label_string = g_malloc(strlen(title) + 2);
  strcpy(label_string, title);
  strcat(label_string, ":");

  /
  switch (pref->type) {

  case PREF_UINT:
    pref->saved_val.uint = *pref->varp.uint;

    /
    switch (pref->info.base) {

    case 10:
      g_snprintf(uint_str, 10+1, "%u", pref->saved_val.uint);
      break;

    case 8:
      g_snprintf(uint_str, 10+1, "%o", pref->saved_val.uint);
      break;

    case 16:
      g_snprintf(uint_str, 10+1, "%x", pref->saved_val.uint);
      break;
    }
    pref->control = create_preference_entry(main_tb, pref->ordinal,
					    label_string, pref->description,
					    uint_str);
    break;

  case PREF_BOOL:
    pref->saved_val.boolval = *pref->varp.boolp;
    pref->control = create_preference_check_button(main_tb, pref->ordinal,
					       label_string, pref->description,
					       pref->saved_val.boolval);
    break;

  case PREF_ENUM:
    pref->saved_val.enumval = *pref->varp.enump;
    if (pref->info.enum_info.radio_buttons) {
      /
      pref->control = create_preference_radio_buttons(main_tb, pref->ordinal,
						  label_string, pref->description,
						  pref->info.enum_info.enumvals,
						  pref->saved_val.enumval);
    } else {
      /
      pref->control = create_preference_option_menu(main_tb, pref->ordinal,
					 label_string, pref->description,
					 pref->info.enum_info.enumvals,
					 pref->saved_val.enumval);
    }
    break;

  case PREF_STRING:
    if (pref->saved_val.string != NULL)
      g_free(pref->saved_val.string);
    pref->saved_val.string = g_strdup(*pref->varp.string);
    pref->control = create_preference_entry(main_tb, pref->ordinal,
					    label_string, pref->description,
					    pref->saved_val.string);
    break;

  case PREF_RANGE:
  {
    char range_string[MAXRANGESTRING];

    if (pref->saved_val.range != NULL)
      g_free(pref->saved_val.range);
    pref->saved_val.range = range_copy(*pref->varp.range);
    range_convert_range(*pref->varp.range, range_string);
    pref->control = create_preference_entry(main_tb, pref->ordinal,
					    label_string, pref->description,
					    range_string);
    break;
  }

  case PREF_OBSOLETE:
    g_assert_not_reached();
    break;
  }
  g_free(label_string);

  return 0;
}
