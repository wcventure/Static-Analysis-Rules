static void
dissect_pana_flags(proto_tree *parent_tree, tvbuff_t *tvb, int offset, guint16 flags)
{
       proto_item *flags_item=NULL;
       proto_tree *flags_tree=NULL;

       if(!parent_tree) return;

       flags_item = proto_tree_add_uint(parent_tree, hf_pana_flags, tvb,
                                             offset, 2, flags);
       flags_tree = proto_item_add_subtree(flags_item, ett_pana_flags);

       proto_tree_add_boolean(flags_tree, hf_pana_flag_r, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_R)
               proto_item_append_text(flags_item, ", Request");
       else
               proto_item_append_text(flags_item, ", Answer");
       proto_tree_add_boolean(flags_tree, hf_pana_flag_s, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_S)
               proto_item_append_text(flags_item, ", S flag set");
       proto_tree_add_boolean(flags_tree, hf_pana_flag_c, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_C)
               proto_item_append_text(flags_item, ", C flag set");
       proto_tree_add_boolean(flags_tree, hf_pana_flag_a, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_A)
               proto_item_append_text(flags_item, ", A flag set");
       proto_tree_add_boolean(flags_tree, hf_pana_flag_p, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_P)
               proto_item_append_text(flags_item, ", P flag set");
       proto_tree_add_boolean(flags_tree, hf_pana_flag_i, tvb, offset, 2, flags);
       if (flags & PANA_FLAG_I)
               proto_item_append_text(flags_item, ", I flag set");
}
