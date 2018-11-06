static int dissect_x11_requests_loop(tvbuff_t *tvb, int *offsetp, proto_tree *root)
{
      int left = tvb_reported_length(tvb), nextLeft;
      proto_item *ti;
      proto_tree *t;
      guint8 v8, v8_2;
      guint16 v16;
      guint32 v32;

      /
      for(;;) {
	    int length, opcode;
	    
	    /
	    if (left < 4) {
		/
		break;
	    }
	    length = VALUE16(tvb, *offsetp + 2) * 4;
	    /
	    if (left < length) {
		/
		break;
	    }
	    if (length < 4) {
	    	/
		break;
	    }

	    next_offset = *offsetp + length;
	    nextLeft = left - length;

	    ti = proto_tree_add_uint(root, hf_x11_request, tvb, *offsetp, length, tvb_get_guint8(tvb, *offsetp));
	    t = proto_item_add_subtree(ti, ett_x11_request);

	    OPCODE();

	    switch(opcode) {
		case 1: /
		  CARD8(depth);
		  REQUEST_LENGTH();
		  WINDOW(wid);
		  WINDOW(parent);
		  INT16(x);
		  INT16(y);
		  CARD16(width);
		  CARD16(height);
		  CARD16(border_width);
		  ENUM16(window_class);
		  VISUALID(visual);
		  windowAttributes(tvb, offsetp, t);
		  break;

		case 2: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  windowAttributes(tvb, offsetp, t);
		  break;

		case 3: /
		case 4: /
		case 5: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 6: /
		  ENUM8(save_set_mode);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 7: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  WINDOW(parent);
		  INT16(x);
		  INT16(y);
		  break;

		case 8: /
		case 9: /
		case 10: /
		case 11: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 12: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  BITMASK16(configure_window);
		  UNUSED(2); 
		  BITFIELD(INT16,  configure_window_mask, x);
		  BITFIELD(INT16,  configure_window_mask, y);
		  BITFIELD(CARD16, configure_window_mask, width);
		  BITFIELD(CARD16, configure_window_mask, height);
		  BITFIELD(CARD16, configure_window_mask, border_width);
		  BITFIELD(WINDOW, configure_window_mask, sibling);
		  BITFIELD(ENUM8,  configure_window_mask, stack_mode);
		  ENDBITMASK;
		  PAD();
		  break;

		case 13: /
		  ENUM8(direction);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 14: /
		case 15: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  break;

		case 16: /
		  BOOL(only_if_exists);
		  REQUEST_LENGTH();
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 17: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  ATOM(atom);
		  break;

		case 18: /
		  ENUM8(mode);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  ATOM(property);
		  ATOM(type);
		  CARD8(format);
		  UNUSED(3);
		  v32 = CARD32(data_length);
		  LISTofBYTE(data, v32);
		  PAD();
		  break;

		case 19: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  ATOM(property);
		  break;

		case 20: /
		  BOOL(delete);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  ATOM(property);
		  ATOM(get_property_type);
		  CARD32(long_offset);
		  CARD32(long_length);
		  break;

		case 21: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 22: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(owner);
		  ATOM(selection);
		  TIMESTAMP(time);
		  break;

		case 23: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  ATOM(selection);
		  break;

		case 24: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(requestor);
		  ATOM(selection);
		  ATOM(target);
		  ATOM(property);
		  TIMESTAMP(time);
		  break;

		case 26: /
		  BOOL(owner_events);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  SETofPOINTEREVENT(pointer_event_mask);
		  ENUM8(pointer_mode);
		  ENUM8(keyboard_mode);
		  WINDOW(confine_to);
		  CURSOR(cursor);
		  TIMESTAMP(time);
		  break;

		case 27: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  TIMESTAMP(time);
		  break;

		case 28: /
		  BOOL(owner_events);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  SETofPOINTEREVENT(event_mask);
		  ENUM8(pointer_mode);
		  ENUM8(keyboard_mode);
		  WINDOW(confine_to);
		  CURSOR(cursor);
		  BUTTON(button);
		  UNUSED(1);
		  SETofKEYMASK(modifiers);
		  break;

		case 29: /
		  BUTTON(button);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  SETofKEYMASK(modifiers);
		  UNUSED(2);
		  break;

		case 30: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CURSOR(cursor);
		  TIMESTAMP(time);
		  SETofPOINTEREVENT(event_mask);
		  UNUSED(2);
		  break;

		case 31: /
		  BOOL(owner_events);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  TIMESTAMP(time);
		  ENUM8(pointer_mode);
		  ENUM8(keyboard_mode);
		  UNUSED(2);
		  break;

		case 32: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  TIMESTAMP(time);
		  break;

		case 33: /
		  BOOL(owner_events);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  SETofKEYMASK(modifiers);
		  KEYCODE(key);
		  ENUM8(pointer_mode);
		  ENUM8(keyboard_mode);
		  UNUSED(3);
		  break;

		case 34: /
		  KEYCODE(key);
		  REQUEST_LENGTH();
		  WINDOW(grab_window);
		  SETofKEYMASK(modifiers);
		  UNUSED(2);
		  break;

		case 35: /
		  ENUM8(allow_events_mode);
		  REQUEST_LENGTH();
		  TIMESTAMP(time);
		  break;

		case 36: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 37: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 38: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 39: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  TIMESTAMP(start);
		  TIMESTAMP(stop);
		  break;

		case 40: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(src_window);
		  WINDOW(dst_window);
		  INT16(src_x);
		  INT16(src_y);
		  break;

		case 41: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(warp_pointer_src_window);
		  WINDOW(warp_pointer_dst_window);
		  INT16(src_x);
		  INT16(src_y);
		  CARD16(src_width);
		  CARD16(src_height);
		  INT16(dst_x);
		  INT16(dst_y);
		  break;

		case 42: /
		  ENUM8(revert_to);
		  REQUEST_LENGTH();
		  WINDOW(focus);
		  TIMESTAMP(time);
		  break;

		case 43: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 44: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 45: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  FONT(fid);
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 46: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  FONT(font);
		  break;

		case 47: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  FONTABLE(font);
		  break;

		case 48: /
		  v8 = BOOL(odd_length);
		  REQUEST_LENGTH();
		  FONTABLE(font);
		  STRING16(string16, (next_offset - *offsetp - (v8 ? 2 : 0)) / 2);
		  PAD();
		  break;

		case 49: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CARD16(max_names);
		  v16 = FIELD16(pattern_length);
		  STRING8(pattern, v16);
		  PAD();
		  break;

		case 50: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CARD16(max_names);
		  v16 = FIELD16(pattern_length);
		  STRING8(pattern, v16);
		  PAD();
		  break;

		case 51: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  v16 = CARD16(str_number_in_path);
		  UNUSED(2);
		  LISTofSTRING8(path, v16);
		  PAD();
		  break;

		case 52: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 53: /
		  CARD8(depth);
		  REQUEST_LENGTH();
		  PIXMAP(pid);
		  DRAWABLE(drawable);
		  CARD16(width);
		  CARD16(height);
		  break;

		case 54: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  PIXMAP(pixmap);
		  break;

		case 55: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  GCONTEXT(cid);
		  DRAWABLE(drawable);
		  gcAttributes(tvb, offsetp, t);
		  break;

		case 56: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  GCONTEXT(gc);
		  gcAttributes(tvb, offsetp, t);
		  break;

		case 57: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  GCONTEXT(src_gc);
		  GCONTEXT(dst_gc);
		  gcMask(tvb, offsetp, t);
		  break;

		case 58: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  GCONTEXT(gc);
		  CARD16(dash_offset);
		  v16 = FIELD16(dashes_length);
		  LISTofCARD8(dashes, v16);
		  PAD();
		  break;

		case 59: /
		  ENUM8(ordering);
		  REQUEST_LENGTH();
		  GCONTEXT(gc);
		  INT16(clip_x_origin);
		  INT16(clip_y_origin);
		  LISTofRECTANGLE(rectangles);
		  break;

		case 60: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  GCONTEXT(gc);
		  break;

		case 61: /
		  BOOL(exposures);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  INT16(x);
		  INT16(y);
		  CARD16(width);
		  CARD16(height);
		  break;

		case 62: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(src_drawable);
		  DRAWABLE(dst_drawable);
		  GCONTEXT(gc);
		  INT16(src_x);
		  INT16(src_y);
		  INT16(dst_x);
		  INT16(dst_y);
		  CARD16(width);
		  CARD16(height);
		  break;

		case 63: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(src_drawable);
		  DRAWABLE(dst_drawable);
		  GCONTEXT(gc);
		  INT16(src_x);
		  INT16(src_y);
		  INT16(dst_x);
		  INT16(dst_y);
		  CARD16(width);
		  CARD16(height);
		  CARD32(bit_plane);
		  break;

		case 64: /
		  ENUM8(coordinate_mode);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofPOINT(points, v16 - 12);
		  break;

		case 65: /
		  ENUM8(coordinate_mode);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofPOINT(points, v16 - 12);
		  break;

		case 66: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofSEGMENT(segments);
		  break;

		case 67: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofRECTANGLE(rectangles);
		  break;

		case 68: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofARC(arcs);
		  break;

		case 69: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  ENUM8(shape);
		  ENUM8(coordinate_mode);
		  UNUSED(2);
		  LISTofPOINT(points, v16 - 16);
		  break;

		case 70: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofRECTANGLE(rectangles);
		  break;

		case 71: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  LISTofARC(arcs);
		  break;

		case 72: /
		  ENUM8(image_format);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  CARD16(width);
		  CARD16(height);
		  INT16(dst_x);
		  INT16(dst_y);
		  CARD8(left_pad);
		  CARD8(depth);
		  UNUSED(2);
		  LISTofBYTE(data, v16 - 24);
		  PAD();
		  break;

		case 73: /
		  ENUM8(image_pixmap_format);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  INT16(x);
		  INT16(y);
		  CARD16(width);
		  CARD16(height);
		  CARD32(plane_mask);
		  break;

		case 74: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  INT16(x);
		  INT16(y);
		  LISTofTEXTITEM8(items);
		  PAD();
		  break;

		case 75: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  INT16(x);
		  INT16(y);
		  LISTofTEXTITEM16(items);
		  PAD();
		  break;

		case 76: /
		  v8 = FIELD8(string_length);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  INT16(x);
		  INT16(y);
		  STRING8(string, v8);
		  PAD();
		  break;

		case 77: /
		  v8 = FIELD8(string_length);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  GCONTEXT(gc);
		  INT16(x);
		  INT16(y);
		  STRING16(string16, v8);
		  PAD();
		  break;

		case 78: /
		  ENUM8(alloc);
		  REQUEST_LENGTH();
		  COLORMAP(mid);
		  WINDOW(window);
		  VISUALID(visual);
		  break;

		case 79: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  break;

		case 80: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(mid);
		  COLORMAP(src_cmap);
		  break;

		case 81: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  break;

		case 82: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  break;

		case 83: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  WINDOW(window);
		  break;

		case 84: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  CARD16(red);
		  CARD16(green);
		  CARD16(blue);
		  UNUSED(2);
		  break;

		case 85: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 86: /
		  BOOL(contiguous);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  CARD16(colors);
		  CARD16(planes);
		  break;

		case 87: /
		  BOOL(contiguous);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  CARD16(colors);
		  CARD16(reds);
		  CARD16(greens);
		  CARD16(blues);
		  break;

		case 88: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  COLORMAP(cmap);
		  CARD32(plane_mask);
		  LISTofCARD32(pixels, v16 - 12);
		  break;

		case 89: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  COLORMAP(cmap);
		  LISTofCOLORITEM(color_items, v16 - 8);
		  break;

		case 90: /
		  COLOR_FLAGS(color);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  CARD32(pixel);	
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 91: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  COLORMAP(cmap);
		  LISTofCARD32(pixels, v16 - 8);
		  break;

		case 92: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  COLORMAP(cmap);
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 93: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CURSOR(cid);
		  PIXMAP(source_pixmap);
		  PIXMAP(mask);
		  CARD16(fore_red);
		  CARD16(fore_green);
		  CARD16(fore_blue);
		  CARD16(back_red);
		  CARD16(back_green);
		  CARD16(back_blue);
		  CARD16(x);
		  CARD16(y);
		  break;

		case 94: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CURSOR(cid);
		  FONT(source_font);
		  FONT(mask_font);
		  CARD16(source_char);
		  CARD16(mask_char);
		  CARD16(fore_red);
		  CARD16(fore_green);
		  CARD16(fore_blue);
		  CARD16(back_red);
		  CARD16(back_green);
		  CARD16(back_blue);
		  break;

		case 95: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CURSOR(cursor);
		  break;

		case 96: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CURSOR(cursor);
		  CARD16(fore_red);
		  CARD16(fore_green);
		  CARD16(fore_blue);
		  CARD16(back_red);
		  CARD16(back_green);
		  CARD16(back_blue);
		  break;

		case 97: /
		  ENUM8(class);
		  REQUEST_LENGTH();
		  DRAWABLE(drawable);
		  CARD16(width);
		  CARD16(height);
		  break;

		case 98: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  v16 = FIELD16(name_length);
		  UNUSED(2);
		  STRING8(name, v16);
		  PAD();
		  break;

		case 99: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 100: /
		  v8 = FIELD8(keycode_count);
		  REQUEST_LENGTH();
		  KEYCODE(first_keycode);
		  v8_2 = FIELD8(keysyms_per_keycode);
		  UNUSED(2);
		  LISTofKEYSYM(keysyms, v8, v8_2);
		  break;

		case 101: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  KEYCODE(first_keycode);
		  FIELD8(count);
		  UNUSED(2);
		  break;

		case 102: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  BITMASK32(keyboard_value);
		  BITFIELD(INT8, keyboard_value_mask, key_click_percent);
		  BITFIELD(INT8, keyboard_value_mask, bell_percent);
		  BITFIELD(INT16, keyboard_value_mask, bell_pitch);
		  BITFIELD(INT16, keyboard_value_mask, bell_duration);
		  BITFIELD(INT16, keyboard_value_mask, led);
		  BITFIELD(ENUM8, keyboard_value_mask, led_mode);
		  BITFIELD(KEYCODE, keyboard_value_mask, keyboard_key);
		  BITFIELD(ENUM8, keyboard_value_mask, auto_repeat_mode);
		  ENDBITMASK;
		  break;

		case 103: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 104: /
		  INT8(percent);
		  REQUEST_LENGTH();
		  break;

		case 105: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  INT16(acceleration_numerator);
		  INT16(acceleration_denominator);
		  INT16(threshold);
		  BOOL(do_acceleration);
		  BOOL(do_threshold);
		  break;

		case 106: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 107: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  INT16(timeout);
		  INT16(interval);
		  ENUM8(prefer_blanking);
		  ENUM8(allow_exposures);
		  UNUSED(2);
		  break;

		case 108: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 109: /
		  ENUM8(change_host_mode);
		  REQUEST_LENGTH();
		  ENUM8(family);
		  UNUSED(1);
		  v16 = CARD16(address_length);
		  LISTofCARD8(address, v16);
		  break;

		case 110: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 111: /
		  ENUM8(access_mode);
		  REQUEST_LENGTH();
		  break;

		case 112: /
		  ENUM8(close_down_mode);
		  REQUEST_LENGTH();
		  break;

		case 113: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  CARD32(resource);
		  break;

		case 114: /
		  UNUSED(1);
		  v16 = REQUEST_LENGTH();
		  WINDOW(window);
		  CARD16(property_number);
		  INT16(delta);
		  LISTofATOM(properties, (v16 - 12));
		  break;

		case 115: /
		  ENUM8(screen_saver_mode);
		  REQUEST_LENGTH();
		  break;

		case 116: /
		  v8 = FIELD8(map_length);
		  REQUEST_LENGTH();
		  LISTofCARD8(map, v8);
		  PAD();
		  break;

		case 117: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;

		case 118: /
		  v8 = FIELD8(keycodes_per_modifier);
		  REQUEST_LENGTH();
		  LISTofKEYCODE(keycodes, v8);
		  break;

		case 119: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;
		  
		case 127: /
		  UNUSED(1);
		  REQUEST_LENGTH();
		  break;
	    }
	    if (*offsetp < next_offset)
		  proto_tree_add_item(t, hf_x11_undecoded, tvb, *offsetp, next_offset - *offsetp, little_endian);
	    *offsetp = next_offset;
	    left = nextLeft;
      }

      return left;
}
