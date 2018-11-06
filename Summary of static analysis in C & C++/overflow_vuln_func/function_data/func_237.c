static void
dissect_vsif (tvbuff_t * tvb, packet_info * pinfo _U_, proto_tree * tree)
{
  proto_item *it;
  proto_tree *vsif_tree;
  guint8 type;
  guint8 length;
  guint32 value;
  guint16 vsif_len;

/
  vsif_len = tvb_length_remaining (tvb, 0);

/
  type = tvb_get_guint8 (tvb, 0);
  if (type != 0x08)
    {
      THROW (ReportedBoundsError);
    }

  length = tvb_get_guint8 (tvb, 1);
  if (length != 3)
    {
      THROW (ReportedBoundsError);
    }

  /
  value = tvb_get_ntoh24 (tvb, 2);
  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_vsif, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"VSIF Encodings");
      vsif_tree = proto_item_add_subtree (it, ett_docsis_vsif);
      proto_tree_add_item (vsif_tree, hf_docsis_vsif_vendorid, tvb, 2, 3, FALSE);

      /
      switch (value)
	{
	case VENDOR_CISCO:
	  proto_item_append_text (it, " (Cisco)");
	  dissect_cisco (tvb, vsif_tree, (guint8) vsif_len);
	  break;
	default:
	  proto_item_append_text (it, " (Unknown)");
	  proto_tree_add_item (vsif_tree, hf_docsis_vsif_vendor_unknown, tvb,
			       0, -1, FALSE);
	  break;
	}

    }				/


}
