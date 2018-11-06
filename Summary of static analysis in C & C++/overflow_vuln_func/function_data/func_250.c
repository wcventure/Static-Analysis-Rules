static void dissect_x11_request(tvbuff_t *tvb, packet_info *pinfo,
    proto_tree *tree, const char *sep, x11_conv_data_t *state,
    guint byte_order)
{
      int offset = 0;
      int *offsetp = &offset;
      int query_ext_offset;
      int next_offset;
      proto_item *ti;
      proto_tree *t;
      int length, opcode, i;
      guint8 v8, v8_2, v8_3;
      guint16 v16;
      guint32 v32;
      gint left;
      gchar *name;

      query_ext_offset = 2; /

      length = tvb_get_guint16(tvb, query_ext_offset, byte_order) * 4;
      query_ext_offset += 2;

      if (length == 0) {
            /
            length = tvb_get_guint32(tvb, query_ext_offset, byte_order) * 4;
            query_ext_offset += 4;
      }

      if (length < 4) {
            /
            return;
      }

      next_offset = offset + length;

      ti = proto_tree_add_item(tree, proto_x11, tvb, 0, -1, ENC_NA);
      t = proto_item_add_subtree(ti, ett_x11);

      if (!pinfo->fd->flags.visited)
            ++state->sequencenumber;

      OPCODE();

      col_append_fstr(pinfo->cinfo, COL_INFO, "%s %s", sep,
                    val_to_str(opcode, state->opcode_vals,
                                "<Unknown opcode %d>"));

      proto_item_append_text(ti, ", Request, opcode: %d (%s)",
                             opcode, val_to_str(opcode, state->opcode_vals,
                                                "<Unknown opcode %d>"));

      /
      switch(opcode) {

            case X_QueryExtension:

                  /

                  v16 = tvb_get_guint16(tvb, query_ext_offset, byte_order);
                  query_ext_offset += 2;
                  /
                  query_ext_offset += 2;
                  name = tvb_get_string_enc(wmem_file_scope(), tvb, query_ext_offset, v16, ENC_ASCII);

                  /
                  i = 0;
                  while(i < MAX_OPCODES) {
                        if (state->opcode_vals[i].strptr == NULL) {
                              state->opcode_vals[i].strptr = name;
                              state->opcode_vals[i].value = -1;
                              wmem_map_insert(state->valtable,
                                                  GINT_TO_POINTER(state->sequencenumber),
                                                  (int *)&state->opcode_vals[i]);
                              break;
                        } else if (strcmp(state->opcode_vals[i].strptr,
                                          name) == 0) {
                              wmem_map_insert(state->valtable,
                                                  GINT_TO_POINTER(state->sequencenumber),
                                                  (int *)&state->opcode_vals[i]);
                              break;
                        }
                        i++;
                  }

                  /
      /
      case X_AllocColor:
      case X_AllocColorCells:
      case X_AllocColorPlanes:
      case X_AllocNamedColor:
      case X_GetAtomName:
      case X_GetFontPath:
      case X_GetGeometry:
      case X_GetImage:
      case X_GetInputFocus:
      case X_GetKeyboardControl:
      case X_GetKeyboardMapping:
      case X_GetModifierMapping:
      case X_GetMotionEvents:
      case X_GetPointerControl:
      case X_GetPointerMapping:
      case X_GetProperty:
      case X_GetScreenSaver:
      case X_GetSelectionOwner:
      case X_GetWindowAttributes:
      case X_GrabKeyboard:
      case X_GrabPointer:
      case X_InternAtom:
      case X_ListExtensions:
      case X_ListFonts:
      case X_ListFontsWithInfo:
      case X_ListHosts:
      case X_ListInstalledColormaps:
      case X_ListProperties:
      case X_LookupColor:
      case X_QueryBestSize:
      case X_QueryColors:
      case X_QueryFont:
      case X_QueryKeymap:
      case X_QueryPointer:
      case X_QueryTextExtents:
      case X_QueryTree:
      case X_SetModifierMapping:
      case X_SetPointerMapping:
      case X_TranslateCoords:
            /
            wmem_map_insert(state->seqtable,
                                GINT_TO_POINTER(state->sequencenumber),
                                GINT_TO_POINTER(opcode));

            break;

      default:
            /
            if (opcode >= X_FirstExtension && opcode <= X_LastExtension) {
                  guint32 minor;
                  minor = tvb_get_guint8(tvb, 1);

                  wmem_map_insert(state->seqtable,
                                      GINT_TO_POINTER(state->sequencenumber),
                                      GINT_TO_POINTER(opcode | (minor << 8)));
            }

            /
            break;
      }

      if (tree == NULL)
            return;

      switch(opcode) {

      case X_CreateWindow:
            CARD8(depth);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(wid);
            WINDOW(parent);
            INT16(x);
            INT16(y);
            CARD16(width);
            CARD16(height);
            CARD16(border_width);
            ENUM16(window_class);
            VISUALID(visual);
            windowAttributes(tvb, offsetp, t, byte_order);
            break;

      case X_ChangeWindowAttributes:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            windowAttributes(tvb, offsetp, t, byte_order);
            break;

      case X_GetWindowAttributes:
      case X_DestroyWindow:
      case X_DestroySubwindows:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_ChangeSaveSet:
            ENUM8(save_set_mode);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_ReparentWindow:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            WINDOW(parent);
            INT16(x);
            INT16(y);
            break;

      case X_MapWindow:
      case X_MapSubwindows:
      case X_UnmapWindow:
      case X_UnmapSubwindows:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_ConfigureWindow:
            {
            guint16 bitmask16;
            static const int * window_attributes_flags[] = {
                  &hf_x11_window_value_mask_background_pixmap,
                  &hf_x11_window_value_mask_background_pixel,
                  &hf_x11_window_value_mask_border_pixmap,
                  &hf_x11_window_value_mask_border_pixel,
                  &hf_x11_window_value_mask_bit_gravity,
                  &hf_x11_window_value_mask_win_gravity,
                  &hf_x11_window_value_mask_backing_store,
                  &hf_x11_window_value_mask_backing_planes,
                  &hf_x11_window_value_mask_backing_pixel,
                  &hf_x11_window_value_mask_override_redirect,
                  &hf_x11_window_value_mask_save_under,
                  &hf_x11_window_value_mask_event_mask,
                  &hf_x11_window_value_mask_do_not_propagate_mask,
                  &hf_x11_window_value_mask_colormap,
                  &hf_x11_window_value_mask_cursor,
                  NULL
            };

            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            proto_tree_add_item(t, hf_x11_window, tvb, *offsetp, 1, byte_order);
            *offsetp += 4;
            bitmask16 = tvb_get_guint16(tvb, *offsetp, byte_order);
            proto_tree_add_bitmask(t, tvb, *offsetp, hf_x11_configure_window_mask, ett_x11_configure_window_mask, window_attributes_flags, byte_order);
            *offsetp += 2;
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            if (bitmask16 & 0x0001) {
                proto_tree_add_item(t, hf_x11_x, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask16 & 0x0002) {
                proto_tree_add_item(t, hf_x11_y, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask16 & 0x0004) {
                proto_tree_add_item(t, hf_x11_width, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask16 & 0x0008) {
                proto_tree_add_item(t, hf_x11_height, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask16 & 0x0010) {
                proto_tree_add_item(t, hf_x11_border_width, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask16 & 0x0020) {
                proto_tree_add_item(t, hf_x11_sibling, tvb, *offsetp, 4, byte_order);
                *offsetp += 4;
            }
            if (bitmask16 & 0x0040) {
                proto_tree_add_item(t, hf_x11_stack_mode, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            if (next_offset - *offsetp > 0) {
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, next_offset - *offsetp, ENC_NA);
                *offsetp = next_offset;
            }
            }
            break;

      case X_CirculateWindow:
            ENUM8(direction);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_GetGeometry:
      case X_QueryTree:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            break;

      case X_InternAtom:
            BOOL(only_if_exists);
            requestLength(tvb, offsetp, t, byte_order);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_GetAtomName:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            ATOM(atom);
            break;

      case X_ChangeProperty:
            ENUM8(mode);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            ATOM(property);
            ATOM(type);
            v8 = CARD8(format);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
            *offsetp += 3;
            v32 = CARD32(data_length);
            switch (v8) {
            case 8:
                if (v32)
                    LISTofBYTE(data, v32);
                break;
            case 16:
                if (v32)
                    LISTofCARD16(data16, v32 * 2);
                break;
            case 32:
                if (v32)
                    LISTofCARD32(data32, v32 * 4);
                break;
            default:
                expert_add_info(pinfo, ti, &ei_x11_invalid_format);
                break;
            }
            PAD();
            break;

      case X_DeleteProperty:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            ATOM(property);
            break;

      case X_GetProperty:
            BOOL(delete);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            ATOM(property);
            ATOM(get_property_type);
            CARD32(long_offset);
            CARD32(long_length);
            break;

      case X_ListProperties:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_SetSelectionOwner:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(owner);
            ATOM(selection);
            TIMESTAMP(time);
            break;

      case X_GetSelectionOwner:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            ATOM(selection);
            break;

      case X_ConvertSelection:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(requestor);
            ATOM(selection);
            ATOM(target);
            ATOM(property);
            TIMESTAMP(time);
            break;

      case X_SendEvent:
            BOOL(propagate);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(destination);
            SETofEVENT(event_mask);
            EVENT();
            break;

      case X_GrabPointer:
            BOOL(owner_events);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            SETofPOINTEREVENT(pointer_event_mask);
            ENUM8(pointer_mode);
            ENUM8(keyboard_mode);
            WINDOW(confine_to);
            CURSOR(cursor);
            TIMESTAMP(time);
            break;

      case X_UngrabPointer:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            TIMESTAMP(time);
            break;

      case X_GrabButton:
            BOOL(owner_events);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            SETofPOINTEREVENT(event_mask);
            ENUM8(pointer_mode);
            ENUM8(keyboard_mode);
            WINDOW(confine_to);
            CURSOR(cursor);
            BUTTON(button);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            SETofKEYMASK(modifiers);
            break;

      case X_UngrabButton:
            BUTTON(button);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            SETofKEYMASK(modifiers);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_ChangeActivePointerGrab:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CURSOR(cursor);
            TIMESTAMP(time);
            SETofPOINTEREVENT(event_mask);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_GrabKeyboard:
            BOOL(owner_events);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            TIMESTAMP(time);
            ENUM8(pointer_mode);
            ENUM8(keyboard_mode);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_UngrabKeyboard:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            TIMESTAMP(time);
            break;

      case X_GrabKey:
            BOOL(owner_events);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            SETofKEYMASK(modifiers);
            KEYCODE(key);
            ENUM8(pointer_mode);
            ENUM8(keyboard_mode);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
            *offsetp += 3;
            break;

      case X_UngrabKey:
            KEYCODE(key);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(grab_window);
            SETofKEYMASK(modifiers);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_AllowEvents:
            ENUM8(allow_events_mode);
            requestLength(tvb, offsetp, t, byte_order);
            TIMESTAMP(time);
            break;

      case X_GrabServer:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_UngrabServer:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_QueryPointer:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_GetMotionEvents:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            TIMESTAMP(start);
            TIMESTAMP(stop);
            break;

      case X_TranslateCoords:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(src_window);
            WINDOW(dst_window);
            INT16(src_x);
            INT16(src_y);
            break;

      case X_WarpPointer:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(warp_pointer_src_window);
            WINDOW(warp_pointer_dst_window);
            INT16(src_x);
            INT16(src_y);
            CARD16(src_width);
            CARD16(src_height);
            INT16(dst_x);
            INT16(dst_y);
            break;

      case X_SetInputFocus:
            ENUM8(revert_to);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(focus);
            TIMESTAMP(time);
            break;

      case X_GetInputFocus:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_QueryKeymap:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_OpenFont:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            FONT(fid);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_CloseFont:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            FONT(font);
            break;

      case X_QueryFont:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            FONTABLE(font);
            break;

      case X_QueryTextExtents:
            v8 = BOOL(odd_length);
            requestLength(tvb, offsetp, t, byte_order);
            FONTABLE(font);
            STRING16(string16, (next_offset - offset - (v8 ? 2 : 0)) / 2);
            PAD();
            break;

      case X_ListFonts:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CARD16(max_names);
            v16 = FIELD16(pattern_length);
            STRING8(pattern, v16);
            PAD();
            break;

      case X_ListFontsWithInfo:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CARD16(max_names);
            v16 = FIELD16(pattern_length);
            STRING8(pattern, v16);
            PAD();
            break;

      case X_SetFontPath:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            v16 = CARD16(str_number_in_path);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            LISTofSTRING8(path, v16);
            PAD();
            break;

      case X_GetFontPath:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_CreatePixmap:
            CARD8(depth);
            requestLength(tvb, offsetp, t, byte_order);
            PIXMAP(pid);
            DRAWABLE(drawable);
            CARD16(width);
            CARD16(height);
            break;

      case X_FreePixmap:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            PIXMAP(pixmap);
            break;

      case X_CreateGC:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(cid);
            DRAWABLE(drawable);
            gcAttributes(tvb, offsetp, t, byte_order);
            break;

      case X_ChangeGC:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(gc);
            gcAttributes(tvb, offsetp, t, byte_order);
            break;

      case X_CopyGC:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(src_gc);
            GCONTEXT(dst_gc);
            gcMask(tvb, offsetp, t, byte_order);
            break;

      case X_SetDashes:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(gc);
            CARD16(dash_offset);
            v16 = FIELD16(dashes_length);
            LISTofCARD8(dashes, v16);
            PAD();
            break;

      case X_SetClipRectangles:
            ENUM8(ordering);
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(gc);
            INT16(clip_x_origin);
            INT16(clip_y_origin);
            LISTofRECTANGLE(rectangles);
            break;

      case X_FreeGC:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            GCONTEXT(gc);
            break;

      case X_ClearArea:
            BOOL(exposures);
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            INT16(x);
            INT16(y);
            CARD16(width);
            CARD16(height);
            break;

      case X_CopyArea:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
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

      case X_CopyPlane:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
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

      case X_PolyPoint:
            ENUM8(coordinate_mode);
            v16 = requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofPOINT(points, v16 - 12);
            break;

      case X_PolyLine:
            ENUM8(coordinate_mode);
            v16 = requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofPOINT(points, v16 - 12);
            break;

      case X_PolySegment:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofSEGMENT(segments);
            break;

      case X_PolyRectangle:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofRECTANGLE(rectangles);
            break;

      case X_PolyArc:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofARC(arcs);
            break;

      case X_FillPoly:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            ENUM8(shape);
            ENUM8(coordinate_mode);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            LISTofPOINT(points, v16 - 16);
            break;

      case X_PolyFillRectangle:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofRECTANGLE(rectangles);
            break;

      case X_PolyFillArc:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            LISTofARC(arcs);
            break;

      case X_PutImage:
            ENUM8(image_format);
            v16 = requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            CARD16(width);
            CARD16(height);
            INT16(dst_x);
            INT16(dst_y);
            CARD8(left_pad);
            CARD8(depth);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            LISTofBYTE(data, v16 - 24);
            PAD();
            break;

      case X_GetImage:
            ENUM8(image_pixmap_format);
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            INT16(x);
            INT16(y);
            CARD16(width);
            CARD16(height);
            CARD32(plane_mask);
            break;

      case X_PolyText8:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            INT16(x);
            INT16(y);
            LISTofTEXTITEM8(items);
            PAD();
            break;

      case X_PolyText16:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            INT16(x);
            INT16(y);
            LISTofTEXTITEM16(items);
            PAD();
            break;

      case X_ImageText8:
            v8 = FIELD8(string_length);
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            INT16(x);
            INT16(y);
            STRING8(string, v8);
            PAD();
            break;

      case X_ImageText16:
            v8 = FIELD8(string_length);
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            GCONTEXT(gc);
            INT16(x);
            INT16(y);
            STRING16(string16, v8);
            PAD();
            break;

      case X_CreateColormap:
            ENUM8(alloc);
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(mid);
            WINDOW(window);
            VISUALID(visual);
            break;

      case X_FreeColormap:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            break;

      case X_CopyColormapAndFree:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(mid);
            COLORMAP(src_cmap);
            break;

      case X_InstallColormap:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            break;

      case X_UninstallColormap:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            break;

      case X_ListInstalledColormaps:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            break;

      case X_AllocColor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            CARD16(red);
            CARD16(green);
            CARD16(blue);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_AllocNamedColor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_AllocColorCells:
            BOOL(contiguous);
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            CARD16(colors);
            CARD16(planes);
            break;

      case X_AllocColorPlanes:
            BOOL(contiguous);
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            CARD16(colors);
            CARD16(reds);
            CARD16(greens);
            CARD16(blues);
            break;

      case X_FreeColors:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            CARD32(plane_mask);
            LISTofCARD32(pixels, v16 - 12);
            break;

      case X_StoreColors:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            LISTofCOLORITEM(color_items, v16 - 8);
            break;

      case X_StoreNamedColor:
            COLOR_FLAGS(color);
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            CARD32(pixel);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_QueryColors:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            LISTofCARD32(pixels, v16 - 8);
            break;

      case X_LookupColor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            COLORMAP(cmap);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_CreateCursor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
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

      case X_CreateGlyphCursor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
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

      case X_FreeCursor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CURSOR(cursor);
            break;

      case X_RecolorCursor:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CURSOR(cursor);
            CARD16(fore_red);
            CARD16(fore_green);
            CARD16(fore_blue);
            CARD16(back_red);
            CARD16(back_green);
            CARD16(back_blue);
            break;

      case X_QueryBestSize:
            ENUM8(class);
            requestLength(tvb, offsetp, t, byte_order);
            DRAWABLE(drawable);
            CARD16(width);
            CARD16(height);
            break;

      case X_QueryExtension:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            v16 = FIELD16(name_length);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            STRING8(name, v16);
            PAD();
            break;

      case X_ListExtensions:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_ChangeKeyboardMapping:
            v8 = FIELD8(keycode_count);
            requestLength(tvb, offsetp, t, byte_order);
            v8_2 = KEYCODE(first_keycode);
            v8_3 = FIELD8(keysyms_per_keycode);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            LISTofKEYSYM(keysyms, state->keycodemap, v8_2, v8, v8_3);
            break;

      case X_GetKeyboardMapping:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            state->request.GetKeyboardMapping.first_keycode
            = KEYCODE(first_keycode);
            FIELD8(count);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_ChangeKeyboardControl:
            {
            guint32 bitmask32;
            static const int * keyboard_value_flags[] = {
                  &hf_x11_keyboard_value_mask_key_click_percent,
                  &hf_x11_keyboard_value_mask_bell_percent,
                  &hf_x11_keyboard_value_mask_bell_pitch,
                  &hf_x11_keyboard_value_mask_bell_duration,
                  &hf_x11_keyboard_value_mask_led,
                  &hf_x11_keyboard_value_mask_led_mode,
                  &hf_x11_keyboard_value_mask_keyboard_key,
                  &hf_x11_keyboard_value_mask_auto_repeat_mode,
                  NULL
            };

            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            bitmask32 = tvb_get_guint32(tvb, *offsetp, byte_order);
            proto_tree_add_bitmask(t, tvb, *offsetp, hf_x11_keyboard_value_mask, ett_x11_keyboard_value_mask, keyboard_value_flags, byte_order);
            *offsetp += 4;
            if (bitmask32 & 0x00000001) {
                proto_tree_add_item(t, hf_x11_key_click_percent, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            if (bitmask32 & 0x00000002) {
                proto_tree_add_item(t, hf_x11_bell_percent, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            if (bitmask32 & 0x00000004) {
                proto_tree_add_item(t, hf_x11_bell_pitch, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask32 & 0x00000008) {
                proto_tree_add_item(t, hf_x11_bell_duration, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask32 & 0x00000010) {
                proto_tree_add_item(t, hf_x11_led, tvb, *offsetp, 2, byte_order);
                *offsetp += 2;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
                *offsetp += 2;
            }
            if (bitmask32 & 0x00000020) {
                proto_tree_add_item(t, hf_x11_led_mode, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            if (bitmask32 & 0x00000040) {
                proto_tree_add_item(t, hf_x11_keyboard_key, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            if (bitmask32 & 0x00000080) {
                proto_tree_add_item(t, hf_x11_auto_repeat_mode, tvb, *offsetp, 1, byte_order);
                *offsetp += 1;
                proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 3, ENC_NA);
                *offsetp += 3;
            }
            }
            break;

      case X_GetKeyboardControl:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_Bell:
            INT8(percent);
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_ChangePointerControl:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            INT16(acceleration_numerator);
            INT16(acceleration_denominator);
            INT16(threshold);
            BOOL(do_acceleration);
            BOOL(do_threshold);
            break;

      case X_GetPointerControl:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_SetScreenSaver:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            INT16(timeout);
            INT16(interval);
            ENUM8(prefer_blanking);
            ENUM8(allow_exposures);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 2, ENC_NA);
            *offsetp += 2;
            break;

      case X_GetScreenSaver:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_ChangeHosts:
            ENUM8(change_host_mode);
            requestLength(tvb, offsetp, t, byte_order);
            v8 = ENUM8(family);
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = CARD16(address_length);
            if (v8 == FAMILY_INTERNET && v16 == 4) {
                  /
                  LISTofIPADDRESS(ip_address, v16);
            } else
                  LISTofCARD8(address, v16);
            break;

      case X_ListHosts:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_SetAccessControl:
            ENUM8(access_mode);
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_SetCloseDownMode:
            ENUM8(close_down_mode);
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_KillClient:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            CARD32(resource);
            break;

      case X_RotateProperties:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            v16 = requestLength(tvb, offsetp, t, byte_order);
            WINDOW(window);
            CARD16(property_number);
            INT16(delta);
            LISTofATOM(properties, (v16 - 12));
            break;

      case X_ForceScreenSaver:
            ENUM8(screen_saver_mode);
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_SetPointerMapping:
            v8 = FIELD8(map_length);
            requestLength(tvb, offsetp, t, byte_order);
            LISTofCARD8(map, v8);
            PAD();
            break;

      case X_GetPointerMapping:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_SetModifierMapping:
            v8 = FIELD8(keycodes_per_modifier);
            requestLength(tvb, offsetp, t, byte_order);
            LISTofKEYCODE(state->modifiermap, keycodes, v8);
            break;

      case X_GetModifierMapping:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;

      case X_NoOperation:
            proto_tree_add_item(t, hf_x11_unused, tvb, *offsetp, 1, ENC_NA);
            *offsetp += 1;
            requestLength(tvb, offsetp, t, byte_order);
            break;
      default:
            tryExtension(opcode, tvb, pinfo, offsetp, t, state, byte_order);
            break;
      }

      if ((left = tvb_reported_length_remaining(tvb, offset)) > 0)
            UNDECODED(left);
}
