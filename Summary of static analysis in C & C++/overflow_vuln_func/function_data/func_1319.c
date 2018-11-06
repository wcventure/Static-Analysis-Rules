		next_tvb = tvb_new_subset(tvb, offset, param_len, param_len);
		TRY
		{
			call_dissector(acse_handle, next_tvb, pinfo, tree);
		}
		CATCH_ALL
		{
			show_exception(tvb, pinfo, tree, EXCEPT_CODE, GET_MESSAGE);
		}
		ENDTRY;
	}
}
static char*
string_to_hex(const unsigned char * in,char * out,int len)
{
	char ascii[MAXSTRING];
	int  i;
	memset(&ascii,0x00,sizeof(ascii));
for(i=0;i<len;i++)
			{
	unsigned char o_out = *(in+i);
    sprintf(out+(i<<1),"%.2x",*  (in+i));
	if(  ( (o_out) >= 'a') & ( (o_out) <='z')  ||
		 ( (o_out) >= 'A') & ( (o_out) <='Z')  ||
		 ( (o_out) >= '0') & ( (o_out) <='9')
	   )
					{
					ascii[i] = o_out;
					}
				else
					{
					ascii[i] = '.';
					}

			}
		strcat(out," ");
		strcat(out,ascii);
    return out;
}
