static guint
fEnumeratedTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label,
        const value_string *vs)
{
    return fEnumeratedTagSplit (tvb, tree, offset, label, vs, 0);
}
