static void set_help_text(GtkWidget *w, help_type_t type)
{

#define BUFF_LEN 4096
#define B_LEN    256
  char buffer[BUFF_LEN];
  header_field_info *hfinfo;
  int i, len, maxlen = 0, maxlen2 = 0, maxlen3 = 0, nb_lines = 0;
  int width, height;
  char *type_name;
  char blanks[B_LEN];

  /

  memset(blanks, ' ', B_LEN - 2);
  blanks[B_LEN-1] = '\0';

  gtk_text_freeze(GTK_TEXT(w));

  switch(type) {

  case OVERVIEW_HELP :
    set_text(w, overview_help, -1);
    break;

  case PROTOCOL_HELP :    
    /
    for (i = 0; i < proto_registrar_n() ; i++) {
      if (proto_registrar_is_protocol(i)) {
	hfinfo = proto_registrar_get_nth(i);
	if ((len = strlen(hfinfo->abbrev)) > maxlen)
	  maxlen = len;
      }
    }

    maxlen++;

    maxlen2 = strlen(proto_help);
    width = gdk_string_width(m_r_font, proto_help);
    set_text(w, proto_help, maxlen2);
    
    /
    for (i = 0; i < proto_registrar_n() ; i++) {
      if (proto_registrar_is_protocol(i)) {
	int blks;
	hfinfo = proto_registrar_get_nth(i);
	blks = maxlen - strlen(hfinfo->abbrev);
	snprintf(buffer, BUFF_LEN, "%s%s%s\n",
		 hfinfo->abbrev,
		 &blanks[B_LEN - blks - 2],
		 hfinfo->name);
	if ((len = strlen(buffer)) > maxlen2) {
	  maxlen2 = len;
	  if ((len = gdk_string_width(m_r_font, buffer)) > width)
	    width = len;
	}
	set_text(w, buffer, strlen(buffer));
	nb_lines++;
      }
    }

    height = (2 + nb_lines) * (m_r_font->ascent + m_r_font->descent);
    gtk_widget_set_usize(GTK_WIDGET(w), 20 + width, 20 + height);
    break;

  case DFILTER_HELP  :

    /

    /
    for (i = 0; i < proto_registrar_n() ; i++) {
      if (!proto_registrar_is_protocol(i)) {
	hfinfo = proto_registrar_get_nth(i);
	if ((len = strlen(hfinfo->abbrev)) > maxlen)
	  maxlen = len;
	if ((len = strlen(hfinfo->name)) > maxlen2)
	  maxlen2 = len;
      }
    }
    maxlen++;
    maxlen2++;

    maxlen3 = strlen(dfilter_help);
    width = gdk_string_width(m_r_font, dfilter_help);
    set_text(w, dfilter_help, maxlen3);

    for (i = 0; i < proto_registrar_n() ; i++) {
      hfinfo = proto_registrar_get_nth(i);	
      if (proto_registrar_is_protocol(i)) {
	snprintf(buffer, BUFF_LEN, "\n%s:\n", hfinfo->name);
	set_text(w, buffer, strlen(buffer));
	nb_lines += 2;
      } else {

	/
	switch(hfinfo->type) {
	case FT_NONE:
	  type_name = "FT_NONE";
	  break;
	case FT_BOOLEAN:
	  type_name = "FT_BOOLEAN";
	  break;
	case FT_UINT8:
	  type_name = "FT_UINT8";
	  break;
	case FT_UINT16:
	  type_name = "FT_UINT16";
	  break;
	case FT_UINT24:
	  type_name = "FT_UINT24";
	  break;
	case FT_UINT32:
	  type_name = "FT_UINT32";
	  break;
	case FT_INT8:
	  type_name = "FT_INT8";
	  break;
	case FT_INT16:
	  type_name = "FT_INT16";
	  break;
	case FT_INT24:
	  type_name = "FT_INT24";
	  break;
	case FT_INT32:
	  type_name = "FT_INT32";
	  break;
	case FT_DOUBLE:
	  type_name = "FT_DOUBLE";
	  break;
	case FT_ABSOLUTE_TIME:
	  type_name = "FT_ABSOLUTE_TIME";
	  break;
	case FT_RELATIVE_TIME:
	  type_name = "FT_RELATIVE_TIME";
	  break;
	case FT_NSTRING_UINT8:
	  type_name = "FT_NSTRING_UINT8";
	  break;
	case FT_STRING:
	  type_name = "FT_STRING";
	  break;
	case FT_ETHER:
	  type_name = "FT_ETHER";
	  break;
	case FT_BYTES:
	  type_name = "FT_BYTES";
	  break;
	case FT_IPv4:
	  type_name = "FT_IPv4";
	  break;
	case FT_IPv6:
	  type_name = "FT_IPv6";
	  break;
	case FT_IPXNET:
	  type_name = "FT_IPXNET";
	  break;
	case FT_TEXT_ONLY:
	  type_name = "FT_TEXT_ONLY";
	  break;
	default:
	  g_assert_not_reached();
	  type_name = NULL;
	}
	snprintf(buffer, BUFF_LEN, "%s%s%s%s(%s)\n",
		 hfinfo->abbrev, 
		 &blanks[B_LEN - (maxlen - strlen(hfinfo->abbrev)) - 2],
		 hfinfo->name,
		 &blanks[B_LEN - (maxlen2 - strlen(hfinfo->name)) - 2],
		 type_name);
	if ((len = strlen(buffer)) > maxlen3) {
	  maxlen3 = len;
	  if ((len = gdk_string_width(m_r_font, buffer)) > width)
	    width = len;
	}
	set_text(w, buffer, strlen(buffer));
	nb_lines ++;
      }

      height = (1 + nb_lines) * (m_r_font->ascent + m_r_font->descent);
      gtk_widget_set_usize(GTK_WIDGET(w), 20 + width, 20 + height);

    }
    break;
  case CFILTER_HELP :
    set_text(w, cfilter_help, -1);
    break;
  default :
    g_assert_not_reached();
    break;
  } /

  gtk_text_thaw(GTK_TEXT(w));

} /
