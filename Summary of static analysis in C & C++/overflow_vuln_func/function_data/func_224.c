static void
dissect_pana_avp_flags(proto_tree *parent_tree, tvbuff_t *tvb, int offset, guint16 flags)
{
       proto_item *avp_flags_item=NULL;
       proto_tree *avp_flags_tree=NULL;

       if(!parent_tree) return;

       avp_flags_item = proto_tree_add_uint(parent_tree, hf_pana_avp_flags, tvb,
                                                       offset, 2, flags);
       avp_flags_tree = proto_item_add_subtree(avp_flags_item, ett_pana_avp_flags);

       proto_tree_add_boolean(avp_flags_tree, hf_pana_avp_flag_v, tvb, offset, 2, flags);
       if (flags & PANA_AVP_FLAG_V)
               proto_item_append_text(avp_flags_item, ", Vendor");
}
