static guint
fAuthenticateRequest (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint lastoffset = 0;

    while (tvb_reported_length_remaining(tvb, offset) > 0) {  /
        lastoffset = offset;

        switch (fTagNo(tvb,offset)) {
        case 0: /
            offset = fUnsignedTag (tvb, tree, offset, "pseudo Random Number: ");
            break;
        case 1: /
            proto_tree_add_item(tree, hf_bacapp_invoke_id, tvb, offset++, 1, ENC_BIG_ENDIAN);
            break;
        case 2: /
            offset = fCharacterString (tvb, tree, offset, "operator Name: ");
            break;
        case 3: /
            offset = fCharacterString (tvb, tree, offset, "operator Password: ");
            break;
        case 4: /
            offset = fBooleanTag (tvb, tree, offset, "start Encyphered Session: ");
            break;
        default:
            return offset;
        }
        if (offset == lastoffset) break;     /
    }
    return offset;
}
