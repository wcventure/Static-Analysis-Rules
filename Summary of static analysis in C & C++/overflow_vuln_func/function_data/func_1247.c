static void dissect_auto_rp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
        guint8 ver_type, rp_count;

        if (check_col(pinfo->cinfo, COL_PROTOCOL))
                col_set_str(pinfo->cinfo, COL_PROTOCOL, "Auto-RP");
        if (check_col(pinfo->cinfo, COL_INFO))
                col_clear(pinfo->cinfo, COL_INFO);
        
        ver_type = tvb_get_guint8(tvb, 0);
        rp_count = tvb_get_guint8(tvb, 1);
        if (check_col(pinfo->cinfo, COL_INFO))
                col_add_fstr(pinfo->cinfo, COL_INFO, "%s (v%s) for %u RP%s",
                             val_to_str(lo_nibble(ver_type), auto_rp_type_vals, "Unknown"),
                             val_to_str(hi_nibble(ver_type), auto_rp_ver_vals, "Unknown"),
                             rp_count, plurality(rp_count, "", "s"));

        if (tree) {
                proto_item *ti, *tv;
                proto_tree *auto_rp_tree, *ver_type_tree;
                int i, offset;
                guint16 holdtime;

                offset = 0;
                ti = proto_tree_add_item(tree, proto_auto_rp, tvb, offset, -1, FALSE);
                auto_rp_tree = proto_item_add_subtree(ti, ett_auto_rp);

                tv = proto_tree_add_text(auto_rp_tree, tvb, offset, 1, "Version: %s, Packet type: %s",
                                         val_to_str(hi_nibble(ver_type), auto_rp_ver_vals, "Unknown"),
                                         val_to_str(lo_nibble(ver_type), auto_rp_type_vals, "Unknown"));
                ver_type_tree = proto_item_add_subtree(tv, ett_auto_rp_ver_type);
                proto_tree_add_uint(ver_type_tree, hf_auto_rp_version, tvb, offset, 1, ver_type);
                proto_tree_add_uint(ver_type_tree, hf_auto_rp_type, tvb, offset, 1, ver_type);
                offset++;

                proto_tree_add_uint(auto_rp_tree, hf_auto_rp_count, tvb, offset, 1, rp_count);
                offset++;

                holdtime = tvb_get_ntohs(tvb, offset);
                proto_tree_add_uint_format(auto_rp_tree, hf_auto_rp_holdtime, tvb, offset, 2, holdtime,
                                           "Holdtime: %u second%s", holdtime, plurality(holdtime, "", "s"));
                offset+=2;

                proto_tree_add_text(auto_rp_tree, tvb, offset, 4, "Reserved: 0x%x", tvb_get_ntohs(tvb, offset));
                offset+=4;

                for (i = 0; i < rp_count; i++)
                        offset = do_auto_rp_map(tvb, offset, auto_rp_tree);

                if (tvb_length_remaining(tvb, offset) > 0)
                        proto_tree_add_text(tree, tvb, offset, tvb_length_remaining(tvb, offset), "Trailing junk");
        }

        return;
}
