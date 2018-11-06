static int
dissect_qnet6_lr(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, gint * poffset, guint encoding)
{
#define QNET6_LR_PAIRS 6
  proto_item   *ti;
  proto_tree   *stree, *srctree, *dstree, *sstree = NULL;
  guint32       total_len, off, len, rlen;
  gint          lr_start, i, hf_index_off = -1, hf_index_len = -1, hf_index = -1;
  guint8        type;
  guint8 const *p, *name[QNET6_LR_PAIRS];

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "QNET_LR");

  /
  rlen = tvb_reported_length_remaining(tvb, *poffset + QNX_QNET6_LR_PKT_SIZE);

  lr_start = *poffset;
  ti = proto_tree_add_item(tree, proto_qnet6_lr, tvb, *poffset, -1, ENC_NA);
  stree = proto_item_add_subtree(ti, ett_qnet6_lr);

  /
  proto_tree_add_item(stree, hf_qnet6_lr_ver, tvb, (*poffset)++, 1, ENC_BIG_ENDIAN);
  (*poffset)++; /
  /
  type = tvb_get_guint8(tvb, *poffset);
  proto_tree_add_item(stree, hf_qnet6_lr_type, tvb, (*poffset)++, 1, ENC_BIG_ENDIAN);
  (*poffset)++; /

  /
  total_len = tvb_get_guint32(tvb, *poffset, encoding);
  proto_tree_add_uint(stree, hf_qnet6_lr_total_len, tvb, *poffset, 4, total_len);
  *poffset += 4;

  ti = proto_tree_add_string(stree, hf_qnet6_lr_src, tvb, *poffset, 4 * 6, "source node information");
  srctree = proto_item_add_subtree(ti, ett_qnet6_lr_src);
  ti = proto_tree_add_string(stree, hf_qnet6_lr_dst, tvb, *poffset + 4 * 6, 4 * 6, "destination node information");
  dstree = proto_item_add_subtree(ti, ett_qnet6_lr_dst);
  rlen = MIN(rlen, total_len);

  for (i = 0; i < QNET6_LR_PAIRS; i++)
    {
      if (i < 3)
        stree = srctree;
      else
        stree = dstree;

      switch (i)
        {
        case 0:
          hf_index_off = hf_qnet6_lr_src_name_off;
          hf_index_len = hf_qnet6_lr_src_name_len;
          hf_index = hf_qnet6_lr_src_name_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "name");
          break;
        case 1:
          hf_index_off = hf_qnet6_lr_src_domain_off;
          hf_index_len = hf_qnet6_lr_src_domain_len;
          hf_index = hf_qnet6_lr_src_domain_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "domain");
          break;
        case 2:
          hf_index_off = hf_qnet6_lr_src_addr_off;
          hf_index_len = hf_qnet6_lr_src_addr_len;
          hf_index = hf_qnet6_lr_src_addr_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "address");
          break;
        case 3:
          hf_index_off = hf_qnet6_lr_dst_name_off;
          hf_index_len = hf_qnet6_lr_dst_name_len;
          hf_index = hf_qnet6_lr_dst_name_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "name");
          break;
        case 4:
          hf_index_off = hf_qnet6_lr_dst_domain_off;
          hf_index_len = hf_qnet6_lr_dst_domain_len;
          hf_index = hf_qnet6_lr_dst_domain_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "domain");
          break;
        case QNET6_LR_PAIRS - 1:
          hf_index_off = hf_qnet6_lr_dst_addr_off;
          hf_index_len = hf_qnet6_lr_dst_addr_len;
          hf_index = hf_qnet6_lr_dst_addr_generated;
          sstree = proto_tree_add_subtree(stree, tvb, *poffset, 4 * 2,
              ett_qnet6_lr_src_name_subtree, NULL, "address");
          break;
        }

      off = tvb_get_guint32(tvb, *poffset, encoding);
      proto_tree_add_item(sstree, hf_index_off, tvb, *poffset, 4, encoding);
      *poffset += 4;

      len = tvb_get_guint32(tvb, *poffset, encoding);
      proto_tree_add_item(sstree, hf_index_len, tvb, *poffset, 4, encoding);
      *poffset += 4;

      if ((off <= rlen) && (len <= rlen))
        { /
          name[i] = tvb_get_string_enc(wmem_packet_scope(),
                                       tvb,
                                       lr_start + off + QNX_QNET6_LR_PKT_SIZE /,
                                       len,
                                       ENC_ASCII|ENC_NA);
          /
          if (i != 2 && i != QNET6_LR_PAIRS - 1)
            {
              ti = proto_tree_add_string(sstree, hf_index, tvb, lr_start + off + QNX_QNET6_LR_PKT_SIZE, len, name[i]);
              PROTO_ITEM_SET_GENERATED(ti);
            }
          else
            {
              p = name[i];
              if (strlen(p) && (*(p + 1) == QNET_LR_SA_FAMILY_MAC))
                {
                  ti = proto_tree_add_string(sstree, hf_index, tvb, lr_start + off + QNX_QNET6_LR_PKT_SIZE, len, p+2);
                  PROTO_ITEM_SET_GENERATED(ti);
                }
            }
        }
      else
        {
          name[i] = NULL;
        }
    }

  switch (type)
    {
    case QNET_LR_TYPE_REQUEST:
      p = name[2];
      if (p && strlen(p) && (*(p + 1) == QNET_LR_SA_FAMILY_MAC))
        {
          col_add_fstr(pinfo->cinfo, COL_INFO,
                        "Who is \"%s.%s\"? Tell \"%s.%s\"@%02x:%02x:%02x:%02x:%02x:%02x",
                        name[3] ? (const char*)name[3] : "?", name[4] ? (const char*)name[4] : "?",
                        name[0] ? (const char*)name[0] : "?", name[1] ? (const char*)name[1] : "?",
                        *(p + 2), *(p + 3), *(p + 4),
                        *(p + 5), *(p + 6), *(p + 7));
        }
      break;
    case QNET_LR_TYPE_REPLY:
      p = name[2];
      if (p && strlen(p) && (*(p + 1) == QNET_LR_SA_FAMILY_MAC))
        {
          col_add_fstr(pinfo->cinfo, COL_INFO,
                        "To \"%s.%s\", \"%s.%s\" is at %02x:%02x:%02x:%02x:%02x:%02x",
                        name[3] ? (const char*)name[3] : "?", name[4] ? (const char*)name[4] : "?",
                        name[0] ? (const char*)name[0] : "?", name[1] ? (const char*)name[1] : "?",
                        *(p + 2), *(p + 3), *(p + 4),
                        *(p + 5), *(p + 6), *(p + 7));
        }
      break;
    default:
      col_add_fstr(pinfo->cinfo, COL_INFO, "Unknown LR Type");
    }

  return *poffset - lr_start;
}