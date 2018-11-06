static void
dissect_bap_phone_delta_opt(const ip_tcp_opt *optp, tvbuff_t *tvb,
			int offset, guint length, packet_info *pinfo _U_,
			proto_tree *tree)
{
  proto_item *tf;
  proto_tree *field_tree;
  proto_item *ti;
  proto_tree *suboption_tree;
  guint8 subopt_type;
  guint8 subopt_len;
  guint8 buf[256];	/

  tf = proto_tree_add_text(tree, tvb, offset, length, "%s", optp->name);
  field_tree = proto_item_add_subtree(tf, *optp->subtree_index);

  offset += 2;
  length -= 2;

  while (length > 0) {
    subopt_type = tvb_get_guint8(tvb, offset);
    subopt_len = tvb_get_guint8(tvb, offset + 1);
    ti = proto_tree_add_text(field_tree, tvb, offset, subopt_len,
		"Sub-Option (%d byte%s)",
		subopt_len, plurality(subopt_len, "", "s"));
    suboption_tree = proto_item_add_subtree(ti, ett_bap_phone_delta_subopt);

    proto_tree_add_text(suboption_tree, tvb, offset, 1,
	"Sub-Option Type: %s (%u)",
	val_to_str(subopt_type, bap_phone_delta_subopt_vals, "Unknown"),
	subopt_type);

    proto_tree_add_text(suboption_tree, tvb, offset + 1, 1,
	"Sub-Option Length: %u", subopt_len);

    switch (subopt_type) {
    case BAP_PHONE_DELTA_SUBOPT_UNIQ_DIGIT:
      proto_tree_add_text(suboption_tree, tvb, offset + 2, 1, "Uniq Digit: %u",
			  tvb_get_guint8(tvb, offset + 2));
      break;
    case BAP_PHONE_DELTA_SUBOPT_SUBSC_NUM:
      tvb_get_nstringz0(tvb, offset + 2, subopt_len - 2, buf);
      proto_tree_add_text(suboption_tree, tvb, offset + 2, subopt_len - 2,
			  "Subscriber Number: %s", buf);
      break;
    case BAP_PHONE_DELTA_SUBOPT_PHONENUM_SUBADDR:
      tvb_get_nstringz0(tvb, offset + 2, subopt_len - 2, buf);
      proto_tree_add_text(suboption_tree, tvb, offset + 2, subopt_len - 2,
			  "Phone Number Sub Address: %s", buf);
      break;
    default:
      proto_tree_add_text(suboption_tree, tvb, offset + 2, subopt_len - 2,
			  "Unknown");
      break;
    }
    offset += subopt_len;
    length -= subopt_len;
  }
}
