static int dissect_cops_object(tvbuff_t *tvb, packet_info *pinfo, guint8 op_code, guint32 offset, proto_tree *tree, guint16 client_type)
{
  guint16 object_len, contents_len;
  guint8 c_num, c_type;
  proto_item *ti;
  proto_tree *obj_tree;
  char *type_str;

  object_len = tvb_get_ntohs(tvb, offset);
  if (object_len < COPS_OBJECT_HDR_SIZE) {
    /
    proto_tree_add_text(tree, tvb, offset, 2,
                        "Bad COPS object length: %u, should be at least %u",
                        object_len, COPS_OBJECT_HDR_SIZE);
    return -1;
  }
  c_num = tvb_get_guint8(tvb, offset + 2);
  c_type = tvb_get_guint8(tvb, offset + 3);

  ti = proto_tree_add_uint_format(tree, hf_cops_obj_c_num, tvb, offset, object_len, c_num,
                                  "%s: %s", val_to_str(c_num, cops_c_num_vals, "Unknown"),
                                  cops_c_type_to_str(c_num, c_type));
  obj_tree = proto_item_add_subtree(ti, ett_cops_obj);

  proto_tree_add_uint(obj_tree, hf_cops_obj_len, tvb, offset, 2, object_len);
  offset += 2;

  proto_tree_add_uint(obj_tree, hf_cops_obj_c_num, tvb, offset, 1, c_num);
  offset++;

  type_str = cops_c_type_to_str(c_num, c_type);
  proto_tree_add_text(obj_tree, tvb, offset, 1, "C-Type: %s%s%u%s",
                      type_str,
                      strlen(type_str) ? " (" : "",
                      c_type,
                      strlen(type_str) ? ")" : "");
  offset++;

  contents_len = object_len - COPS_OBJECT_HDR_SIZE;
  dissect_cops_object_data(tvb, pinfo, offset, obj_tree, op_code, client_type, c_num, c_type, contents_len);

  /
  if (object_len % sizeof (guint32))
    object_len += (sizeof (guint32) - object_len % sizeof (guint32));

  return object_len;
}
