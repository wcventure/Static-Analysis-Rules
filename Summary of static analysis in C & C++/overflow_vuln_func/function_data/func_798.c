static void
fApplicationTags (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label, const value_string
 *src)
{
	guint8 offs, tag_no, class_tag, tmp, unused;
	guint64 val = 0;
	guint32 i, lvt;
	int j;
	gfloat f_val = 0.0;
	gdouble d_val = 0.0;
	guint8 *str_val;
	guint8 bf_arr[256];

	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);

	(*offset) += offs;	/

	switch (tag_no) {
		case 0:	/
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "%sNULL", LABEL(label));
			break;
		case 1:	/
			proto_tree_add_text(tree, tvb, (*offset)++, 1, "%s%s", LABEL(label), lvt == 0 ? "FALSE" : "TRUE");
			break;
		case 2:	/
			fUnsignedTag (tvb, tree, offset, label, lvt);
			break;
		case 3:	/
			fSignedTag (tvb, tree, offset, label, lvt);
			break;
		case 4:	/
			(*offset)++;
			f_val = tvb_get_ntohieee_float(tvb, *offset);
			proto_tree_add_text(tree, tvb, *offset, 4, "%s%f", LABEL(label), f_val);
			(*offset)+=4;
			break;
		case 5:	/
			(*offset)++;
			d_val = tvb_get_ntohieee_double(tvb, *offset);
			proto_tree_add_text(tree, tvb, *offset, 8, "%s%lf", LABEL(label), d_val);
			(*offset)+=8;
			break;
		case 6: /
			(*offset)++;
			proto_tree_add_text(tree, tvb, *offset-offs-1, offs+1, "%s (%d Characters)", LABEL(label), (int)lvt);
			fOctetString (tvb, tree, offset, label, lvt);
			break;
		case 7: /
			(*offset)++;
			tmp = tvb_get_guint8(tvb, *offset);
			if (tmp == 3) {
				proto_tree_add_text (tree, tvb, *offset, 4, "   String Character Set: %s", val_to_str((guint) tmp, bacapp_character_set, "Reserved by ASHRAE"));
				(*offset)+=4;
				lvt-=4;
			}
			if (tmp == 4) {
				proto_tree_add_text (tree, tvb, *offset, 2, "   String Character Set: %s", val_to_str((guint) tmp, bacapp_character_set, "Reserved by ASHRAE"));
				(*offset)+=2;
				lvt-=2;
			}
			if ((tmp != 3) && (tmp != 4)) {
				proto_tree_add_text (tree, tvb, *offset, 1, "   String Character Set: %s", val_to_str((guint) tmp, bacapp_character_set, "Reserved by ASHRAE"));
				(*offset)++;
				lvt--;
			}
			do {
				guint8 l = (guint8) min(lvt, 255);
				str_val = tvb_get_string(tvb, *offset, l);
				/
				proto_tree_add_text(tree, tvb, *offset, l, "%s'%s'", LABEL(label), str_val);
				g_free(str_val);
				lvt -= l;
				(*offset) += l;
			} while (lvt > 0);
			break;
		case 8: /
			(*offset)++;
			unused = tvb_get_guint8(tvb, *offset); /
			for (i = 0; i < (lvt-2); i++) {
				tmp = tvb_get_guint8(tvb, (*offset)+i+1);
				for (j = 0; j < 8; j++) {
					if (src != NULL) {
						if (tmp & (1 << (7 - j)))
							proto_tree_add_text(tree, tvb, (*offset)+i+1, 1, "%s%s = TRUE", LABEL(label), val_to_str((guint) (i*8 +j), src, "Reserved by ASHRAE"));
						else
							proto_tree_add_text(tree, tvb, (*offset)+i+1, 1, "%s%s = FALSE", LABEL(label), val_to_str((guint) (i*8 +j), src, "Reserved by ASHRAE"));

					} else {
						bf_arr[min(255,(i*8)+j)] = tmp & (1 << (7 - j)) ? '1' : '0';
					}
				}
			}
			tmp = tvb_get_guint8(tvb, (*offset)+lvt-1);	/
			if (src == NULL) {
				for (j = 0; j < (8 - unused); j++)
					bf_arr[min(255,((lvt-2)*8)+j)] = tmp & (1 << (7 - j)) ? '1' : '0';
				for (; j < 8; j++)
					bf_arr[min(255,((lvt-2)*8)+j)] = 'x';
				bf_arr[min(255,((lvt-2)*8)+j)] = '\0';
				proto_tree_add_text(tree, tvb, *offset, lvt, "%sB'%s'", LABEL(label), bf_arr);
			} else {
				for (j = 0; j < (8 - unused); j++) {
					if (tmp & (1 << (7 - j)))
						proto_tree_add_text(tree, tvb, (*offset)+i+1, 1, "%s%s = TRUE", LABEL(label), val_to_str((guint) (i*8 +j), src, "Reserved by ASHRAE"));
					else
						proto_tree_add_text(tree, tvb, (*offset)+i+1, 1, "%s%s = FALSE", LABEL(label), val_to_str((guint) (i*8 +j), src, "Reserved by ASHRAE"));
				}
			}
			(*offset)+=lvt;
			break;
		case 9: /
			(*offset)++;
			for (i = 0; i < min(lvt,8); i++) {
				tmp = tvb_get_guint8(tvb, (*offset)+i);
				val = (val << 8) + tmp;
			}
			if (src != NULL)
				proto_tree_add_text(tree, tvb, *offset, lvt, "%s%s", LABEL(label), val_to_str((guint) val, src, "Reserved by ASHRAE"));
			else
				proto_tree_add_text(tree, tvb, *offset, lvt, "%s%" PRIu64, LABEL(label), val);

			(*offset)+=lvt;
			break;
		case 10: /
			fDateTag (tvb, tree, offset, label, lvt);
			break;
		case 11: /
			fTimeTag (tvb, tree, offset, label, lvt);
			break;
		case 12: /
			fObjectIdentifier (tvb, tree, offset, LABEL(label));
			break;
		case 13: /
		case 14:
		case 15:
			(*offset)++;
			proto_tree_add_text(tree, tvb, *offset, lvt, "%s'reserved for ASHRAE'", LABEL(label));
			(*offset)+=lvt;
			break;
	}
}
