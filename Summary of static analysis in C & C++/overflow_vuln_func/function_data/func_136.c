static guint
fBitStringTagVS (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label,
    const value_string *src)
{
    guint8      tag_no, tag_info, tmp;
    gint        j, unused, skip;
    guint       start = offset;
    guint       offs;
    guint32     lvt, i, numberOfBytes;
    guint8      bf_arr[256];
    proto_tree* subtree = tree;
    proto_item* ti = 0;

    offs = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    numberOfBytes = lvt-1; /
    offset += offs;
    unused  = tvb_get_guint8(tvb, offset); /
    ti      = proto_tree_add_text(tree, tvb, start, offs+lvt,
                                  "%s(Bit String)", label);
    if (ti) {
        subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    }
    fTagHeaderTree(tvb, subtree, start, &tag_no, &tag_info, &lvt);
    proto_tree_add_text(subtree, tvb, offset, 1,
                "Unused bits: %u", unused);
    skip = 0;
    for (i = 0; i < numberOfBytes; i++) {
        tmp = tvb_get_guint8(tvb, (offset)+i+1);
        if (i == numberOfBytes-1) { skip = unused; }
        for (j = 0; j < 8-skip; j++) {
            if (src != NULL) {
                if (tmp & (1 << (7 - j)))
                    proto_tree_add_text(subtree, tvb,
                        offset+i+1, 1,
                        "%s = TRUE",
                        val_to_str((guint) (i*8 +j),
                            src,
                            ASHRAE_Reserved_Fmt));
                else
                    proto_tree_add_text(subtree, tvb,
                        offset+i+1, 1,
                        "%s = FALSE",
                        val_to_str((guint) (i*8 +j),
                            src,
                            ASHRAE_Reserved_Fmt));
            } else {
                bf_arr[MIN(255,(i*8)+j)] = tmp & (1 << (7 - j)) ? '1' : '0';
            }
        }
    }

    if (src == NULL) {
        bf_arr[MIN(255,numberOfBytes*8-unused)] = 0;
        proto_tree_add_text(subtree, tvb, offset, lvt, "B'%s'", bf_arr);
    }

    offset += lvt;

    return offset;
}
