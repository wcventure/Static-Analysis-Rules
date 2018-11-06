static int
ndps_string(tvbuff_t* tvb, int hfinfo, proto_tree *ndps_tree, int offset, char *stringval, size_t buflen)
{
        int     foffset = offset;
        guint32 str_length;
        char    buffer[1024];
        guint32 i;
        guint16 c_char;
        guint32 length_remaining = 0;
        
        if (stringval == NULL) {
                stringval = buffer;
                buflen = sizeof buffer;
        }
        str_length = tvb_get_ntohl(tvb, foffset);
        foffset += 4;
        length_remaining = tvb_length_remaining(tvb, foffset);
        if(str_length > (guint)length_remaining || str_length > 1024)
        {
                proto_tree_add_string(ndps_tree, hfinfo, tvb, foffset,
                    length_remaining + 4, "<String too long to process>");
                foffset += length_remaining;
                return foffset;
        }
        if(str_length == 0)
        {
       	    proto_tree_add_string(ndps_tree, hfinfo, tvb, offset,
                4, "<Not Specified>");
            return foffset;
        }
        for ( i = 0; i < str_length; i++ )
        {
                c_char = tvb_get_guint8(tvb, foffset );
                if (c_char<0x20 || c_char>0x7e)
                {
                        if (c_char != 0x00)
                        { 
                                c_char = 0x2e;
                                if (i < buflen - 1)
	                                stringval[i] = c_char & 0xff;
                        }
                        else
                        {
                                i--;
                                str_length--;
                        }
                }
                else
                {
                	if (i < buflen - 1)
	                        stringval[i] = c_char & 0xff;
                }
                foffset++;
                length_remaining--;
                
                if(length_remaining==1)
                {
                	i++;
                	break;
                }        
        }
        stringval[i] = '\0';
        
        str_length = tvb_get_ntohl(tvb, offset);
        proto_tree_add_string(ndps_tree, hfinfo, tvb, offset+4,
                str_length, stringval);
        foffset += align_4(tvb, foffset);
        return foffset;
}
