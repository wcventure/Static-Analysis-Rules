static void
io_stat_draw(io_stat_t *io)
{
	int i, tics, ystart, ys;
	guint32 last_interval, first_interval, interval_delta;
	gint32 current_interval;
	guint32 top_y_border;
	guint32 bottom_y_border;
	PangoLayout  *layout;
	int label_width, label_height;
	guint32 draw_width, draw_height;
	char label_string[45];
	GtkAllocation widget_alloc;
	/
	guint32 num_time_intervals; /
	guint64 max_value;		/
	guint32 max_y;			/
	gboolean draw_y_as_time;
	cairo_t *cr;

	if(!io->needs_redraw){
		return;
	}
	io->needs_redraw=FALSE;
	/
	io->max_interval = (guint32)((cfile.elapsed_time.secs*1000) +
				     ((cfile.elapsed_time.nsecs+500000)/1000000) +
				     io->interval);
	io->max_interval = (io->max_interval / io->interval) * io->interval;
	/
	num_time_intervals = io->num_items+1;

	/
	if(num_time_intervals > NUM_IO_ITEMS){
		simple_dialog(ESD_TYPE_ERROR, ESD_BTN_OK, "IO-Stat error. There are too many entries, bailing out");
		return;
	}

	/
	max_value=0;
	for(i=0;i<MAX_GRAPHS;i++){
		int idx;

		if(!io->graphs[i].display){
			continue;
		}
		for(idx=0; (guint32)(idx) < num_time_intervals; idx++){
			guint64 val;

			val=get_it_value(io, i, idx);

			/
			if(val>max_value){
				max_value=val;
			}
		}
	}

	/
#if GTK_CHECK_VERSION(2,22,0)
	cr = cairo_create (io->surface);
#else
	cr = gdk_cairo_create (io->pixmap);
#endif
	cairo_set_source_rgb (cr, 1, 1, 1);
	gtk_widget_get_allocation(io->draw_area, &widget_alloc);
	cairo_rectangle (cr, 0, 0, widget_alloc.width,widget_alloc.height);
	cairo_fill (cr);
	cairo_destroy (cr);
	/
	if(io->max_y_units==AUTO_MAX_YSCALE){
		max_y = yscale_max[MAX_YSCALE-1];
		for(i=MAX_YSCALE-1; i>1; i--){
			if(max_value < yscale_max[i]){
				max_y = yscale_max[i];
			}
		}
	} else if(io->max_y_units==LOGARITHMIC_YSCALE){
		max_y = 1000000000;
		for(i=1000000000; i>1; i/=10){
			if(max_value<(guint32)i){
				max_y=i;
			}
		}
	} else {
		/
		max_y=io->max_y_units;
	}

	/
	draw_y_as_time = FALSE;
	if(io->count_type==COUNT_TYPE_ADVANCED){
		draw_y_as_time = TRUE;
		for(i=0; i<MAX_GRAPHS; i++){
			int adv_type;

			if(!io->graphs[i].display){
				continue;
			}
			adv_type=proto_registrar_get_ftype(io->graphs[i].hf_index);
			switch(adv_type) {
			case FT_RELATIVE_TIME:
				switch(io->graphs[i].calc_type){
				case CALC_TYPE_SUM:
				case CALC_TYPE_MAX:
				case CALC_TYPE_MIN:
				case CALC_TYPE_AVG:
					break;
				default:
					draw_y_as_time = FALSE;
				}
				break;
			default:
				draw_y_as_time = FALSE;
			}
		}
	}

	/
	if(draw_y_as_time){
		if(io->max_y_units==LOGARITHMIC_YSCALE){
			print_time_scale_string(label_string, 15, 100000, 100000, TRUE); /
		} else {
			print_time_scale_string(label_string, 15, max_y, max_y, FALSE);
		}
	} else {
		g_snprintf(label_string, 15, "%d", max_y);
	}
	layout = gtk_widget_create_pango_layout(io->draw_area, label_string);
	pango_layout_get_pixel_size(layout, &label_width, &label_height);

	io->left_x_border = 10;
	io->right_x_border = label_width + 20;
	top_y_border = 10;
	bottom_y_border = label_height + 20;

	/
	draw_width = io->surface_width-io->right_x_border - io->left_x_border;
	draw_height = io->surface_height-top_y_border - bottom_y_border;

	/
	if (num_time_intervals >= NUM_IO_ITEMS-1) {
		g_snprintf (label_string, 45, "Warning: Graph limited to %d entries", NUM_IO_ITEMS);
		pango_layout_set_text(layout, label_string, -1);

#if GTK_CHECK_VERSION(2,22,0)
		cr = cairo_create (io->surface);
#else
		cr = gdk_cairo_create (io->pixmap);
#endif
		cairo_move_to (cr, 5, io->surface_height-bottom_y_border-draw_height-label_height/2);
		pango_cairo_show_layout (cr, layout);
		cairo_destroy (cr);
		cr = NULL;
	}

	/
#if GTK_CHECK_VERSION(2,22,0)
	cr = cairo_create (io->surface);
#else
	cr = gdk_cairo_create (io->pixmap);
#endif
	cairo_set_line_width (cr, 1.0);
	cairo_move_to(cr, io->surface_width-io->right_x_border+1.5, top_y_border + 0.5);
	cairo_line_to(cr, io->surface_width-io->right_x_border+1.5, io->surface_height-bottom_y_border + 0.5);
	cairo_stroke(cr);
	cairo_destroy(cr);
	if(io->max_y_units==LOGARITHMIC_YSCALE){
		tics = (int)log10((double)max_y);
		ystart = draw_height/10;
		ys = -1;
	} else {
		tics = 10;
		ystart=ys=0;
	}

	for(i=ys;i<=tics;i++){
		int xwidth, lwidth, ypos;

		xwidth = 5;
		if(io->max_y_units==LOGARITHMIC_YSCALE){
			if (i==ys) {
				/
				ypos=io->surface_height-bottom_y_border;
			} else if (i==tics) {
				/
				ypos=io->surface_height-bottom_y_border-draw_height;
			} else {
				int j;
				/
				for(j=2; j<10; j++) {
					ypos=(int)(io->surface_height-bottom_y_border-(draw_height-ystart)*(i+log10((double)j))/tics-ystart);
					/
#if GTK_CHECK_VERSION(2,22,0)
					cr = cairo_create (io->surface);
#else
					cr = gdk_cairo_create (io->pixmap);
#endif
					cairo_set_line_width (cr, 1.0);
					cairo_move_to(cr, io->surface_width-io->right_x_border+1.5, ypos+0.5);
					cairo_line_to(cr, io->surface_width-io->right_x_border+1.5+xwidth,ypos+0.5);
					cairo_stroke(cr);
					cairo_destroy(cr);
				}
				ypos=io->surface_height-bottom_y_border-(draw_height-ystart)*i/tics-ystart;
			}
			/
			xwidth=10;
		} else {
			if (!(i%5)) {
				/
				xwidth = 10;
			}
			ypos=io->surface_height-bottom_y_border-draw_height*i/10;
		}
		/
#if GTK_CHECK_VERSION(2,22,0)
		cr = cairo_create (io->surface);
#else
		cr = gdk_cairo_create (io->pixmap);
#endif
		cairo_set_line_width (cr, 1.0);
		cairo_move_to(cr, io->surface_width-io->right_x_border+1.5, ypos+0.5);
		cairo_line_to(cr, io->surface_width-io->right_x_border+1.5+xwidth,ypos+0.5);
		cairo_stroke(cr);
		cairo_destroy(cr);
		/
		if(xwidth==10) {
			guint32 value;
			if(io->max_y_units==LOGARITHMIC_YSCALE){
				value = (guint32)(max_y / pow(10,tics-i));
				if(draw_y_as_time){
					print_time_scale_string(label_string, 15, value, value, TRUE);
				} else {
					g_snprintf(label_string, 15, "%d", value);
				}
			} else {
				value = (max_y/10)*i;
				if(draw_y_as_time){
					print_time_scale_string(label_string, 15, value, max_y, FALSE);
				} else {
					g_snprintf(label_string, 15, "%d", value);
				}
			}

			pango_layout_set_text(layout, label_string, -1);
			pango_layout_get_pixel_size(layout, &lwidth, NULL);

#if GTK_CHECK_VERSION(2,22,0)
			cr = cairo_create (io->surface);
#else
			cr = gdk_cairo_create (io->pixmap);
#endif
			cairo_move_to (cr, io->surface_width-io->right_x_border+15+label_width-lwidth, ypos-label_height/2);
			pango_cairo_show_layout (cr, layout);
			cairo_destroy (cr);
			cr = NULL;

		}
	}

	/
	if(io->last_interval==0xffffffff) {
		last_interval = io->max_interval;
	} else {
		last_interval = io->last_interval;
	}

/
	/
#if GTK_CHECK_VERSION(2,22,0)
		cr = cairo_create (io->surface);
#else
		cr = gdk_cairo_create (io->pixmap);
#endif
		cairo_set_line_width (cr, 1.0);
		cairo_move_to(cr, io->left_x_border+0.5, io->surface_height-bottom_y_border+1.5);
		cairo_line_to(cr, io->surface_width-io->right_x_border+1.5,io->surface_height-bottom_y_border+1.5);
		cairo_stroke(cr);
		cairo_destroy(cr);
	if((last_interval/io->interval)>=draw_width/io->pixels_per_tick){
		first_interval=(last_interval/io->interval)-draw_width/io->pixels_per_tick+1;
		first_interval*=io->interval;
	} else {
		first_interval=0;
	}

	interval_delta=(100/io->pixels_per_tick)*io->interval;
	for(current_interval=last_interval;current_interval>=(gint32)first_interval;current_interval=current_interval-io->interval){
		int x, xlen;

		/
		/
		if(((io->pixels_per_tick<5) && (current_interval%(10*io->interval))) ||
		   ((io->pixels_per_tick==5) && (current_interval%(5*io->interval)))){
			continue;
		}

		if(!(current_interval%interval_delta)){
			xlen=10;
		} else if(!(current_interval%(interval_delta/2))){
			xlen=8;
		} else {
			xlen=5;
		}
		x=draw_width+io->left_x_border-((last_interval-current_interval)/io->interval)*io->pixels_per_tick;
#if GTK_CHECK_VERSION(2,22,0)
		cr = cairo_create (io->surface);
#else
		cr = gdk_cairo_create (io->pixmap);
#endif
		cairo_set_line_width (cr, 1.0);
		cairo_move_to(cr, x-1-io->pixels_per_tick/2+0.5, io->surface_height-bottom_y_border+1.5);
		cairo_line_to(cr, x-1-io->pixels_per_tick/2+0.5, io->surface_height-bottom_y_border+xlen+1.5);
		cairo_stroke(cr);
		cairo_destroy(cr);
		if(xlen==10){
			int lwidth, x_pos;
			print_interval_string (label_string, 15, current_interval, io, TRUE);
			pango_layout_set_text(layout, label_string, -1);
			pango_layout_get_pixel_size(layout, &lwidth, NULL);

			if ((x-1-io->pixels_per_tick/2-lwidth/2) < 5) {
				x_pos=5;
			} else if ((x-1-io->pixels_per_tick/2+lwidth/2) > (io->surface_width-5)) {
				x_pos=io->surface_width-lwidth-5;
			} else {
				x_pos=x-1-io->pixels_per_tick/2-lwidth/2;
			}
#if GTK_CHECK_VERSION(2,22,0)
			cr = cairo_create (io->surface);
#else
			cr = gdk_cairo_create (io->pixmap);
#endif
			cairo_move_to (cr, x_pos, io->surface_height-bottom_y_border+15);
			pango_cairo_show_layout (cr, layout);
			cairo_destroy (cr);
			cr = NULL;
		}

	}
	g_object_unref(G_OBJECT(layout));

	/
	for(i=MAX_GRAPHS-1; i>=0; i--){
		guint64 val;
		guint32 interval, x_pos, y_pos, prev_x_pos, prev_y_pos;
		/
		guint32 mavg_in_average_count = 0, mavg_left = 0, mavg_right = 0;
		guint64 mavg_cumulated = 0;
		guint64 mavg_to_remove = 0, mavg_to_add = 0;

		if(!io->graphs[i].display){
			continue;
		}

		if(io->filter_type == MOVING_AVERAGE_FILTER){
			/
			guint64 warmup_interval;

			if(first_interval/io->interval > io->filter_order/2){
				warmup_interval = first_interval/io->interval - io->filter_order/2;
				warmup_interval *= io->interval;
			} else {
				warmup_interval = 0;
			}
			mavg_to_remove = warmup_interval;
			for(;warmup_interval<first_interval;warmup_interval+=io->interval){
				mavg_cumulated += get_it_value(io, i, (int)warmup_interval/io->interval);
				mavg_in_average_count++;
				mavg_left++;
			}
			mavg_cumulated += get_it_value(io, i, (int)warmup_interval/io->interval);
			mavg_in_average_count++;
			for(warmup_interval += io->interval;
			    ((warmup_interval < (first_interval + (io->filter_order/2) * (guint64)io->interval)) &&
			    (warmup_interval <= (io->num_items * io->interval)));
			    warmup_interval += io->interval) {

				mavg_cumulated += get_it_value(io, i, (int)warmup_interval / io->interval);
				mavg_in_average_count++;
				mavg_right++;
			}
			mavg_to_add = warmup_interval;
		}

		/
		prev_x_pos = draw_width-1 -
			     io->pixels_per_tick * ((last_interval - first_interval) / io->interval) +
			     io->left_x_border;
		val = get_it_value(io, i, first_interval / io->interval);

		if(io->filter_type==MOVING_AVERAGE_FILTER
		&& mavg_in_average_count > 0) {
		    val = mavg_cumulated / mavg_in_average_count;
		}

		if(val>max_y){
			prev_y_pos=0;
		} else if (io->max_y_units==LOGARITHMIC_YSCALE){
			if (val==0) {
				prev_y_pos = (guint32)(draw_height - 1 + top_y_border);
			} else {
				prev_y_pos = (guint32) (
							(draw_height - ystart)-1 -
							((log10((double)((gint64)val)) * (draw_height - ystart)) / log10((double)max_y)) +
							top_y_border
						       );
			}
		} else {
			prev_y_pos=(guint32)(draw_height-1-(val*draw_height)/max_y+top_y_border);
		}

		for(interval = first_interval;
			interval < last_interval;
			interval += io->interval) {
			x_pos=draw_width-1-io->pixels_per_tick*((last_interval-interval)/io->interval)+io->left_x_border;

			val = get_it_value(io, i, interval/io->interval);
			/
			if (io->filter_type==MOVING_AVERAGE_FILTER) {
				if (interval != first_interval){
					mavg_left++;
					if (mavg_left > io->filter_order/2) {
						mavg_left--;
						mavg_in_average_count--;
						mavg_cumulated -= get_it_value(io, i, (int)mavg_to_remove/io->interval);
						mavg_to_remove += io->interval;
					}
					if (mavg_to_add<=(guint64)io->num_items*io->interval){
						mavg_in_average_count++;
						mavg_cumulated += get_it_value(io, i, (int)mavg_to_add/io->interval);
						mavg_to_add += io->interval;
					} else {
						mavg_right--;
					}
				}
				if (mavg_in_average_count > 0) {
					val = mavg_cumulated / mavg_in_average_count;
				}
			}

			if (val>max_y) {
				y_pos=0;
			} else if (io->max_y_units==LOGARITHMIC_YSCALE) {
				if (val==0) {
					y_pos=(guint32)(draw_height-1+top_y_border);
				} else {
					y_pos = (guint32) (
							   (draw_height - ystart) - 1 -
							   (log10((double)(gint64)val) * (draw_height - ystart)) / log10((double)max_y) +
							   top_y_border
							  );
				}
			} else {
				y_pos = (guint32)(draw_height - 1 -
						  ((val * draw_height) / max_y) +
						  top_y_border);
			}

			switch(io->graphs[i].plot_style){
			case PLOT_STYLE_LINE:
				/
				if( (prev_y_pos!=0) || (y_pos!=0) ){
#if GTK_CHECK_VERSION(2,22,0)
					cr = cairo_create (io->surface);
#else
					cr = gdk_cairo_create (io->pixmap);
#endif
					gdk_cairo_set_source_color (cr, &io->graphs[i].color);
					cairo_set_line_width (cr, 1.0);
					cairo_move_to(cr, prev_x_pos+0.5, prev_y_pos+0.5);
					cairo_line_to(cr, x_pos+0.5, y_pos+0.5);
					cairo_stroke(cr);
					cairo_destroy(cr);
				}
				break;
			case PLOT_STYLE_IMPULSE:
				if(val){
#if GTK_CHECK_VERSION(2,22,0)
					cr = cairo_create (io->surface);
#else
					cr = gdk_cairo_create (io->pixmap);
#endif
					gdk_cairo_set_source_color (cr, &io->graphs[i].color);
					cairo_set_line_width (cr, 1.0);
					cairo_move_to(cr, x_pos+0.5, draw_height-1+top_y_border+0.5);
					cairo_line_to(cr, x_pos+0.5, y_pos+0.5);
					cairo_stroke(cr);
					cairo_destroy(cr);
				}
				break;
			case PLOT_STYLE_FILLED_BAR:
				if(val){
#if GTK_CHECK_VERSION(2,22,0)
					cr = cairo_create (io->surface);
#else
					cr = gdk_cairo_create (io->pixmap);
#endif
					cairo_rectangle (cr,
						x_pos-(gdouble)io->pixels_per_tick/2+0.5,
						y_pos+0.5,
						io->pixels_per_tick,
						draw_height-1+top_y_border-y_pos);
					gdk_cairo_set_source_color (cr, &io->graphs[i].color);
					cairo_fill (cr);
					cairo_destroy (cr);
				}
				break;
			case PLOT_STYLE_DOT:
				if(val){
#if GTK_CHECK_VERSION(2,22,0)
					cr = cairo_create (io->surface);
#else
					cr = gdk_cairo_create (io->pixmap);
#endif
					cairo_arc (cr,
						x_pos+0.5,
						y_pos+0.5,
						(gdouble)io->pixels_per_tick/2,
						0,
						2 * G_PI);
					gdk_cairo_set_source_color (cr, &io->graphs[i].color);
					cairo_fill (cr);
					cairo_destroy (cr);
				}
				break;
			}

			prev_y_pos=y_pos;
			prev_x_pos=x_pos;
		}
	}

	cr = gdk_cairo_create (gtk_widget_get_window(io->draw_area));

#if GTK_CHECK_VERSION(2,22,0)
	cairo_set_source_surface (cr, io->surface, 0, 0);
#else
	gdk_cairo_set_source_pixmap (cr, io->pixmap, 0, 0);
#endif
	cairo_rectangle (cr, 0, 0, io->surface_width, io->surface_height);
	cairo_fill (cr);

	cairo_destroy (cr);

	/
	if (io->max_interval == 0) {
		gtk_adjustment_set_upper(io->scrollbar_adjustment, (gfloat) io->interval);
		gtk_adjustment_set_step_increment(io->scrollbar_adjustment, (gfloat) (io->interval/10));
		gtk_adjustment_set_page_increment(io->scrollbar_adjustment, (gfloat) io->interval);
	} else {
		gtk_adjustment_set_upper(io->scrollbar_adjustment, (gfloat) io->max_interval);
		gtk_adjustment_set_step_increment(io->scrollbar_adjustment, (gfloat) ((last_interval-first_interval)/10));
		gtk_adjustment_set_page_increment(io->scrollbar_adjustment, (gfloat) (last_interval-first_interval));
	}
	gtk_adjustment_set_page_size(io->scrollbar_adjustment, gtk_adjustment_get_page_increment(io->scrollbar_adjustment));
	gtk_adjustment_set_value(io->scrollbar_adjustment, (gfloat)first_interval);
	gtk_adjustment_changed(io->scrollbar_adjustment);
	gtk_adjustment_value_changed(io->scrollbar_adjustment);

}
