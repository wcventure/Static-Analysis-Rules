static void
dissect_a11_radius( tvbuff_t *tvb, int offset, proto_tree *tree, int app_len)
{
  proto_item *ti;
  proto_tree *radius_tree=NULL;
  size_t     radius_len;
  guint8     radius_type;
  guint8     radius_subtype;
  int        attribute_type;
  guint      attribute_len;
  guint      offset0;
  guint      radius_offset;
  guint      i;
  guint8     *str_val;
  guint      radius_vendor_id = 0;

  /
  if (!tree) return;

  offset0 = offset;

  /
  if (tvb_reported_length_remaining(tvb, offset)  < 12) {
    return;
  }

  ti = proto_tree_add_text(tree, tvb, offset - 2, app_len, "Airlink Record");

  radius_tree = proto_item_add_subtree(ti, ett_a11_radiuses);

  /
  while (tvb_reported_length_remaining(tvb, offset) > 0
         && ((int)offset-(int)offset0) <app_len-2)
  {

    radius_type = tvb_get_guint8(tvb, offset);
    radius_len = tvb_get_guint8(tvb, offset + 1);
    if (radius_len < 2)
    {
      proto_tree_add_text(radius_tree, tvb, offset, 2,
                "Bogus RADIUS length %lu, should be >= 2",
                (unsigned long)radius_len);
      break;
    }

    if (radius_type == RADIUS_VENDOR_SPECIFIC)
    {
      if (radius_len < SKIP_HDR_LEN)
      {
        proto_tree_add_text(radius_tree, tvb, offset, radius_len,
                  "Bogus RADIUS length %lu, should be >= %u",
                  (unsigned long)radius_len, SKIP_HDR_LEN);
        offset += radius_len;
        continue;
      }
      radius_vendor_id = tvb_get_ntohl(tvb, offset +2); 

      if(radius_vendor_id != VENDOR_THE3GPP2)
      {
        proto_tree_add_text(radius_tree, tvb, offset, radius_len,
                "Unknown Vendor-specific Attribute (Vendor Id: %x)", radius_vendor_id);
        offset += radius_len;
        continue;
      }
    }
    else
    {

      /
      if(radius_type == 31)
      {
      	str_val = tvb_get_ephemeral_string(tvb,offset+2,radius_len-2);
        proto_tree_add_text(radius_tree, tvb, offset, radius_len,
            "MSID: %s", str_val);
      }
      else if (radius_type == 46)
      {
        if (radius_len < 2+4)
        {
          proto_tree_add_text(radius_tree, tvb, offset, radius_len,
                    "Bogus RADIUS length %lu, should be >= %u",
                    (unsigned long)radius_len, 2+4);
        }
        else
        {
          proto_tree_add_text(radius_tree, tvb, offset, radius_len,
              "Acct Session Time: %d",tvb_get_ntohl(tvb,offset+2));
        }
      }
      else
      {
        proto_tree_add_text(radius_tree, tvb, offset, radius_len,
              "Unknown RADIUS Attributes (Type: %d)", radius_type);
      }

      offset += radius_len;
      continue;
    }
    
    offset += SKIP_HDR_LEN;
    radius_len -= SKIP_HDR_LEN;
    radius_offset = 0;

    /

    while (radius_len > radius_offset)
    {
      if (radius_len < radius_offset + 2)
      {
        proto_tree_add_text(radius_tree, tvb, offset + radius_offset, 2,
                  "Bogus RADIUS length %lu, should be >= %u",
                  (unsigned long)(radius_len + SKIP_HDR_LEN),
                  radius_offset + 2 + SKIP_HDR_LEN);
        return;
      }

      radius_subtype = tvb_get_guint8(tvb, offset + radius_offset);	
      attribute_len = tvb_get_guint8(tvb, offset + radius_offset + 1);
      if (attribute_len < 2)
      {
        proto_tree_add_text(radius_tree, tvb, offset + radius_offset, 2,
                  "Bogus attribute length %u, should be >= 2", attribute_len);
        return;
      }
      if (attribute_len > radius_len - radius_offset)
      {
        proto_tree_add_text(radius_tree, tvb, offset + radius_offset, 2,
                  "Bogus attribute length %u, should be <= %lu",
                  attribute_len, (unsigned long)(radius_len - radius_offset));
        return;
      }

      attribute_type = -1;
      for(i = 0; i < NUM_ATTR; i++) {
        if (attrs[i].subtype == radius_subtype) {
            attribute_type = i;
            break;
        }
      }

      if ((radius_subtype == 48) &&
	(attribute_len == 0x0a))
      {
	/
        str_val = tvb_get_ephemeral_string(tvb,offset+radius_offset+2,attribute_len-2);
	proto_tree_add_text(radius_tree, tvb, offset+radius_offset,
	   attribute_len,
	  "3GPP2: ESN-48 (String) (%s)", str_val);
      }
      else if(attribute_type >= 0) {
        switch(attrs[attribute_type].data_type) {
        case ATTR_TYPE_INT:
          proto_tree_add_text(radius_tree, tvb, offset + radius_offset,
            attribute_len, "3GPP2: %s (0x%04x)", attrs[attribute_type].attrname,
            tvb_get_ntohl(tvb,offset + radius_offset + 2));
          break;
        case ATTR_TYPE_IPV4:
          proto_tree_add_text(radius_tree, tvb, offset + radius_offset,
            attribute_len, "3GPP2: %s (%s)", attrs[attribute_type].attrname,
            ip_to_str(tvb_get_ptr(tvb,offset + radius_offset + 2,4)));
          break;
        case ATTR_TYPE_TYPE:
          proto_tree_add_text(radius_tree, tvb, offset + radius_offset,
            attribute_len, "3GPP2: %s (%s)", attrs[attribute_type].attrname,
            val_to_str(tvb_get_ntohl(tvb,offset+radius_offset+2),
            a11_airlink_types,"Unknown"));
          break;
        case ATTR_TYPE_STR:
          str_val = tvb_get_ephemeral_string(tvb,offset+radius_offset+2,attribute_len-2);
          proto_tree_add_text(radius_tree, tvb, offset+radius_offset,
             attribute_len,
            "3GPP2: %s (%s)", attrs[attribute_type].attrname, str_val);
          break;
        case ATTR_TYPE_NULL:
          break;
        default:
          proto_tree_add_text(radius_tree, tvb, offset+radius_offset, attribute_len,
            "RADIUS: %s", attrs[attribute_type].attrname);
          break;
        }
      }
      else {
        proto_tree_add_text(radius_tree, tvb, offset+radius_offset, attribute_len,
          "RADIUS: Unknown 3GPP2 Attribute (Type:%d, SubType:%d)",
          radius_type,radius_subtype);
      }

      radius_offset += attribute_len;
    }
    offset += radius_len;

  }

}
