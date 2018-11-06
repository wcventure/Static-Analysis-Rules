static gboolean
dissect_pana(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

       guint16 pana_res;
       guint16 msg_length;
       guint16 flags;
       guint32 buffer_length;
       guint16 msg_type;
       guint16 avp_length;
       guint16 avp_offset;

       /
       buffer_length = tvb_length(tvb);

       /
       if(buffer_length < 12) {
               return FALSE;
       }

       /
       pana_res = tvb_get_ntohs(tvb, 0);
       msg_length = tvb_get_ntohs(tvb, 2);
       flags = tvb_get_ntohs(tvb, 4);
       msg_type = tvb_get_ntohs(tvb, 6);
       avp_length = msg_length-16;
       avp_offset = 16;

       /
       if(msg_length < 12) {
               return FALSE;
       }

       /
       if(msg_length != buffer_length) {
               return FALSE;
       }

       /
       if (avp_length) {
	       guint16 first_avp_length;

               if (avp_length < MIN_AVP_SIZE)
		    return FALSE;

               first_avp_length = tvb_get_ntohs(tvb, avp_offset + 4);

               if (first_avp_length < MIN_AVP_SIZE || first_avp_length > avp_length)
		    return FALSE;
       }

       /
       if(pana_res!=0){
               return FALSE;
       }

       /
       if(flags&0x03ff){
               return FALSE;
       }

       /
       if(msg_type>4 || msg_type==0){
               return FALSE;
       }


       dissect_pana_pdu(tvb, pinfo, tree);

       return TRUE;
}
