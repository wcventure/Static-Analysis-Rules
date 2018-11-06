static void dissect_cops_pr_objects(tvbuff_t *tvb, guint32 offset, proto_tree *tree, guint16 pr_len)
{
  guint16 object_len, contents_len;
  guint8 s_num, s_type;
  char *type_str;
  int ret;
  proto_tree *cops_pr_tree, *obj_tree;
  proto_item *ti;

  cops_pr_tree = proto_item_add_subtree(tree, ett_cops_pr_obj);

  while (pr_len >= COPS_OBJECT_HDR_SIZE) {
    object_len = tvb_get_ntohs(tvb, offset);
    if (object_len < COPS_OBJECT_HDR_SIZE) {
      /
      proto_tree_add_text(tree, tvb, offset, 2,
                          "Bad COPS PR object length: %u, should be at least %u",
                          object_len, COPS_OBJECT_HDR_SIZE);
      return;
    }
    s_num = tvb_get_guint8(tvb, offset + 2);

    ti = proto_tree_add_uint_format(cops_pr_tree, hf_cops_obj_s_num, tvb, offset, object_len, s_num,
                                    "%s", val_to_str(s_num, cops_s_num_vals, "Unknown"));
    obj_tree = proto_item_add_subtree(cops_pr_tree, ett_cops_pr_obj);

    proto_tree_add_uint(obj_tree, hf_cops_obj_len, tvb, offset, 2, object_len);
    offset += 2;
    pr_len -= 2;

    proto_tree_add_uint(obj_tree, hf_cops_obj_s_num, tvb, offset, 1, s_num);
    offset++;
    pr_len--;

    s_type = tvb_get_guint8(tvb, offset);
    type_str = val_to_str(s_type, cops_s_type_vals, "Unknown");
    proto_tree_add_text(obj_tree, tvb, offset, 1, "S-Type: %s%s%u%s",
                        type_str,
                        strlen(type_str) ? " (" : "",
                        s_type,
                        strlen(type_str) ? ")" : "");
    offset++;
    pr_len--;

    contents_len = object_len - COPS_OBJECT_HDR_SIZE;
    ret = dissect_cops_pr_object_data(tvb, offset, obj_tree, s_num, s_type, contents_len);
    if (ret < 0)
      break;

    /
    if (object_len % sizeof (guint32))
      object_len += (sizeof (guint32) - object_len % sizeof (guint32));

    pr_len -= object_len - COPS_OBJECT_HDR_SIZE;
    offset += object_len - COPS_OBJECT_HDR_SIZE;
  }
}
