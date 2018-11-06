static void
decode_sse(proto_tree* ext_tree, tvbuff_t* tvb, int offset, size_t ext_len)
{
    guint8 msid_len =  0;
    guint8 msid_start_offset =  0;
    guint8 msid_num_digits = 0;
    guint8 msid_index = 0;
    guint8 msid_digits[A11_MSG_MSID_LEN_MAX+2];
    const guint8* p_msid = 0;
    gboolean odd_even_ind = 0;

    /
    proto_tree_add_item(ext_tree, hf_a11_ses_ptype, tvb, offset, 2, FALSE);
    offset += 2;
    ext_len -= 2;

    /
    if (ext_len < 4)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode Session Key - SSE too short");
        return;
    }
    proto_tree_add_item(ext_tree, hf_a11_ses_key, tvb, offset, 4, FALSE);
    offset += 4;
    ext_len -= 4;


    /
    if (ext_len < 2)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode Session Id Version - SSE too short");
        return;
    }
    proto_tree_add_item(ext_tree, hf_a11_ses_sidver, tvb, offset+1, 1, FALSE);
    offset += 2;
    ext_len -= 2;


    /
    if (ext_len < 2)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode SRID - SSE too short");
        return;
    }
    proto_tree_add_item(ext_tree, hf_a11_ses_mnsrid, tvb, offset, 2, FALSE);
    offset += 2;
    ext_len -= 2;

    /
    if (ext_len < 2)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode MSID Type - SSE too short");
        return;
    }
    proto_tree_add_item(ext_tree, hf_a11_ses_msid_type, tvb, offset, 2, FALSE);
    offset += 2;
    ext_len -= 2;


    /
    if (ext_len < 1)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode MSID Length - SSE too short");
        return;
    }
    msid_len =  tvb_get_guint8(tvb, offset);
    proto_tree_add_item(ext_tree, hf_a11_ses_msid_len, tvb, offset, 1, FALSE);
    offset += 1;
    ext_len -= 1;

    /
    if (ext_len < msid_len)
    {
        proto_tree_add_text(ext_tree, tvb, offset, 0,
                    "Cannot decode MSID - SSE too short");
        return;
    }

    msid_start_offset = offset;

    if(msid_len > A11_MSG_MSID_ELEM_LEN_MAX)
    {
        p_msid = "MSID is too long";
    }else
    {
       /
       for(msid_index=0; msid_index<msid_len; msid_index++)
       {
           guint8 msid_digit = tvb_get_guint8(tvb, offset);
           offset += 1;
           ext_len -= 1;
   
           msid_digits[msid_index*2] = (msid_digit & 0x0F) + '0';
           msid_digits[(msid_index*2) + 1] = ((msid_digit & 0xF0) >> 4) + '0';
       }
   
       odd_even_ind = (msid_digits[0] == '1');
       msid_num_digits = 0;
   
       if(odd_even_ind)
       {
           msid_num_digits = ((msid_len-1) * 2) + 1;
       }else
       {
           msid_num_digits = (msid_len-1) * 2;
       }
    
       msid_digits[msid_num_digits + 1] = 0;
       p_msid = msid_digits + 1;
    }
   

    proto_tree_add_string
      (ext_tree, hf_a11_ses_msid, tvb, msid_start_offset, msid_len, p_msid);

    return;
}
