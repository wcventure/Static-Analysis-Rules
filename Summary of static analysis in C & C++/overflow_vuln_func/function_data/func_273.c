static void
dissect_option_route( guchar parm_type, int offset, guchar parm_len,
                      tvbuff_t *tvb, proto_tree *tree ) {

  guchar      next_hop = 0;
  guchar      this_hop = 0;
  guchar      netl     = 0;
  guchar      last_hop = 0;
  guchar      cnt_hops = 0;

  proto_item *ti;
  proto_tree *osi_route_tree = NULL;

  static const value_string osi_opt_route[] = {
        { 0x03, "No Network Entity Titles Recorded Yet"},
        { 0xff, "Recording Terminated !"},
        { 0,    NULL} };

  if ( parm_type == OSI_OPT_SOURCE_ROUTING ) {
    next_hop = tvb_get_guint8(tvb, offset + 1 );
    netl     = tvb_get_guint8(tvb, next_hop + 2 );
    this_hop = offset + 3;         /

    ti = proto_tree_add_text( tree, tvb, offset + next_hop, netl,
            "Source Routing: %s   ( Next Hop Highlighted In Data Buffer )",
            (tvb_get_guint8(tvb, offset) == 0) ? "Partial Source Routing" :
                                                 "Complete Source Routing"  );
  }
  else {
    last_hop = tvb_get_guint8(tvb, offset + 1 );
        /
    netl     = tvb_get_guint8(tvb, last_hop );
        /

    ti = proto_tree_add_text( tree, tvb, offset + next_hop, netl,
            "Record of Route: %s : %s",
            (tvb_get_guint8(tvb, offset) == 0) ? "Partial Source Routing" :
                                                 "Complete Source Routing" ,
            val_to_str( last_hop, osi_opt_route, "Unknown (0x%x" ) );
    if ( 255 == last_hop )
      this_hop = parm_len + 1;   /
    else
      this_hop = offset + 3;
  }
  osi_route_tree = proto_item_add_subtree( ti, ott_osi_route );

  while ( this_hop < parm_len ) {
    netl = tvb_get_guint8(tvb, this_hop + 1);
    proto_tree_add_text( osi_route_tree, tvb, offset + this_hop, netl,
                  "Hop #%3u NETL: %2u, NET: %s",
                  cnt_hops++,
                  netl,
                  print_nsap_net( tvb_get_ptr(tvb, this_hop + 1, netl), netl ) );
    this_hop += 1 + netl;
  }
}
