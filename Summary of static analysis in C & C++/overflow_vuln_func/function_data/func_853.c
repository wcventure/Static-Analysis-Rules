static void
fIAm (tvbuff_t *tvb, proto_tree *tree, guint *offset)
{
	guint8 offs, tmp, tag_no, class_tag, i;
	guint32 lvt;
	guint32 val = 0;

	/
	fApplicationTags (tvb, tree, offset, "BACnetObjectIdentifier: ", NULL);

	/
	fApplicationTags (tvb, tree, offset, "Maximum APDU Length accepted: ", NULL);

	/
	offs = fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	(*offset) += offs + 1;	/
	for (i = 0; i < min(lvt, 4); i++) {
		tmp = tvb_get_guint8(tvb, (*offset)+i);
		val = (val << 8) + tmp;
	}
	proto_tree_add_text(tree, tvb, *offset, 1, "segmentationSupported: %s",
	    val_to_str(val, bacapp_segmentation, "Invalid (%u)"));
	(*offset)+=lvt;

	/
	(*offset) += fTagHeader (tvb, offset, &tag_no, &class_tag, &lvt);
	fUnsignedTag (tvb, tree, offset, "vendorID: ", lvt);
}
