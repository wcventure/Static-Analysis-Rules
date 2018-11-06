static gboolean
read_filters_file(FILE *f, gpointer arg)
{
	/
	/
	/
	gchar name[256],filter_exp[256], buf[1024];
	guint16 fg_r, fg_g, fg_b, bg_r, bg_g, bg_b;
	GdkColor fg_color, bg_color;
	color_filter_t *colorf;
	dfilter_t *temp_dfilter;

	do {
		if (fgets(buf,sizeof buf, f) == NULL)
			break;

		if (strspn(buf," \t") == (size_t)((strchr(buf,'*') - buf))) {
			/
			continue;
		}

		/
		if (sscanf(buf," @%256[^@]@%256[^@]@[%hu,%hu,%hu][%hu,%hu,%hu]",
		    name, filter_exp, &bg_r, &bg_g, &bg_b, &fg_r, &fg_g, &fg_b)
		    == 8) {
			/

			if (!dfilter_compile(filter_exp, &temp_dfilter)) {
				simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
		"Could not compile color filter %s from saved filters.\n%s",
				    name, dfilter_error_msg);
				continue;
			}
			if (!get_color(&fg_color)) {
				/
				simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
				    "Could not allocate foreground color "
				    "specified in input file for %s.", name);
				dfilter_free(temp_dfilter);
				continue;
			}
			if (!get_color(&bg_color)) {
				/
				simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK,
				    "Could not allocate background color "
				    "specified in input file for %s.", name);
				dfilter_free(temp_dfilter);
				continue;
			}

			colorf = new_color_filter(name, filter_exp);
			colorf->c_colorfilter = temp_dfilter;
			fg_color.red = fg_r;
			fg_color.green = fg_g;
			fg_color.blue = fg_b;
			bg_color.red = bg_r;
			bg_color.green = bg_g;
			bg_color.blue = bg_b;

			gdkcolor_to_color_t(&colorf->bg_color, &bg_color);
			gdkcolor_to_color_t(&colorf->fg_color, &fg_color);

			if (arg != NULL)
				color_add_filter_cb (colorf, arg);
		}    /
	} while(!feof(f));
	return TRUE;
}
